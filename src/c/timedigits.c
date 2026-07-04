#include <pebble.h>
#include "timedigits.h"
#include "_globals.h"
#include "settings.h"
#include "helpers.h"
#include "window.h"
#include "fonts.h"
#include "bluetooth.h"
#include "battery.h"

#include <pebble-fctx/fctx.h>
#include <pebble-fctx/fpath.h>
#include <pebble-fctx/ffont.h>

enum {t_sep, t_ampm};
static TextLayer * t_layer[2] = {NULL};
static Layer *center_layer;

static AppTimer *blink_timer;
static char time_text[] = "0000";
static int16_t cached_date_char_height = 0;

static Layer *fctx_clock_layer;

#if DEBUG_DRAW_LINES
static Layer *debug_lines_layer;
static int16_t debug_virtual_top_screen = 0;
static int16_t debug_virtual_bottom_screen = 0;
#endif
static char fctx_hour_text[3] = "12";
static char fctx_min_text[3] = "59";
static char fctx_sec_text[3] = "00";
static char fctx_date_text[20] = "MON 31";
static bool s_was_24h = false;


typedef struct  __attribute__((__packed__)){
  int16_t left;
  int16_t top;
  int16_t width;
  int16_t hight;
  char*   text;
  uint8_t color;
  uint8_t font;
} digit_data_t;

enum {f_big, f_sec};

static digit_data_t dynamic_digit_data[14];
static int16_t cached_big_em = 108;
static int16_t cached_small_em = 30;

static inline const FontLayoutParams *font_layout(int idx) {
  return &font_layouts[idx < 0 || idx >= 3 ? 0 : idx];
}

/*
t1  Date
t2  AM/PM/24H Indicator
t3  Clock Shadow
t4  Clock
*/

static void calculate_dynamic_positions();
#if DEBUG_DRAW_LINES
static void debug_lines_update_proc(Layer *l, GContext *ctx) {
  (void)l;
  graphics_context_set_stroke_color(ctx, GColorRed);
  graphics_context_set_stroke_width(ctx, 1);
  graphics_draw_line(ctx, GPoint(0, debug_virtual_top_screen), GPoint(200, debug_virtual_top_screen));
  graphics_draw_line(ctx, GPoint(0, debug_virtual_bottom_screen), GPoint(200, debug_virtual_bottom_screen));
}
#endif

static void calculate_dynamic_positions() {
  GRect bounds = FULLSCREEN;
  int16_t screen_w = bounds.size.w;

  // Measure glyph widths for active fonts
  GSize big_size = graphics_text_layout_get_content_size("8", font_big, GRect(0, 0, screen_w, 228), GTextOverflowModeFill, GTextAlignmentCenter);
  GSize sep_size = graphics_text_layout_get_content_size(":", font_big, GRect(0, 0, screen_w, 228), GTextOverflowModeFill, GTextAlignmentCenter);
  GSize small_size = graphics_text_layout_get_content_size("8", font_small, GRect(0, 0, screen_w, 228), GTextOverflowModeFill, GTextAlignmentCenter);

  int16_t dh = big_size.h > 0 ? big_size.h : 108;
  int16_t small_h = small_size.h > 0 ? small_size.h : 30;
  int16_t small_dw = small_size.w > 0 ? small_size.w : 15;

  cached_big_em = dh;
  cached_small_em = small_h;

// Positioning uses measured layer widths directly — consistent, no clipping
  int16_t layer_dw = big_size.w > 0 ? big_size.w : 49;
  int16_t layer_sw = sep_size.w > 0 ? sep_size.w : layer_dw;
  int16_t layer_small_dw = small_dw;
  bool has_seconds = (global_settings.Seconds && !powerSaveEngaged);

  int16_t gap = has_seconds ? font_layout(global_settings.FontFaceDigital)->ss_gap : 0;
  int16_t total_w = has_seconds ? (4 * layer_dw + layer_sw + gap + 2 * layer_small_dw) : (4 * layer_dw + layer_sw);
  int16_t start_x = (screen_w - total_w) / 2;
  if (start_x < 0) start_x = 0;

  const FontLayoutParams *fl = font_layout(global_settings.FontFaceDigital);
  start_x += fl->h_offset;
  int16_t panel_top = BACKGROUND_PANEL.origin.y - TIMEDIGITS_CENTER.origin.y;
  int16_t panel_bottom = BACKGROUND_PANEL.origin.y + BACKGROUND_PANEL.size.h - TIMEDIGITS_CENTER.origin.y;
  int16_t virtual_top = panel_top + TIMEDIGITS_VIRTUAL_TOP_OFFSET;
  int16_t virtual_bottom = panel_bottom - TIMEDIGITS_VIRTUAL_BOTTOM_OFFSET;
  int16_t virtual_center = (virtual_top + virtual_bottom) / 2;
#if DEBUG_DRAW_LINES
  debug_virtual_top_screen = virtual_top + TIMEDIGITS_CENTER.origin.y;
  debug_virtual_bottom_screen = virtual_bottom + TIMEDIGITS_CENTER.origin.y;
#endif
  int16_t y_big = virtual_center + fl->group_center_dy - dh / 2;
  int16_t y_small = y_big + fl->ss_dy;
  int16_t x = start_x;

  DBG_LOG("=== CALCULATE_DYNAMIC_POSITIONS ===");
  DBG_LOG("  bounds: %d,%d %dx%d", bounds.origin.x, bounds.origin.y, bounds.size.w, bounds.size.h);
  DBG_LOG("  GFont big '8': %dx%d  sep ':': %dx%d  small '8': %dx%d", big_size.w, big_size.h, sep_size.w, sep_size.h, small_size.w, small_size.h);
  DBG_LOG("  cached_big_em=%d cached_small_em=%d", cached_big_em, cached_small_em);
  DBG_LOG("  layer_dw=%d layer_sw=%d layer_small_dw=%d", layer_dw, layer_sw, layer_small_dw);
  DBG_LOG("  total_w=%d start_x=%d gap=%d", total_w, start_x - fl->h_offset, gap);
  DBG_LOG("  font_layout[%d]: group_center_dy=%d ss_dy=%d ss_gap=%d date_bottom_dy=%d h_offset=%d", global_settings.FontFaceDigital, fl->group_center_dy, fl->ss_dy, fl->ss_gap, fl->date_bottom_dy, fl->h_offset);
  DBG_LOG("  y_big=%d y_small=%d has_seconds=%d", y_big, y_small, has_seconds);
  DBG_LOG("  --- VERTICAL LAYOUT ---");
  DBG_LOG("  HH:MM: top=%d h=%d bottom=%d center=%d", y_big, dh, y_big + dh, y_big + dh / 2);
  if (has_seconds) {
    DBG_LOG("  SS: top=%d h=%d bottom=%d center=%d", y_small, small_h, y_small + small_h, y_small + small_h / 2);
  }
  DBG_LOG("  BACKGROUND_PANEL: %d,%d %dx%d", BACKGROUND_PANEL.origin.x, BACKGROUND_PANEL.origin.y, BACKGROUND_PANEL.size.w, BACKGROUND_PANEL.size.h);
  DBG_LOG("  panel_top=%d panel_bottom=%d virtual_top=%d virtual_bottom=%d virtual_center=%d", panel_top, panel_bottom, virtual_top, virtual_bottom, virtual_center);

  // Shadow digits (indices 0-4 or 0-6)
  dynamic_digit_data[0]  = (digit_data_t){x, y_big, layer_dw, dh, "8", c_t3, f_big};
  dynamic_digit_data[1]  = (digit_data_t){x + layer_dw, y_big, layer_dw, dh, "8", c_t3, f_big};
  dynamic_digit_data[2]  = (digit_data_t){x + 2*layer_dw, y_big, layer_sw, dh, ":", c_t3, f_big};
  dynamic_digit_data[3]  = (digit_data_t){x + 2*layer_dw + layer_sw, y_big, layer_dw, dh, "8", c_t3, f_big};
  dynamic_digit_data[4]  = (digit_data_t){x + 3*layer_dw + layer_sw, y_big, layer_dw, dh, "8", c_t3, f_big};

  // Clock digits (indices 7-11 or 7-13): same layout, different colors
  dynamic_digit_data[7]  = (digit_data_t){x, y_big, layer_dw, dh, "8", c_t4, f_big};
  dynamic_digit_data[8]  = (digit_data_t){x + layer_dw, y_big, layer_dw, dh, "8", c_t4, f_big};
  dynamic_digit_data[9]  = (digit_data_t){x + 2*layer_dw, y_big, layer_sw, dh, ":", c_t4, f_big};
  dynamic_digit_data[10] = (digit_data_t){x + 2*layer_dw + layer_sw, y_big, layer_dw, dh, "8", c_t4, f_big};
  dynamic_digit_data[11] = (digit_data_t){x + 3*layer_dw + layer_sw, y_big, layer_dw, dh, "8", c_t4, f_big};

  if (has_seconds) {
    // Seconds digits (indices 5-6 shadow, 12-13 clock)
    dynamic_digit_data[5]  = (digit_data_t){x + 4*layer_dw + layer_sw + gap, y_small, layer_small_dw, small_h, "8", c_t3, f_sec};
    dynamic_digit_data[6]  = (digit_data_t){x + 4*layer_dw + layer_sw + gap + layer_small_dw, y_small, layer_small_dw, small_h, "8", c_t3, f_sec};
    dynamic_digit_data[12] = (digit_data_t){x + 4*layer_dw + layer_sw + gap, y_small, layer_small_dw, small_h, "8", c_t4, f_sec};
    dynamic_digit_data[13] = (digit_data_t){x + 4*layer_dw + layer_sw + gap + layer_small_dw, y_small, layer_small_dw, small_h, "8", c_t4, f_sec};
  }

  // Log all digit positions
  int n_digits = has_seconds ? 14 : 5;
  for (int i = 0; i < n_digits; i++) {
    if (i >= 5 && i < 7 && !has_seconds) continue;
    if (i >= 7 && i < 14 && !has_seconds) continue;
    DBG_LOG("  digit[%d]: x=%d y=%d w=%d h=%d", i, dynamic_digit_data[i].left, dynamic_digit_data[i].top, dynamic_digit_data[i].width, dynamic_digit_data[i].hight);
  }
}


static void fctx_update_proc(Layer *l, GContext *ctx) {
  (void)l;
  if (!get_fctx_font() || !center_layer || !t_layer[t_sep]) return;

  FContext fctx;
  fctx_enable_aa(global_settings.UseAntialiasing);
  fctx_init_context(&fctx, ctx);
  fctx_set_color_bias(&fctx, global_settings.ColorBias);

  bool has_seconds = (global_settings.Seconds && !powerSaveEngaged);

  FFont *fctx_f = get_fctx_font();

  bool hide_first = !clock_is_24h_style() && (time_text[0] == '0');
  bool sep_hidden = layer_get_hidden(text_layer_get_layer(t_layer[t_sep]));

  char d1[] = {time_text[0], 0};
  char d2[] = {time_text[1], 0};
  char d3[] = {time_text[2], 0};
  char d4[] = {time_text[3], 0};
  char sep[] = ":";
  char s1[] = {fctx_sec_text[0], 0};
  char s2[] = {fctx_sec_text[1], 0};

  int start = has_seconds ? 7 : 0;

  GPoint window_offset = layer_get_frame(my_window_layer).origin;
  GPoint center_origin = layer_get_frame(center_layer).origin;
  center_origin.x += window_offset.x;
  center_origin.y += window_offset.y;
  #define FCTX_X(d) (dynamic_digit_data[d].left + dynamic_digit_data[d].width + center_origin.x)
  #define FCTX_Y(d) (dynamic_digit_data[d].top + center_origin.y)

  fctx_set_fill_color(&fctx, color_helper(colors[c_t3], global_settings.Invert));
  fctx_set_text_em_height(&fctx, fctx_f, cached_big_em);
  fctx_begin_fill(&fctx);

  if (!has_seconds) {
    FPoint p;
    p.x = INT_TO_FIXED(FCTX_X(0));
    p.y = INT_TO_FIXED(FCTX_Y(0) + dynamic_digit_data[0].hight / 2);
    fctx_set_offset(&fctx, p);
    fctx_draw_string(&fctx, "8", fctx_f, GTextAlignmentRight, FTextAnchorMiddle);

    p.x = INT_TO_FIXED(FCTX_X(1));
    p.y = INT_TO_FIXED(FCTX_Y(1) + dynamic_digit_data[1].hight / 2);
    fctx_set_offset(&fctx, p);
    fctx_draw_string(&fctx, "8", fctx_f, GTextAlignmentRight, FTextAnchorMiddle);

    p.x = INT_TO_FIXED(FCTX_X(2));
    p.y = INT_TO_FIXED(FCTX_Y(2) + dynamic_digit_data[2].hight / 2);
    fctx_set_offset(&fctx, p);
    fctx_draw_string(&fctx, ":", fctx_f, GTextAlignmentRight, FTextAnchorMiddle);

    p.x = INT_TO_FIXED(FCTX_X(3));
    p.y = INT_TO_FIXED(FCTX_Y(3) + dynamic_digit_data[3].hight / 2);
    fctx_set_offset(&fctx, p);
    fctx_draw_string(&fctx, "8", fctx_f, GTextAlignmentRight, FTextAnchorMiddle);

    p.x = INT_TO_FIXED(FCTX_X(4));
    p.y = INT_TO_FIXED(FCTX_Y(4) + dynamic_digit_data[4].hight / 2);
    fctx_set_offset(&fctx, p);
    fctx_draw_string(&fctx, "8", fctx_f, GTextAlignmentRight, FTextAnchorMiddle);
  } else {
    FPoint p;
    for (int i = 0; i < 5; i++) {
      p.x = INT_TO_FIXED(FCTX_X(i));
      p.y = INT_TO_FIXED(FCTX_Y(i) + dynamic_digit_data[i].hight / 2);
      fctx_set_offset(&fctx, p);
      const char *txt = (i == 2) ? ":" : "8";
      fctx_draw_string(&fctx, txt, fctx_f, GTextAlignmentRight, FTextAnchorMiddle);
    }
    fctx_set_text_em_height(&fctx, fctx_f, cached_small_em);
    for (int i = 5; i < 7; i++) {
      p.x = INT_TO_FIXED(FCTX_X(i));
      p.y = INT_TO_FIXED(FCTX_Y(i) + dynamic_digit_data[i].hight / 2);
      fctx_set_offset(&fctx, p);
      fctx_draw_string(&fctx, "8", fctx_f, GTextAlignmentRight, FTextAnchorMiddle);
    }
  }
  fctx_end_fill(&fctx);

  fctx_set_text_em_height(&fctx, fctx_f, cached_big_em);
  fctx_set_fill_color(&fctx, color_helper(colors[c_t4], global_settings.Invert));
  fctx_begin_fill(&fctx);

  if (!has_seconds) {
    FPoint p;
    int idx = 0;
    if (!hide_first) {
      p.x = INT_TO_FIXED(FCTX_X(start + idx));
      p.y = INT_TO_FIXED(FCTX_Y(start + idx) + dynamic_digit_data[start + idx].hight / 2);
      fctx_set_offset(&fctx, p);
      fctx_draw_string(&fctx, d1, fctx_f, GTextAlignmentRight, FTextAnchorMiddle);
    }
    idx++;
    p.x = INT_TO_FIXED(FCTX_X(start + idx));
    p.y = INT_TO_FIXED(FCTX_Y(start + idx) + dynamic_digit_data[start + idx].hight / 2);
    fctx_set_offset(&fctx, p);
    fctx_draw_string(&fctx, d2, fctx_f, GTextAlignmentRight, FTextAnchorMiddle);
    idx++;
    if (!sep_hidden) {
      p.x = INT_TO_FIXED(FCTX_X(start + idx));
      p.y = INT_TO_FIXED(FCTX_Y(start + idx) + dynamic_digit_data[start + idx].hight / 2);
      fctx_set_offset(&fctx, p);
      fctx_draw_string(&fctx, sep, fctx_f, GTextAlignmentRight, FTextAnchorMiddle);
    }
    idx++;
    p.x = INT_TO_FIXED(FCTX_X(start + idx));
    p.y = INT_TO_FIXED(FCTX_Y(start + idx) + dynamic_digit_data[start + idx].hight / 2);
    fctx_set_offset(&fctx, p);
    fctx_draw_string(&fctx, d3, fctx_f, GTextAlignmentRight, FTextAnchorMiddle);
    idx++;
    p.x = INT_TO_FIXED(FCTX_X(start + idx));
    p.y = INT_TO_FIXED(FCTX_Y(start + idx) + dynamic_digit_data[start + idx].hight / 2);
    fctx_set_offset(&fctx, p);
    fctx_draw_string(&fctx, d4, fctx_f, GTextAlignmentRight, FTextAnchorMiddle);
  } else {
    FPoint p;
    int idx = 0;
    if (!hide_first) {
      p.x = INT_TO_FIXED(FCTX_X(start + idx));
      p.y = INT_TO_FIXED(FCTX_Y(start + idx) + dynamic_digit_data[start + idx].hight / 2);
      fctx_set_offset(&fctx, p);
      fctx_draw_string(&fctx, d1, fctx_f, GTextAlignmentRight, FTextAnchorMiddle);
    }
    idx++;
    p.x = INT_TO_FIXED(FCTX_X(start + idx));
    p.y = INT_TO_FIXED(FCTX_Y(start + idx) + dynamic_digit_data[start + idx].hight / 2);
    fctx_set_offset(&fctx, p);
    fctx_draw_string(&fctx, d2, fctx_f, GTextAlignmentRight, FTextAnchorMiddle);
    idx++;
    if (!sep_hidden) {
      p.x = INT_TO_FIXED(FCTX_X(start + idx));
      p.y = INT_TO_FIXED(FCTX_Y(start + idx) + dynamic_digit_data[start + idx].hight / 2);
      fctx_set_offset(&fctx, p);
      fctx_draw_string(&fctx, sep, fctx_f, GTextAlignmentRight, FTextAnchorMiddle);
    }
    idx++;
    p.x = INT_TO_FIXED(FCTX_X(start + idx));
    p.y = INT_TO_FIXED(FCTX_Y(start + idx) + dynamic_digit_data[start + idx].hight / 2);
    fctx_set_offset(&fctx, p);
    fctx_draw_string(&fctx, d3, fctx_f, GTextAlignmentRight, FTextAnchorMiddle);
    idx++;
    p.x = INT_TO_FIXED(FCTX_X(start + idx));
    p.y = INT_TO_FIXED(FCTX_Y(start + idx) + dynamic_digit_data[start + idx].hight / 2);
    fctx_set_offset(&fctx, p);
    fctx_draw_string(&fctx, d4, fctx_f, GTextAlignmentRight, FTextAnchorMiddle);
    idx++;
    fctx_set_text_em_height(&fctx, fctx_f, cached_small_em);
    p.x = INT_TO_FIXED(FCTX_X(start + idx));
    p.y = INT_TO_FIXED(FCTX_Y(start + idx) + dynamic_digit_data[start + idx].hight / 2);
    fctx_set_offset(&fctx, p);
    fctx_draw_string(&fctx, s1, fctx_f, GTextAlignmentRight, FTextAnchorMiddle);
    idx++;
    p.x = INT_TO_FIXED(FCTX_X(start + idx));
    p.y = INT_TO_FIXED(FCTX_Y(start + idx) + dynamic_digit_data[start + idx].hight / 2);
    fctx_set_offset(&fctx, p);
    fctx_draw_string(&fctx, s2, fctx_f, GTextAlignmentRight, FTextAnchorMiddle);
  }
  fctx_end_fill(&fctx);
  #undef FCTX_X
  #undef FCTX_Y

  fctx_set_text_em_height(&fctx, fctx_f, cached_small_em);
  fctx_set_fill_color(&fctx, color_helper(colors[c_t1], global_settings.Invert));
  fctx_begin_fill(&fctx);
  FPoint dp;
  int16_t date_ch = cached_date_char_height > 0 ? cached_date_char_height : 18;
  int16_t panel_bottom_fctx = BACKGROUND_PANEL.origin.y + BACKGROUND_PANEL.size.h - TIMEDIGITS_CENTER.origin.y;
  int16_t date_x = TIMEDIGITS_DATE.origin.x + 1;
  int16_t date_y = panel_bottom_fctx + font_layout(global_settings.FontFaceDigital)->date_bottom_dy - date_ch;
  int16_t date_w = TIMEDIGITS_DATE.size.w;
  int16_t date_h = date_ch;
  dp.x = INT_TO_FIXED(date_x + center_origin.x + date_w);
  dp.y = INT_TO_FIXED(date_y + center_origin.y + date_h / 2);
  fctx_set_offset(&fctx, dp);
  fctx_draw_string(&fctx, fctx_date_text, fctx_f, GTextAlignmentRight, FTextAnchorMiddle);

  fctx_end_fill(&fctx);
  fctx_deinit_context(&fctx);
}


void timedigits_settings_callback() {

  calculate_dynamic_positions();
  cached_date_char_height = graphics_text_layout_get_content_size("MON 31", font_small, GRect(0, 0, 186, 40), GTextOverflowModeFill, GTextAlignmentRight).h;
  if (cached_date_char_height <= 0) cached_date_char_height = 18;

  text_layer_set_text_color(t_layer[t_ampm], color_helper(colors[c_t2], global_settings.Invert));

  tick_timer_service_unsubscribe();
  if (blink_timer) {
    app_timer_cancel(blink_timer);
    blink_timer = NULL;
  }

  //Somebody set us up the CLOCK
  time_t now = time(NULL);
  struct tm *tick_time = localtime(&now);

  //Tick, Tick, Tick, BOOM!
  handle_tick(tick_time, DAY_UNIT + HOUR_UNIT + MINUTE_UNIT + SECOND_UNIT);
  s_was_24h = clock_is_24h_style();

  if((global_settings.Blink || global_settings.Seconds) && !powerSaveEngaged) {
    tick_timer_service_subscribe(SECOND_UNIT, handle_tick);
  }
  else {
    tick_timer_service_subscribe(MINUTE_UNIT, handle_tick);
    layer_set_hidden(text_layer_get_layer(t_layer[t_sep]), false);
  }
}

static void blink_timer_callback(void *data) {
  if (!t_layer[t_sep]) return;
  layer_set_hidden(text_layer_get_layer(t_layer[t_sep]), true);
  layer_mark_dirty(fctx_clock_layer);
}

void handle_tick(struct tm *tick_time, TimeUnits units_changed) {

  if ((units_changed & SECOND_UNIT) && !powerSaveEngaged) {

    if((global_settings.Blink == BLINK_ON) && !powerSaveEngaged) {
      layer_set_hidden(text_layer_get_layer(t_layer[t_sep]), !(tick_time->tm_sec%2));
    }
    else if (global_settings.Blink == BLINK_OFF) {
      layer_set_hidden(text_layer_get_layer(t_layer[t_sep]), false);
    }
    if((global_settings.Blink == BLINK_DOUBLE_RATE) && !powerSaveEngaged) {
      layer_set_hidden(text_layer_get_layer(t_layer[t_sep]), false);
      app_timer_cancel(blink_timer);
      blink_timer = app_timer_register(500, blink_timer_callback, NULL);
    }
    else if (blink_timer) {
      app_timer_cancel(blink_timer);
      blink_timer = NULL;
    }
    if (global_settings.Blink) layer_mark_dirty(fctx_clock_layer);
    if (global_settings.Seconds) {
      char digit5[] = "0";
      char digit6[] = "0";
      digit5[0] = '0'+ tick_time->tm_sec /10;
      digit6[0] = '0'+ tick_time->tm_sec %10;
      fctx_sec_text[0] = digit5[0];
      fctx_sec_text[1] = digit6[0];
      fctx_sec_text[2] = 0;
      layer_mark_dirty(fctx_clock_layer);
    }
  }  //SECOND_UNIT

  if(units_changed & MINUTE_UNIT) {

    bool is_24h = clock_is_24h_style();
    if (is_24h) {
      strftime(time_text, sizeof(time_text), "%H%M", tick_time);
    }
    else {
      strftime(time_text, sizeof(time_text), "%I%M", tick_time);
    }
    if (is_24h != s_was_24h) {
      s_was_24h = is_24h;
      text_layer_set_text(t_layer[t_ampm], is_24h ? "24H" : (tick_time->tm_hour >= 12 ? "PM" : "AM"));
    }
    fctx_hour_text[0] = time_text[0];
    fctx_hour_text[1] = time_text[1];
    fctx_hour_text[2] = 0;
    fctx_min_text[0] = time_text[2];
    fctx_min_text[1] = time_text[3];
    fctx_min_text[2] = 0;
    layer_mark_dirty(fctx_clock_layer);

    timed_colorset(tick_time->tm_hour,tick_time->tm_min);

    if (global_settings.PowerSave==1) {
      bool isPowerSave=(setting_is_power_save(tick_time->tm_hour,tick_time->tm_min));
      if(isPowerSave && !powerSaveEngaged) {
        //we should save power
        powerSaveEngaged = true;
        bluetooth_deinit();
        battery_deinit();
        update_settings();
      }
      else if (!isPowerSave && powerSaveEngaged) {
        //we should stop saving power
        powerSaveEngaged = false;
        /*
        // Original: only cleared powerSaveEngaged if Blink or Seconds was ON.
        // Unclear why this guard existed — possibly an attempt to avoid
        // re-engaging power save when SECOND_UNIT ticks were needed.
        // Keeping commented in case we observe a regression and need to
        // understand the original intent.
        if (global_settings.Blink || global_settings.Seconds) {
        powerSaveEngaged = false;
        }
        */

        bluetooth_init();
        battery_init();
        update_settings();
      }
    }

  } //MINUTE_UNIT

  if(units_changed & HOUR_UNIT) {
    if(appStarted && global_settings.HourlyVibe && !powerSaveEngaged) {
      //vibe!
      vibes_short_pulse();
    }
    if (!clock_is_24h_style()) {
      if (tick_time->tm_hour >= 12) {
        text_layer_set_text(t_layer[t_ampm], "PM");
      }
      else {
        text_layer_set_text(t_layer[t_ampm], "AM");
      }
    }
  } //HOUR_UNIT



  if (units_changed & DAY_UNIT) {

    static char date_day[4];
    static char date_monthday[3];
    static char date_month[6];
    static char full_date_text[20];

    strftime(date_day, sizeof(date_day), "%a", tick_time);

    strftime(date_monthday,
      sizeof(date_monthday),
      "%d",
      tick_time);

      strftime(date_month,
             sizeof(date_month),
             "%b",
             tick_time);

    /* snprintf(full_date_text,
      sizeof(full_date_text),
      "%s %s %s",
      upcase(date_day),
      date_monthday,
      upcase(date_month)); */

    snprintf(full_date_text,
      sizeof(full_date_text),
      "%s %s",
      upcase(date_day),
      date_monthday);

    snprintf(fctx_date_text, sizeof(fctx_date_text), "%s", full_date_text);
    layer_mark_dirty(fctx_clock_layer);
  }

}

void timedigits_init() {

  //Center Panel
  center_layer = layer_create(TIMEDIGITS_CENTER);

  layer_add_child(my_window_layer, center_layer);

#if DEBUG_DRAW_LINES
  debug_lines_layer = layer_create(FULLSCREEN);
  layer_set_update_proc(debug_lines_layer, debug_lines_update_proc);
  layer_add_child(my_window_layer, debug_lines_layer);
#endif

  // FCTX clock layer (always active)
  fctx_clock_layer = layer_create(FULLSCREEN);
  layer_set_update_proc(fctx_clock_layer, fctx_update_proc);
  layer_add_child(my_window_layer, fctx_clock_layer);

  // Separator blink-state tracker (minimal hidden layer)
  t_layer[t_sep] = text_layer_create(GRect(0, 0, 1, 1));
  text_layer_set_background_color(t_layer[t_sep], GColorClear);
  layer_add_child(center_layer, text_layer_get_layer(t_layer[t_sep]));

  //Indicator - AM/PM/24H
  t_layer[t_ampm] = text_layer_create_detailed(TIMEDIGITS_AMPM, false,
                                  GColorClear, color_helper(colors[c_t2], global_settings.Invert),
                                  GTextAlignmentLeft, font_tiny);
  if (clock_is_24h_style()) {
    text_layer_set_text(t_layer[t_ampm], "24H");
  }
  layer_add_child(center_layer, text_layer_get_layer(t_layer[t_ampm]));

  calculate_dynamic_positions();
  cached_date_char_height = graphics_text_layout_get_content_size("MON 31", font_small, GRect(0, 0, 186, 40), GTextOverflowModeFill, GTextAlignmentRight).h;
  if (cached_date_char_height <= 0) cached_date_char_height = 18;

  //Somebody set us up the CLOCK
  time_t now = time(NULL);
  struct tm *tick_time = localtime(&now);

  //Tick, Tick, Tick, BOOM!
  handle_tick(tick_time, DAY_UNIT + HOUR_UNIT + MINUTE_UNIT + SECOND_UNIT);
  s_was_24h = clock_is_24h_style();
  if((global_settings.Blink || global_settings.Seconds) && !powerSaveEngaged ) {
    tick_timer_service_subscribe(SECOND_UNIT, handle_tick);
  }
  else {
    tick_timer_service_subscribe(MINUTE_UNIT, handle_tick);
  }

  settings_register_callback(timedigits_settings_callback, SETTINGS_CALLBACK_TIMEDIGITS);
  timedigits_settings_callback();
  layer_mark_dirty(fctx_clock_layer);
}

void timedigits_deinit() {

  tick_timer_service_unsubscribe();
  if (blink_timer) {
    app_timer_cancel(blink_timer);
    blink_timer = NULL;
  }

  layer_set_update_proc(fctx_clock_layer, NULL);
  layer_destroy(fctx_clock_layer);
  fctx_clock_layer = NULL;

  for (int i = 0; i < 2; i++) {
    if (t_layer[i] != NULL) {
      text_layer_destroy(t_layer[i]);
      t_layer[i] = NULL;
    }
  }

  layer_destroy(center_layer);
#if DEBUG_DRAW_LINES
  layer_destroy(debug_lines_layer);
#endif
}
