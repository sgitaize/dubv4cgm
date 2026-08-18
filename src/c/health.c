#include <pebble.h>
#include "_globals.h"
#include "health.h"
#include "settings.h"
#include "helpers.h"
#include "window.h"
 #include "fonts.h"

static bool health_enabled = false;

#ifdef PBL_HEALTH

static TextLayer *health_text_layer;
static TextLayer *hr_text_layer = NULL;
static Layer *health_layer, *health_foot_layer, *health_foot2_layer, *health_zee_layer;
static Layer *hr_heart_layer = NULL;
static char hr_text_buf[8] = "";
static HealthValue s_hr = 0;

static GPath *foot_path_ptr = NULL;
static GPathInfo FOOT_PATH_INFO = {
  .num_points = 13,
  .points = (GPoint []) {{0,1}, {1,1}, {1,0}, {3,0}, {3,1}, {4,1}, {4,4}, {3,4}, {3,7}, {1,7}, {1,5}, {0,5}, {0,1}}
};
static GPath *heel_path_ptr = NULL;
static GPathInfo HEEL_PATH_INFO = {
  .num_points = 4,
  .points = (GPoint []) {{1,9}, {3,9}, {3,11}, {1,11}}
};
static GPath *zee1_path_ptr = NULL;
static GPathInfo ZEE1_PATH_INFO = {
  .num_points = 6,
  .points = (GPoint []) {{0,3}, {3,3}, {3,4}, {0,7}, {0,8}, {3,8}}
};
static GPath *zee2_path_ptr = NULL;
static GPathInfo ZEE2_PATH_INFO = {
  .num_points = 6,
  .points = (GPoint []) {{4,1}, {8,1}, {8,3}, {4,7}, {4,8}, {8,8}}
};
static GPath *zee3_path_ptr = NULL;
static GPathInfo ZEE3_PATH_INFO = {
  .num_points = 6,
  .points = (GPoint []) {{9,0}, {15,0}, {15,1}, {9,7}, {9,8}, {15,8}}
};



static HealthValue s_sleep, s_deep_sleep, s_steps, s_active, s_distance;

void health_icon_layer_update_callback(Layer *my_layer, GContext* ctx) {
  graphics_context_set_stroke_color(ctx, color_helper(colors[c_h1], global_settings.Invert));
  graphics_context_set_fill_color(ctx, color_helper(colors[c_h1], global_settings.Invert));
  if(s_steps < HEALTH_STEP_MIN) {
    //zzz
    gpath_draw_outline_open(ctx, zee1_path_ptr);
    gpath_draw_outline_open(ctx, zee2_path_ptr);
    gpath_draw_outline_open(ctx, zee3_path_ptr);
  } else {
    //steps
    gpath_draw_filled(ctx, foot_path_ptr);
    gpath_draw_outline(ctx, foot_path_ptr);
    gpath_draw_filled(ctx, heel_path_ptr);
    gpath_draw_outline(ctx, heel_path_ptr);
  }

}

void health_settings_callback() {
  health_deinit();

  #if defined (PBL_HEALTH)
    health_init();
  #endif
}

void health_update() {
  static char str[20], str2[20];
  if(s_steps < HEALTH_STEP_MIN) {
    //zzz
    int hours = 0, minutes = 0;
    duration_to_time(s_sleep, &hours, &minutes);

    if(hours>0) {
      snprintf(str2, sizeof(str2), "%dH'%dM", hours, minutes);
    } else {
      snprintf(str2, sizeof(str2), "%dM", minutes);
    }

    layer_set_hidden(health_zee_layer, false);
    layer_set_hidden(health_foot_layer, true);
    layer_set_hidden(health_foot2_layer, true);
  } else {
    //steps
    format_commas(s_steps, str);
    snprintf(str2, sizeof(str2), "%s", str);

    layer_set_hidden(health_zee_layer, true);
    layer_set_hidden(health_foot_layer, false);
    layer_set_hidden(health_foot2_layer, false);
  }
  text_layer_set_text(health_text_layer, str2);
  text_layer_set_text_color(health_text_layer, color_helper(colors[c_h1], global_settings.Invert));

  if (s_hr > 0) {
    snprintf(hr_text_buf, sizeof(hr_text_buf), "%d", (int)s_hr);
    if (hr_text_layer) {
      text_layer_set_text(hr_text_layer, hr_text_buf);
      text_layer_set_text_color(hr_text_layer, color_helper(colors[c_h2], global_settings.Invert));
    }
    if (hr_heart_layer) layer_set_hidden(hr_heart_layer, false);
  } else {
    if (hr_text_layer) text_layer_set_text(hr_text_layer, "");
    if (hr_heart_layer) layer_set_hidden(hr_heart_layer, true);
  }
}

static void hr_heart_update_proc(Layer *layer, GContext *ctx) {
  (void)layer;
  if (s_hr <= 0) return;
  GColor hr_col = color_helper(colors[c_h2], global_settings.Invert);
  graphics_context_set_fill_color(ctx, hr_col);
  graphics_context_set_stroke_color(ctx, hr_col);
  graphics_fill_circle(ctx, GPoint(4, 4), 3);
  graphics_fill_circle(ctx, GPoint(9, 4), 3);
  GPoint pts[3] = {{1, 5}, {12, 5}, {6, 11}};
  GPathInfo path_info = {.num_points = 3, .points = pts};
  GPath *p = gpath_create(&path_info);
  gpath_draw_filled(ctx, p);
  gpath_destroy(p);
}

void health_handler(HealthEventType event, void *context) {
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "health_handler");
  if (event != HealthEventSleepUpdate) {
    s_steps = health_service_sum_today(HealthMetricStepCount);
		//s_distance = health_service_sum_today(HealthMetricStepCount);
		//s_active = health_service_sum_today(HealthMetricActiveSeconds);
  }
  if (event != HealthEventMovementUpdate ) {
    s_sleep = health_service_sum_today(HealthMetricSleepSeconds);
    //s_deep_sleep = health_service_sum_today(HealthMetricSleepRestfulSeconds);
  }
  #ifdef PBL_PLATFORM_EMERY
  {
    time_t now = time(NULL);
    HealthServiceAccessibilityMask hr_mask =
        health_service_metric_accessible(HealthMetricHeartRateBPM, now, now);
    if (hr_mask & HealthServiceAccessibilityMaskAvailable) {
      s_hr = health_service_peek_current_value(HealthMetricHeartRateBPM);
    }
  }
  #endif
  health_update();

}

void health_init() {

  settings_register_callback(health_settings_callback, SETTINGS_CALLBACK_HEALTH);

  if(!global_settings.Health) {
    return;
  }

  health_layer = layer_create(HEALTH_LAYER);
  layer_add_child(my_window_layer, health_layer);

  health_text_layer = text_layer_create_detailed(HEALTH_TEXT_LAYER, false,
                                GColorClear, color_helper(colors[c_h1], global_settings.Invert),
                                GTextAlignmentLeft, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  layer_add_child(health_layer, text_layer_get_layer(health_text_layer));

  // HR inline with date
  hr_heart_layer = layer_create(GRect(52, 86, 14, 12));
  layer_set_update_proc(hr_heart_layer, hr_heart_update_proc);
  layer_add_child(my_window_layer, hr_heart_layer);
  layer_set_hidden(hr_heart_layer, true);

  hr_text_layer = text_layer_create_detailed(GRect(70, 74, 48, 28), false,
                                GColorClear, color_helper(colors[c_h2], global_settings.Invert),
                                GTextAlignmentLeft, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  text_layer_set_text(hr_text_layer, "");
  layer_add_child(my_window_layer, text_layer_get_layer(hr_text_layer));

  foot_path_ptr = gpath_create(&FOOT_PATH_INFO);
  heel_path_ptr = gpath_create(&HEEL_PATH_INFO);
  zee1_path_ptr = gpath_create(&ZEE1_PATH_INFO);
  zee2_path_ptr = gpath_create(&ZEE2_PATH_INFO);
  zee3_path_ptr = gpath_create(&ZEE3_PATH_INFO);

  health_zee_layer = layer_create(HEALTH_ZEE_LAYER);
  layer_set_update_proc(health_zee_layer, health_icon_layer_update_callback);
  layer_add_child(health_layer, health_zee_layer);

  health_foot_layer = layer_create(HEALTH_FOOT_LAYER);
  layer_set_update_proc(health_foot_layer, health_icon_layer_update_callback);
  layer_add_child(health_layer, health_foot_layer);

  health_foot2_layer = layer_create(HEALTH_FOOT2_LAYER);
  layer_set_update_proc(health_foot2_layer, health_icon_layer_update_callback);
  layer_add_child(health_layer, health_foot2_layer);

  health_service_events_subscribe(health_handler, NULL);
	health_handler(HealthEventMovementUpdate, NULL);
	health_handler(HealthEventSleepUpdate, NULL);

  health_enabled = true;
}

void health_deinit() {

  if(!health_enabled) {
      return;
  }
  health_service_events_unsubscribe();

  gpath_destroy(foot_path_ptr);
  foot_path_ptr = NULL;

  gpath_destroy(heel_path_ptr);
  heel_path_ptr = NULL;

  gpath_destroy(zee1_path_ptr);
  zee1_path_ptr = NULL;

  gpath_destroy(zee2_path_ptr);
  zee2_path_ptr = NULL;

  gpath_destroy(zee3_path_ptr);
  zee3_path_ptr = NULL;

  layer_destroy(health_foot_layer);
  layer_destroy(health_foot2_layer);
  layer_destroy(health_zee_layer);
  text_layer_destroy(health_text_layer);
  layer_destroy(health_layer);

  if (hr_text_layer) {
    text_layer_destroy(hr_text_layer);
    hr_text_layer = NULL;
  }
  if (hr_heart_layer) {
    layer_destroy(hr_heart_layer);
    hr_heart_layer = NULL;
  }

  health_enabled = false;

}

#endif
