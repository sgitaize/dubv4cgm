#include <pebble.h>
#include "_globals.h"
#include "complications.h"
#include "decorations.h"
#include "settings.h"
#include "helpers.h"
#include "window.h"

// dubv4cgm: Nightscout CGM value in panel row 1 (instead of steps) and
// configurable edge labels (CGM, weather, steps, heart rate, battery).
// CGM data comes from pkjs (same fetch logic as casiocgm); the watch ages
// the reading locally, so the age/stale state stays right between fetches.

#define CGM_LAYER GRect(36, 47, 96, 28)
#define CGM_ARROW_W 12
#define VIBE_COOLDOWN_SEC (10 * 60)

static Layer *cgm_layer = NULL;

static char s_value[8] = "";
static char s_delta[8] = "";
static char s_trend = '-';
static int  s_status = -1;         // -1 = nothing received yet
static time_t s_ts = 0;
static int  s_sgv = 0;

static bool s_wx_valid = false;
static int  s_wx_temp = 0;
static int  s_wx_code = 0;

static int  s_steps = 0;
static int  s_hr = 0;

static time_t s_last_req = 0;
static time_t s_last_vibe_low = 0, s_last_vibe_high = 0;
static bool s_connected = true;

// ── CGM state ─────────────────────────────────────────────────────────────
static int cgm_age_min(void) {
  if (s_ts <= 0) return -1;
  time_t now = time(NULL);
  return now <= s_ts ? 0 : (int)((now - s_ts) / 60);
}

static bool cgm_has_value(void) {
  return (s_status == CGM_STATUS_OK || s_status == CGM_STATUS_OLD) && s_sgv > 0 && s_value[0];
}

// Stale like supercgm: older than 2x the sensor interval, at least 5 min
static bool cgm_is_stale(void) {
  int stale = global_settings.CgmStaleMin * 60;
  if (stale < 300) stale = 300;
  return s_status == CGM_STATUS_OLD || (time(NULL) - s_ts) > stale;
}

static const char *cgm_status_text(void) {
  switch (s_status) {
    case -1:                 return "CGM ...";
    case CGM_STATUS_NO_CONN: return "NO CONN";
    case 4:                  return "SET URL";   // pkjs: no Nightscout URL
    default:                 return "NO BG";
  }
}

static GColor cgm_color(GColor base) {
  if (!global_settings.CgmColorize || cgm_is_stale()) return base;
  if (s_sgv > global_settings.CgmHigh) return color_helper((GColor){.argb = global_settings.CgmColorHigh}, global_settings.Invert);
  if (s_sgv < global_settings.CgmLow)  return color_helper((GColor){.argb = global_settings.CgmColorLow}, global_settings.Invert);
  return base;
}

// Trend as a drawn arrow (same shapes as casiocgm). t: U u r - f d D
static void draw_trend_arrow(GContext *ctx, char t, GRect r, GColor col) {
  int cx = r.origin.x + r.size.w / 2;
  int cy = r.origin.y + r.size.h / 2;
  int ah = 6;
  graphics_context_set_stroke_color(ctx, col);
  graphics_context_set_stroke_width(ctx, 2);
  switch (t) {
    case 'U':
      graphics_draw_line(ctx, GPoint(cx, cy-ah-1), GPoint(cx-ah, cy-1));
      graphics_draw_line(ctx, GPoint(cx, cy-ah-1), GPoint(cx+ah, cy-1));
      graphics_draw_line(ctx, GPoint(cx, cy+1),    GPoint(cx-ah, cy+ah+1));
      graphics_draw_line(ctx, GPoint(cx, cy+1),    GPoint(cx+ah, cy+ah+1));
      break;
    case 'u':
      graphics_draw_line(ctx, GPoint(cx, cy+ah), GPoint(cx, cy-ah));
      graphics_draw_line(ctx, GPoint(cx, cy-ah), GPoint(cx-ah+1, cy-1));
      graphics_draw_line(ctx, GPoint(cx, cy-ah), GPoint(cx+ah-1, cy-1));
      break;
    case 'r':
      graphics_draw_line(ctx, GPoint(cx-ah+1, cy+ah-1), GPoint(cx+ah-1, cy-ah+1));
      graphics_draw_line(ctx, GPoint(cx+ah-1, cy-ah+1), GPoint(cx, cy-ah+1));
      graphics_draw_line(ctx, GPoint(cx+ah-1, cy-ah+1), GPoint(cx+ah-1, cy));
      break;
    case 'f':
      graphics_draw_line(ctx, GPoint(cx-ah+1, cy-ah+1), GPoint(cx+ah-1, cy+ah-1));
      graphics_draw_line(ctx, GPoint(cx+ah-1, cy+ah-1), GPoint(cx, cy+ah-1));
      graphics_draw_line(ctx, GPoint(cx+ah-1, cy+ah-1), GPoint(cx+ah-1, cy));
      break;
    case 'd':
      graphics_draw_line(ctx, GPoint(cx, cy-ah), GPoint(cx, cy+ah));
      graphics_draw_line(ctx, GPoint(cx, cy+ah), GPoint(cx-ah+1, cy+1));
      graphics_draw_line(ctx, GPoint(cx, cy+ah), GPoint(cx+ah-1, cy+1));
      break;
    case 'D':
      graphics_draw_line(ctx, GPoint(cx, cy-1),    GPoint(cx-ah, cy-ah-1));
      graphics_draw_line(ctx, GPoint(cx, cy-1),    GPoint(cx+ah, cy-ah-1));
      graphics_draw_line(ctx, GPoint(cx, cy+ah+1), GPoint(cx-ah, cy+1));
      graphics_draw_line(ctx, GPoint(cx, cy+ah+1), GPoint(cx+ah, cy+1));
      break;
    default:
      graphics_draw_line(ctx, GPoint(cx-ah, cy), GPoint(cx+ah, cy));
      graphics_draw_line(ctx, GPoint(cx+ah, cy), GPoint(cx+1, cy-ah+1));
      graphics_draw_line(ctx, GPoint(cx+ah, cy), GPoint(cx+1, cy+ah-1));
      break;
  }
  graphics_context_set_stroke_width(ctx, 1);
}

// Panel row 1: "123 ↗ +3" (fresh), "123" struck through (stale), status text
static void cgm_layer_update(Layer *layer, GContext *ctx) {
  GRect b = layer_get_bounds(layer);
  GColor base = color_helper(colors[c_h1], global_settings.Invert);
  GFont f_val = fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);
  GFont f_small = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);

  if (!cgm_has_value()) {
    graphics_context_set_text_color(ctx, base);
    graphics_draw_text(ctx, cgm_status_text(), f_small, GRect(0, 5, b.size.w, 22),
                       GTextOverflowModeFill, GTextAlignmentLeft, NULL);
    return;
  }

  bool stale = cgm_is_stale();
  GColor col = cgm_color(base);
  GSize vs = graphics_text_layout_get_content_size(s_value, f_val, GRect(0, 0, b.size.w, 28),
                                                   GTextOverflowModeFill, GTextAlignmentLeft);
  graphics_context_set_text_color(ctx, col);
  graphics_draw_text(ctx, s_value, f_val, GRect(0, 0, vs.w + 2, 28),
                     GTextOverflowModeFill, GTextAlignmentLeft, NULL);
  int x = vs.w + 2;

  if (stale) {
    graphics_context_set_stroke_color(ctx, col);
    graphics_context_set_stroke_width(ctx, 2);
    graphics_draw_line(ctx, GPoint(-1, 17), GPoint(vs.w + 1, 17));
    graphics_context_set_stroke_width(ctx, 1);
    return;
  }

  draw_trend_arrow(ctx, s_trend, GRect(x, 7, CGM_ARROW_W, 14), col);
  x += CGM_ARROW_W + 2;
  if (s_delta[0] && x < b.size.w) {
    graphics_context_set_text_color(ctx, base);
    // small font: "+0.7" must not reach the "100%" battery text
    graphics_draw_text(ctx, s_delta, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD), GRect(x, 7, b.size.w - x, 18),
                       GTextOverflowModeFill, GTextAlignmentLeft, NULL);
  }
}

// ── Edge labels ───────────────────────────────────────────────────────────
static const char *weather_text(int code) {
  if (code == 0)  return "CLEAR";
  if (code <= 2)  return "FAIR";
  if (code == 3)  return "CLOUDY";
  if (code <= 48) return "FOG";
  if (code <= 57) return "DRIZZLE";
  if (code <= 67) return "RAIN";
  if (code <= 77) return "SNOW";
  if (code <= 82) return "SHOWERS";
  if (code <= 86) return "SNOW";
  return "STORM";
}

static void slot_text(uint8_t type, uint8_t idx, char *buf, size_t len) {
  static const char *defaults[DEC_LABEL_COUNT] = {"LIGHT", "PREV", "NEXT", ""};
  buf[0] = '\0';
  switch (type) {
    case SLOT_LABEL:
      snprintf(buf, len, "%s", defaults[idx]);
      break;
    case SLOT_CGM:
      if (!cgm_has_value())   snprintf(buf, len, "%s", cgm_status_text());
      else if (cgm_is_stale()) snprintf(buf, len, "%s OLD", s_value);
      else                     snprintf(buf, len, "%s %s", s_value, s_delta);
      break;
    case SLOT_CGM_AGE: {
      int age = cgm_age_min();
      if (age < 0) snprintf(buf, len, "CGM --");
      else         snprintf(buf, len, "%d MIN AGO", age);
      break;
    }
    case SLOT_WEATHER:
      if (s_wx_valid) snprintf(buf, len, "%d° %s", s_wx_temp, weather_text(s_wx_code));
      else            snprintf(buf, len, "--°");
      break;
    case SLOT_STEPS: {
      char n[12];
      format_commas(s_steps, n);
      snprintf(buf, len, "%s STEPS", n);
      break;
    }
    case SLOT_HR:
      if (s_hr > 0) snprintf(buf, len, "%d BPM", s_hr);
      else          snprintf(buf, len, "-- BPM");
      break;
    case SLOT_BATTERY:
      snprintf(buf, len, "BAT %d%%", battery_state_service_peek().charge_percent);
      break;
    default:
      break;
  }
}

static void update_labels(void) {
  uint8_t types[DEC_LABEL_COUNT] = {global_settings.SlotTL, global_settings.SlotTR,
                                    global_settings.SlotBR, global_settings.SlotBL};
  char buf[24];
  for (uint8_t i = 0; i < DEC_LABEL_COUNT; i++) {
    slot_text(types[i], i, buf, sizeof(buf));
    decorations_set_label(i, buf);
  }
}

bool complications_backlight_color(GColor *out) {
  if (!global_settings.CgmBacklight || !cgm_has_value() || cgm_is_stale()) return false;
  if (s_sgv > global_settings.CgmHigh) { out->argb = global_settings.CgmColorHigh; return true; }
  if (s_sgv < global_settings.CgmLow)  { out->argb = global_settings.CgmColorLow;  return true; }
  return false;
}

static void refresh(void) {
  static int8_t s_lit = -1;   // re-apply the backlight colour only on changes
  GColor c;
  int8_t lit = complications_backlight_color(&c) ? (int8_t)c.argb : 0;
  if (lit != s_lit) { s_lit = lit; apply_light_color(); }
  update_labels();
  if (cgm_layer) layer_mark_dirty(cgm_layer);
}

static void read_health(void) {
#if defined(PBL_HEALTH)
  time_t now = time(NULL);
  if (health_service_metric_accessible(HealthMetricStepCount, time_start_of_today(), now) & HealthServiceAccessibilityMaskAvailable)
    s_steps = (int)health_service_sum_today(HealthMetricStepCount);
  if (health_service_metric_accessible(HealthMetricHeartRateBPM, now, now) & HealthServiceAccessibilityMaskAvailable)
    s_hr = (int)health_service_peek_current_value(HealthMetricHeartRateBPM);
#endif
}

// ── Phone communication ───────────────────────────────────────────────────
static void request_bg(void) {
  DictionaryIterator *iter;
  if (app_message_outbox_begin(&iter) != APP_MSG_OK) return;
  dict_write_uint8(iter, MESSAGE_KEY_RequestBg, 1);
  app_message_outbox_send();
}

static void check_alerts(void) {
  if (!cgm_has_value() || cgm_is_stale()) return;
  time_t now = time(NULL);
  if (global_settings.VibeLow && s_sgv < global_settings.CgmLow) {
    if (now - s_last_vibe_low >= VIBE_COOLDOWN_SEC) {
      s_last_vibe_low = now;
      static const uint32_t segs[] = {200, 100, 200, 100, 200};
      vibes_enqueue_custom_pattern((VibePattern){ .durations = segs, .num_segments = ARRAY_LENGTH(segs) });
    }
  } else if (global_settings.VibeHigh && s_sgv > global_settings.CgmHigh) {
    if (now - s_last_vibe_high >= VIBE_COOLDOWN_SEC) {
      s_last_vibe_high = now;
      static const uint32_t segs[] = {200, 100, 200};
      vibes_enqueue_custom_pattern((VibePattern){ .durations = segs, .num_segments = ARRAY_LENGTH(segs) });
    }
  }
}

bool complications_inbox(DictionaryIterator *iter) {
  Tuple *t;
  bool data = false;
  if ((t = dict_find(iter, MESSAGE_KEY_CgmValue))) snprintf(s_value, sizeof(s_value), "%s", t->value->cstring);
  if ((t = dict_find(iter, MESSAGE_KEY_CgmDelta))) snprintf(s_delta, sizeof(s_delta), "%s", t->value->cstring);
  if ((t = dict_find(iter, MESSAGE_KEY_CgmTrend))) s_trend = t->value->cstring[0];
  if ((t = dict_find(iter, MESSAGE_KEY_CgmSgv)))   s_sgv = (int)t->value->int32;
  if ((t = dict_find(iter, MESSAGE_KEY_CgmTs)))    s_ts = (time_t)t->value->int32;
  if ((t = dict_find(iter, MESSAGE_KEY_CgmStatus))) {
    s_status = (int)t->value->int32;
    // OLD keeps the last value (shown struck through), errors drop it
    if (s_status != CGM_STATUS_OK && s_status != CGM_STATUS_OLD) s_sgv = 0;
    check_alerts();
    data = true;
  }
  if ((t = dict_find(iter, MESSAGE_KEY_WeatherTemp))) {
    s_wx_temp = (int)t->value->int32;
    s_wx_valid = true;
    data = true;
  }
  if ((t = dict_find(iter, MESSAGE_KEY_WeatherCode))) s_wx_code = (int)t->value->int32;
  if (data) refresh();
  return data && !dict_find(iter, MESSAGE_KEY_SlotMain);
}

// Watchdog (as casiocgm): the phone's fetch chain is a single JS timer that
// can die when the phone suspends PebbleKit JS. Reading older than the
// interval + 3 min → ask the phone (wakes the JS), at most every 3 min.
static void bg_watchdog(void) {
  time_t now = time(NULL);
  int interval = global_settings.CgmStaleMin * 30;  // stale = 2x interval
  if (interval < 60) interval = 60;
  if (s_status == 4) return;                          // no URL configured
  if (now - s_ts < interval + 180 || now - s_last_req < 180) return;
  if (!connection_service_peek_pebble_app_connection()) return;
  s_last_req = now;
  request_bg();
}

void complications_minute_tick(void) {
  // BT reconnect: fetch right away instead of waiting for the next timer
  bool c = connection_service_peek_pebble_app_connection();
  if (c && !s_connected) request_bg();
  s_connected = c;
  bg_watchdog();
  read_health();
  refresh();
}

static void complications_settings_callback(void) {
  if (cgm_layer) layer_set_hidden(cgm_layer, global_settings.SlotMain != 0);
  refresh();
}

void complications_init(void) {
  cgm_layer = layer_create(CGM_LAYER);
  layer_set_update_proc(cgm_layer, cgm_layer_update);
  layer_add_child(my_window_layer, cgm_layer);
  layer_set_hidden(cgm_layer, global_settings.SlotMain != 0);
  s_connected = connection_service_peek_pebble_app_connection();
  read_health();
  update_labels();
  settings_register_callback(complications_settings_callback, SETTINGS_CALLBACK_COMPLICATIONS);
}

void complications_deinit(void) {
  settings_unregister_callback(SETTINGS_CALLBACK_COMPLICATIONS);
  if (cgm_layer) {
    layer_destroy(cgm_layer);
    cgm_layer = NULL;
  }
}
