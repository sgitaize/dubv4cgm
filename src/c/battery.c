#include <pebble.h>
#include "_globals.h"
#include "battery.h"
#include "settings.h"
#include "helpers.h"
#include "window.h"
#include "fonts.h"


static Layer *battery_layer, *bolt_layer;
static TextLayer *battery_percent_layer;

static bool batteryCharging = false;
static uint8_t batteryPercent;

static GPath *bolt_path_ptr = NULL;

#if defined(PBL_PLATFORM_EMERY)
static GPathInfo BOLT_PATH_INFO = {
  .num_points = 13,
  .points = (GPoint []) {{5,5},{8,5},{8,4},{11,4},{11,3},{11,5},{16,5},{14,5},{14,7},{11,7},{11,8},{11,5},{15,5}}
};
#else
static GPathInfo BOLT_PATH_INFO = {
  .num_points = 13,
  .points = (GPoint []) {{4,4},{6,4},{6,3},{8,3},{8,2},{8,4},{12,4},{10,4},{10,5},{8,5},{8,6},{8,4},{11,4}}
};
#endif

/*
bi1 Battery Indicator
bi2 Battery Indicator Warning
bi3 Battery Indicator Critical
bi4 Battery Charging
*/

void battery_settings_callback() {
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "battery_settings_callback()");
  text_layer_set_text_color(battery_percent_layer, color_helper(colors[c_bi1], global_settings.Invert));
  layer_mark_dirty(text_layer_get_layer(battery_percent_layer));
  layer_mark_dirty(battery_layer);
}

void battery_update(BatteryChargeState charge_state) {
  batteryPercent = charge_state.charge_percent;
  batteryCharging = charge_state.is_charging;
  if(batteryPercent==0) {
    batteryPercent=1;
  }
  /*if(batteryPercent==100) {
    layer_set_hidden(text_layer_get_layer(battery_percent_layer),true);
  }
  else {*/
    layer_set_hidden(text_layer_get_layer(battery_percent_layer),false);
    static char txt[5];
    snprintf(txt, sizeof(txt), "%u%%", batteryPercent);
    text_layer_set_text(battery_percent_layer, txt);
  //}

}

void battery_layer_update_callback(Layer *my_layer, GContext* ctx) {
  GColor color = color_helper(colors[c_bi1], global_settings.Invert);
  if(batteryPercent<20) {
    color = color_helper(colors[c_bi3], global_settings.Invert);
  }
  else if(batteryPercent<30) {
    color = color_helper(colors[c_bi2], global_settings.Invert);
  }

  text_layer_set_text_color(battery_percent_layer, color);

  graphics_context_set_stroke_color(ctx, color);
  graphics_draw_rect(ctx, BATTERY_ICON);
  graphics_draw_rect(ctx, BATTERY_ICON_TERMINAL);

  graphics_context_set_stroke_color(ctx, color);
  graphics_context_set_fill_color(ctx, color);

  if(batteryCharging) {
    graphics_context_set_stroke_color(ctx, color_helper(colors[c_bi4], global_settings.Invert));
    gpath_draw_outline(ctx, bolt_path_ptr);
  }
  else {
    uint8_t width = (uint8_t)((batteryPercent/100.0)*(float)BATTERY_FILL_MAX_W);
    if(width < BATTERY_FILL_CEIL) {
      width++;
    }
    graphics_fill_rect(ctx, GRect(BATTERY_FILL_OFFSET_X, BATTERY_FILL_OFFSET_Y, width, BATTERY_FILL_HEIGHT), 0, GCornerNone);
  }
}


void battery_init() {

  //APP_LOG(APP_LOG_LEVEL_DEBUG, "battery_init()");

  battery_percent_layer = text_layer_create_detailed(BATTERY_PERCENT, false,
                                GColorClear, color_helper(colors[c_bi1], global_settings.Invert),
                                PBL_IF_RECT_ELSE(GTextAlignmentRight, GTextAlignmentLeft), font_tiny);
  layer_add_child(my_window_layer, text_layer_get_layer(battery_percent_layer));

  bolt_path_ptr = gpath_create(&BOLT_PATH_INFO);
  battery_layer = layer_create(BATTERY_LAYER);
  //layer_set_hidden(battery_layer, true);
  layer_set_update_proc(battery_layer, battery_layer_update_callback);
  #if defined(PBL_RECT)
    layer_add_child(my_window_layer, battery_layer);
  #endif

  battery_update(battery_state_service_peek());
  battery_state_service_subscribe(&battery_update);

  settings_register_callback(battery_settings_callback, SETTINGS_CALLBACK_BATTERY);

  //animation_slide_in(battery_layer, 1000, DOWN);
  //animation_slide_in(text_layer_get_layer(battery_percent_layer), 1100, DOWN);
}

void battery_deinit() {

  //APP_LOG(APP_LOG_LEVEL_DEBUG, "battery_deinit()");

  settings_unregister_callback(SETTINGS_CALLBACK_BATTERY);
  battery_state_service_unsubscribe();
  gpath_destroy(bolt_path_ptr);
  bolt_path_ptr = NULL;

  text_layer_destroy(battery_percent_layer);

  layer_remove_from_parent(bolt_layer);
  layer_remove_from_parent(battery_layer);

  layer_destroy(bolt_layer);
  layer_destroy(battery_layer);
}
