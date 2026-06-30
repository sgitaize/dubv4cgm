#include <pebble.h>
#include "decorations.h"
#include "_globals.h"
#include "helpers.h"
#include "settings.h"
#include "fonts.h"
#include "window.h"

static Layer *decorations_layer, *wr_outer_layer, *button_back_icon_layer, *button_next_icon_layer, *button_prev_icon_layer;
static TextLayer *water_layer, *resist_layer, *button_back_layer, *button_next_layer, *button_prev_layer;
static BitmapLayer *logo_layer;
static GBitmap *logo_image;
static int8_t current_logo = -1;

static const int logo_resource_ids[LOGOS_COUNT] = {
  RESOURCE_ID_IMAGE_LOGO_PEBBLE_OLD,
  RESOURCE_ID_IMAGE_LOGO_PEBBLE_NEW
};

static GPath *arrow_left_path_ptr = NULL;
static GPathInfo ARROW_LEFT_PATH_INFO = {
  .num_points = 5,
  .points = (GPoint []) {{0,3}, {5,0}, {5,7}, {0,4}, {0,3}}
};

static GPath *arrow_right_path_ptr = NULL;
static GPathInfo ARROW_RIGHT_PATH_INFO = {
  .num_points = 5,
  .points = (GPoint []) {{0,0}, {5,3}, {5,4}, {0,7}, {0,0}}
};

static GPath *wr_outer_path_ptr = NULL;
static GPathInfo WR_OUTER_PATH_INFO = {
  .num_points = 9,
  .points = (GPoint []) {{0,3}, {3,0}, {53,0}, {55,3}, {55,13}, {47,21}, {7,21}, {0,15}, {0,3}}
};

static GPath *wr_w_path_ptr = NULL;
static GPathInfo WR_W_PATH_INFO = {
  .num_points = 39,
  .points = (GPoint []) {{ 9, 3},{14, 3},{14,12},{16,12},{16, 8},{18, 8},
                          {18, 5},{19, 5},{19, 3},{22, 3},{22,12},{24,12},
                          {24, 9},{26, 9},{26, 5},{27, 5},{27, 3},{30, 3},
                          {30, 4},{28, 4},{28, 8},{27, 8},{27,11},{26,11},
                          {26,15},{24,15},{24,18},{20,18},{20, 8},
                          {18, 8},{18,12},{16,12},{16,15},{15,15},{15,18},
                          {11,18},{11,11},{ 9,11},{ 9, 3}}
};

static GPath *wr_r_path_ptr = NULL;
static GPathInfo WR_R_PATH_INFO = {
  .num_points = 27,
  .points = (GPoint []) {{32, 3},{42, 3},{42, 4},{43, 4},{43, 5},{45, 5},
                          {45, 9},{43, 9},{43,11},{42,11},{42,14},{43,14},
                          {43,18},{39,18},{39,14},{38,14},{38,12},{35,12},
                          {35,14},{34,14},{34,18},{30,18},
                          {30,15},{31,15},{31, 7},{32, 7},{32, 3}}
};

/*
d1  Horizontal Line Top
d2  Horizontal Line Bottom
d3  Water Resist Box Stroke
d4  Water Resist Box Fill
d5  WR Letters
d6  Water Resist Text
d7  Button Labels
d8  Button Icons
d9  Branding Text
*/

static void decorations_load_logo(int8_t index) {
  if (index < 0 || index >= LOGOS_COUNT) return;
  if (index == current_logo) return;
  current_logo = index;

  if (logo_image) {
    gbitmap_destroy(logo_image);
    logo_image = NULL;
  }

  logo_image = gbitmap_create_with_resource(logo_resource_ids[index]);
  GColor * xcolors = gbitmap_get_palette(logo_image);
  xcolors[0].argb = color_helper(colors[c_bg4], global_settings.Invert).argb;
  xcolors[1].argb = color_helper(colors[c_d9], global_settings.Invert).argb;
  bitmap_layer_set_bitmap(logo_layer, logo_image);
}

void decorations_settings_callback() {

  //APP_LOG(APP_LOG_LEVEL_DEBUG, "decorations_settings_callback()");

  text_layer_set_text_color(water_layer, color_helper(colors[c_d6], global_settings.Invert));
  text_layer_set_text_color(resist_layer, color_helper(colors[c_d6], global_settings.Invert));

  text_layer_set_text_color(button_back_layer, color_helper(colors[c_d7], global_settings.Invert));
  text_layer_set_text_color(button_next_layer, color_helper(colors[c_d7], global_settings.Invert));
  text_layer_set_text_color(button_prev_layer, color_helper(colors[c_d7], global_settings.Invert));

  GColor * xcolors = gbitmap_get_palette(logo_image);
  xcolors[0].argb = color_helper(colors[c_bg4], global_settings.Invert).argb;
  xcolors[1].argb = color_helper(colors[c_d9], global_settings.Invert).argb;

  decorations_load_logo(global_settings.Logo);

  if(global_settings.BrandingMask) {
    layer_set_hidden(bitmap_layer_get_layer(logo_layer), true);
  }
  else {
    layer_set_hidden(bitmap_layer_get_layer(logo_layer), false);
  }

  layer_mark_dirty(decorations_layer);
}

void back_icon_layer_update_callback(Layer *my_layer, GContext* ctx) {
  //Arrow BACK
  graphics_context_set_stroke_color(ctx, color_helper(colors[c_d8], global_settings.Invert));
  graphics_context_set_fill_color(ctx, color_helper(colors[c_d8], global_settings.Invert));
  gpath_draw_filled(ctx, arrow_left_path_ptr);
  gpath_draw_outline(ctx, arrow_left_path_ptr);
}


void next_icon_layer_update_callback(Layer *my_layer, GContext* ctx) {
  //Arrow NEXT
  graphics_context_set_stroke_color(ctx, color_helper(colors[c_d8], global_settings.Invert));
  graphics_context_set_fill_color(ctx, color_helper(colors[c_d8], global_settings.Invert));
  gpath_draw_filled(ctx, arrow_right_path_ptr);
  gpath_draw_outline(ctx, arrow_right_path_ptr);
}


void prev_icon_layer_update_callback(Layer *my_layer, GContext* ctx) {
  //Arrow PREV
  graphics_context_set_stroke_color(ctx, color_helper(colors[c_d8], global_settings.Invert));
  graphics_context_set_fill_color(ctx, color_helper(colors[c_d8], global_settings.Invert));
  gpath_draw_filled(ctx, arrow_right_path_ptr);
  gpath_draw_outline(ctx, arrow_right_path_ptr);
}

void decorations_layer_update_callback(Layer *my_layer, GContext* ctx) {
  //Line Top
  graphics_context_set_stroke_color(ctx, color_helper(colors[c_d1], global_settings.Invert));
  graphics_draw_line(ctx, DECORATIONS_LINE_TOP_START, DECORATIONS_LINE_TOP_END);

  //Line Bottom
  graphics_context_set_stroke_color(ctx, color_helper(colors[c_d2], global_settings.Invert));
  graphics_draw_line(ctx, DECORATIONS_LINE_BOTTOM_START, DECORATIONS_LINE_BOTTOM_END);
}

void decorations_wr_outer_layer_update_callback(Layer *my_layer, GContext* ctx) {

  //Box surrounding WR
  graphics_context_set_stroke_color(ctx, color_helper(colors[c_d3], global_settings.Invert));
  graphics_context_set_fill_color(ctx, color_helper(colors[c_d4], global_settings.Invert));
  gpath_draw_filled(ctx, wr_outer_path_ptr);
  gpath_draw_outline(ctx, wr_outer_path_ptr);

  //W and R
  graphics_context_set_stroke_color(ctx, color_helper(colors[c_d5], global_settings.Invert));
  graphics_context_set_fill_color(ctx, color_helper(colors[c_d5], global_settings.Invert));
  gpath_draw_filled(ctx, wr_w_path_ptr);
  gpath_draw_outline(ctx, wr_w_path_ptr);
  gpath_draw_filled(ctx, wr_r_path_ptr);
  gpath_draw_outline(ctx, wr_r_path_ptr);

  //Hole in the R
  graphics_context_set_fill_color(ctx, color_helper(colors[c_d4], global_settings.Invert));
  graphics_fill_rect(ctx, DECORATIONS_WR_HOLE, 0, GCornerNone);

}

void decorations_init() {

  //APP_LOG(APP_LOG_LEVEL_DEBUG, "decorations_init()");

  decorations_layer = layer_create(FULLSCREEN);
  //layer_set_hidden(decorations_layer, true);
  layer_add_child(my_window_layer, decorations_layer);
  layer_set_update_proc(decorations_layer, decorations_layer_update_callback);


  button_back_icon_layer = layer_create(DECORATIONS_BUTTON_BACK_ICON);
  layer_set_update_proc(button_back_icon_layer, back_icon_layer_update_callback);
  layer_add_child(decorations_layer, button_back_icon_layer);

  button_next_icon_layer = layer_create(DECORATIONS_BUTTON_NEXT_ICON);
  layer_set_update_proc(button_next_icon_layer, next_icon_layer_update_callback);
  layer_add_child(decorations_layer, button_next_icon_layer);

  button_prev_icon_layer = layer_create(DECORATIONS_BUTTON_PREV_ICON);
  layer_set_update_proc(button_prev_icon_layer, prev_icon_layer_update_callback);
  layer_add_child(decorations_layer, button_prev_icon_layer);

  arrow_left_path_ptr = gpath_create(&ARROW_LEFT_PATH_INFO);
  arrow_right_path_ptr = gpath_create(&ARROW_RIGHT_PATH_INFO);

  wr_outer_path_ptr = gpath_create(&WR_OUTER_PATH_INFO);
  wr_w_path_ptr = gpath_create(&WR_W_PATH_INFO);
  wr_r_path_ptr = gpath_create(&WR_R_PATH_INFO);

  wr_outer_layer = layer_create(DECORATIONS_WR_OUTER);
  //layer_set_hidden(wr_outer_layer, true);
  layer_set_update_proc(wr_outer_layer, decorations_wr_outer_layer_update_callback);
  layer_add_child(decorations_layer, wr_outer_layer);

  // WATER LABEL
  water_layer = text_layer_create(DECORATIONS_WR_WATER);
  text_layer_set_text_alignment(water_layer, GTextAlignmentRight);
  text_layer_set_text(water_layer, "WATER");
  text_layer_set_background_color(water_layer, GColorClear);
  text_layer_set_text_color(water_layer, color_helper(colors[c_d6], global_settings.Invert));
  text_layer_set_font(water_layer, font_tiny);
  layer_add_child(decorations_layer, text_layer_get_layer(water_layer));

  // RESIST LABEL
  resist_layer = text_layer_create(DECORATIONS_WR_RESIST);
  text_layer_set_text_alignment(resist_layer, GTextAlignmentLeft);
  text_layer_set_text(resist_layer, "RESIST");
  text_layer_set_background_color(resist_layer, GColorClear);
  text_layer_set_text_color(resist_layer, color_helper(colors[c_d6], global_settings.Invert));
  text_layer_set_font(resist_layer, font_tiny);
  layer_add_child(decorations_layer, text_layer_get_layer(resist_layer));

  // BACK BUTTON LABEL
  button_back_layer = text_layer_create_detailed(DECORATIONS_BUTTON_BACK_LABEL, false, GColorClear
                                                 , color_helper(colors[c_d7], global_settings.Invert),
                                                 GTextAlignmentLeft, font_tiny);
  text_layer_set_text(button_back_layer, "LIGHT");
  layer_add_child(decorations_layer, text_layer_get_layer(button_back_layer));


  // NEXT BUTTON LABEL
  button_next_layer = text_layer_create_detailed(DECORATIONS_BUTTON_NEXT_LABEL, false, GColorClear
                                                 , color_helper(colors[c_d7], global_settings.Invert),
                                                 GTextAlignmentRight, font_tiny);
  text_layer_set_text(button_next_layer, "NEXT");
  layer_add_child(decorations_layer, text_layer_get_layer(button_next_layer));


  // PREV BUTTON LABEL
  button_prev_layer = text_layer_create_detailed(DECORATIONS_BUTTON_PREV_LABEL, false, GColorClear,
                                                  color_helper(colors[c_d7], global_settings.Invert),
                                                  GTextAlignmentRight, font_tiny);
  text_layer_set_text(button_prev_layer, "PREV");
  layer_add_child(decorations_layer, text_layer_get_layer(button_prev_layer));

  // BRANDING LABEL
  logo_layer = bitmap_layer_create(DECORATIONS_LOGO);
  decorations_load_logo(global_settings.Logo);
  layer_add_child(decorations_layer, bitmap_layer_get_layer(logo_layer));

  settings_register_callback(decorations_settings_callback, SETTINGS_CALLBACK_DECORATIONS);


  //animation_slide_in(decorations_layer, 700, RIGHT);
  //animation_slide_in(wr_outer_layer, 700, UP);
}

void decorations_deinit() {

  //APP_LOG(APP_LOG_LEVEL_DEBUG, "decorations_init()");

  gpath_destroy(wr_outer_path_ptr);
  gpath_destroy(wr_w_path_ptr);
  gpath_destroy(wr_r_path_ptr);
  gpath_destroy(arrow_left_path_ptr);
  gpath_destroy(arrow_right_path_ptr);

  wr_outer_path_ptr = NULL;
  wr_w_path_ptr = NULL;
  wr_r_path_ptr = NULL;
  arrow_left_path_ptr = NULL;
  arrow_right_path_ptr = NULL;

  layer_remove_from_parent(bitmap_layer_get_layer(logo_layer));
  gbitmap_destroy(logo_image);
  logo_image = NULL;
  current_logo = -1;
  bitmap_layer_destroy(logo_layer);

  layer_remove_from_parent(text_layer_get_layer(water_layer));
  layer_remove_from_parent(text_layer_get_layer(resist_layer));
  layer_remove_from_parent(text_layer_get_layer(button_back_layer));
  layer_remove_from_parent(text_layer_get_layer(button_next_layer));
  layer_remove_from_parent(text_layer_get_layer(button_prev_layer));

  layer_remove_from_parent(button_back_icon_layer);
  layer_remove_from_parent(button_next_icon_layer);
  layer_remove_from_parent(button_prev_icon_layer);
  layer_remove_from_parent(wr_outer_layer);
  layer_remove_from_parent(decorations_layer);

  text_layer_destroy(water_layer);
  text_layer_destroy(resist_layer);
  text_layer_destroy(button_back_layer);
  text_layer_destroy(button_next_layer);
  text_layer_destroy(button_prev_layer);

  layer_destroy(button_back_icon_layer);
  layer_destroy(button_next_icon_layer);
  layer_destroy(button_prev_icon_layer);
  layer_destroy(wr_outer_layer);
  layer_destroy(decorations_layer);
}

void decorations_toggle(bool is_obstructed) {
  if(is_obstructed) {
    layer_set_hidden(decorations_layer, true);
  } else {
    layer_set_hidden(decorations_layer, false);
  }
}
