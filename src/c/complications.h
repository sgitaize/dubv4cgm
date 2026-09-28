#pragma once
#include <pebble.h>

// Content of the four edge label slots (SlotTL/TR/BR/BL)
enum {
  SLOT_LABEL = 0,    // original text (LIGHT / PREV / NEXT / empty)
  SLOT_NONE = 1,
  SLOT_CGM = 2,      // "123 +3"
  SLOT_CGM_AGE = 3,  // "2 MIN AGO"
  SLOT_WEATHER = 4,  // "18° CLOUDY"
  SLOT_STEPS = 5,    // "3,572 STEPS"
  SLOT_HR = 6,       // "54 BPM"
  SLOT_BATTERY = 7   // "BAT 80%"
};

// CGM status (same semantics as casiocgm / Nightscout-supercgm)
enum { CGM_STATUS_OK = 0, CGM_STATUS_NO_DATA = 1, CGM_STATUS_NO_CONN = 2, CGM_STATUS_OLD = 3 };

// Returns true if the message only carried CGM/weather data (no settings)
bool complications_inbox(DictionaryIterator *iter);
void complications_minute_tick(void);
// Backlight colour for an out-of-range CGM value (setting CgmBacklight)
bool complications_backlight_color(GColor *out);
void complications_init(void);
void complications_deinit(void);
