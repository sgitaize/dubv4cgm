#pragma once
#include <pebble.h>
#include "settings.h"

void timedigits_settings_callback();
void handle_tick(struct tm *tick_time, TimeUnits units_changed);
void timedigits_init();
void timedigits_deinit();
