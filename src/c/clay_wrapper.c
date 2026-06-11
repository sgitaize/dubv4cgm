#include <pebble.h>
#include "clay_wrapper.h"

static int8_t parse_time_to_halfday(const char *str) {
  if (!str || str[2] != ':') return 0;
  int8_t h = (str[0] - '0') * 10 + (str[1] - '0');
  int8_t m = (str[3] - '0') * 10 + (str[4] - '0');
  int8_t v = h * 2 + 1;
  if (m >= 30) v++;
  return v;
}

static uint8_t parse_cstring_uint8(const char *str) {
  if (!str) return 0;
  uint8_t v = 0;
  for (int i = 0; str[i] && str[i] >= '0' && str[i] <= '9'; i++) {
    v = v * 10 + (str[i] - '0');
  }
  return v;
}

static void clay_wrapper_inbox(DictionaryIterator *iter, void *context) {
  (void)context;
  int cnt = 0;

  Tuple *t = dict_read_first(iter);
  while (t) {
    cnt++;
    uint32_t key = t->key;

    // Set1 colors
    if (key == MESSAGE_KEY_Set1_bg1)  colorsSet1[c_bg1] = GColorFromHEX(t->value->int32);
    else if (key == MESSAGE_KEY_Set1_bg2) colorsSet1[c_bg2] = GColorFromHEX(t->value->int32);
    else if (key == MESSAGE_KEY_Set1_bg3) colorsSet1[c_bg3] = GColorFromHEX(t->value->int32);
    else if (key == MESSAGE_KEY_Set1_bg4) colorsSet1[c_bg4] = GColorFromHEX(t->value->int32);
    else if (key == MESSAGE_KEY_Set1_bi1) colorsSet1[c_bi1] = GColorFromHEX(t->value->int32);
    else if (key == MESSAGE_KEY_Set1_bi2) colorsSet1[c_bi2] = GColorFromHEX(t->value->int32);
    else if (key == MESSAGE_KEY_Set1_bi3) colorsSet1[c_bi3] = GColorFromHEX(t->value->int32);
    else if (key == MESSAGE_KEY_Set1_bi4) colorsSet1[c_bi4] = GColorFromHEX(t->value->int32);
    else if (key == MESSAGE_KEY_Set1_bl1) colorsSet1[c_bl1] = GColorFromHEX(t->value->int32);
    else if (key == MESSAGE_KEY_Set1_bl2) colorsSet1[c_bl2] = GColorFromHEX(t->value->int32);
    else if (key == MESSAGE_KEY_Set1_bl3) colorsSet1[c_bl3] = GColorFromHEX(t->value->int32);
    else if (key == MESSAGE_KEY_Set1_bl4) colorsSet1[c_bl4] = GColorFromHEX(t->value->int32);
    else if (key == MESSAGE_KEY_Set1_d1)  colorsSet1[c_d1] = GColorFromHEX(t->value->int32);
    else if (key == MESSAGE_KEY_Set1_d2)  colorsSet1[c_d2] = GColorFromHEX(t->value->int32);
    else if (key == MESSAGE_KEY_Set1_d3)  colorsSet1[c_d3] = GColorFromHEX(t->value->int32);
    else if (key == MESSAGE_KEY_Set1_d4)  colorsSet1[c_d4] = GColorFromHEX(t->value->int32);
    else if (key == MESSAGE_KEY_Set1_d5)  colorsSet1[c_d5] = GColorFromHEX(t->value->int32);
    else if (key == MESSAGE_KEY_Set1_d6)  colorsSet1[c_d6] = GColorFromHEX(t->value->int32);
    else if (key == MESSAGE_KEY_Set1_d7)  colorsSet1[c_d7] = GColorFromHEX(t->value->int32);
    else if (key == MESSAGE_KEY_Set1_d8)  colorsSet1[c_d8] = GColorFromHEX(t->value->int32);
    else if (key == MESSAGE_KEY_Set1_d9)  colorsSet1[c_d9] = GColorFromHEX(t->value->int32);
    else if (key == MESSAGE_KEY_Set1_t1)  colorsSet1[c_t1] = GColorFromHEX(t->value->int32);
    else if (key == MESSAGE_KEY_Set1_t2)  colorsSet1[c_t2] = GColorFromHEX(t->value->int32);
    else if (key == MESSAGE_KEY_Set1_t3)  colorsSet1[c_t3] = GColorFromHEX(t->value->int32);
    else if (key == MESSAGE_KEY_Set1_t4)  colorsSet1[c_t4] = GColorFromHEX(t->value->int32);

    // Set2 colors
    else if (key == MESSAGE_KEY_Set2_bg1)  colorsSet2[c_bg1] = GColorFromHEX(t->value->int32);
    else if (key == MESSAGE_KEY_Set2_bg2) colorsSet2[c_bg2] = GColorFromHEX(t->value->int32);
    else if (key == MESSAGE_KEY_Set2_bg3) colorsSet2[c_bg3] = GColorFromHEX(t->value->int32);
    else if (key == MESSAGE_KEY_Set2_bg4) colorsSet2[c_bg4] = GColorFromHEX(t->value->int32);
    else if (key == MESSAGE_KEY_Set2_bi1) colorsSet2[c_bi1] = GColorFromHEX(t->value->int32);
    else if (key == MESSAGE_KEY_Set2_bi2) colorsSet2[c_bi2] = GColorFromHEX(t->value->int32);
    else if (key == MESSAGE_KEY_Set2_bi3) colorsSet2[c_bi3] = GColorFromHEX(t->value->int32);
    else if (key == MESSAGE_KEY_Set2_bi4) colorsSet2[c_bi4] = GColorFromHEX(t->value->int32);
    else if (key == MESSAGE_KEY_Set2_bl1) colorsSet2[c_bl1] = GColorFromHEX(t->value->int32);
    else if (key == MESSAGE_KEY_Set2_bl2) colorsSet2[c_bl2] = GColorFromHEX(t->value->int32);
    else if (key == MESSAGE_KEY_Set2_bl3) colorsSet2[c_bl3] = GColorFromHEX(t->value->int32);
    else if (key == MESSAGE_KEY_Set2_bl4) colorsSet2[c_bl4] = GColorFromHEX(t->value->int32);
    else if (key == MESSAGE_KEY_Set2_d1)  colorsSet2[c_d1] = GColorFromHEX(t->value->int32);
    else if (key == MESSAGE_KEY_Set2_d2)  colorsSet2[c_d2] = GColorFromHEX(t->value->int32);
    else if (key == MESSAGE_KEY_Set2_d3)  colorsSet2[c_d3] = GColorFromHEX(t->value->int32);
    else if (key == MESSAGE_KEY_Set2_d4)  colorsSet2[c_d4] = GColorFromHEX(t->value->int32);
    else if (key == MESSAGE_KEY_Set2_d5)  colorsSet2[c_d5] = GColorFromHEX(t->value->int32);
    else if (key == MESSAGE_KEY_Set2_d6)  colorsSet2[c_d6] = GColorFromHEX(t->value->int32);
    else if (key == MESSAGE_KEY_Set2_d7)  colorsSet2[c_d7] = GColorFromHEX(t->value->int32);
    else if (key == MESSAGE_KEY_Set2_d8)  colorsSet2[c_d8] = GColorFromHEX(t->value->int32);
    else if (key == MESSAGE_KEY_Set2_d9)  colorsSet2[c_d9] = GColorFromHEX(t->value->int32);
    else if (key == MESSAGE_KEY_Set2_t1)  colorsSet2[c_t1] = GColorFromHEX(t->value->int32);
    else if (key == MESSAGE_KEY_Set2_t2)  colorsSet2[c_t2] = GColorFromHEX(t->value->int32);
    else if (key == MESSAGE_KEY_Set2_t3)  colorsSet2[c_t3] = GColorFromHEX(t->value->int32);
    else if (key == MESSAGE_KEY_Set2_t4)  colorsSet2[c_t4] = GColorFromHEX(t->value->int32);

    // Select settings (sent as cstring "0", "1", "2")
    else if (key == MESSAGE_KEY_Blink)     global_settings.Blink = parse_cstring_uint8(t->value->cstring);
    else if (key == MESSAGE_KEY_SwitchSet) global_settings.SwitchSet = parse_cstring_uint8(t->value->cstring);
    else if (key == MESSAGE_KEY_Logo)      global_settings.Logo = parse_cstring_uint8(t->value->cstring);

    // Time settings (sent as cstring "HH:MM")
    else if (key == MESSAGE_KEY_PS_Start)      global_settings.PS_Start = (uint8_t)parse_time_to_halfday(t->value->cstring);
    else if (key == MESSAGE_KEY_PS_End)        global_settings.PS_End = (uint8_t)parse_time_to_halfday(t->value->cstring);
    else if (key == MESSAGE_KEY_SwitchStart)   global_settings.SwitchStart = (uint8_t)parse_time_to_halfday(t->value->cstring);
    else if (key == MESSAGE_KEY_SwitchEnd)     global_settings.SwitchEnd = (uint8_t)parse_time_to_halfday(t->value->cstring);

    // Toggle settings (sent as int32 0 or 1)
    else if (key == MESSAGE_KEY_Health)        global_settings.Health = (uint8_t)t->value->int32;
    else if (key == MESSAGE_KEY_Invert)        global_settings.Invert = (uint8_t)t->value->int32;
    else if (key == MESSAGE_KEY_BluetoothVibe) global_settings.BluetoothVibe = (uint8_t)t->value->int32;
    else if (key == MESSAGE_KEY_HourlyVibe)    global_settings.HourlyVibe = (uint8_t)t->value->int32;
    else if (key == MESSAGE_KEY_BrandingMask)  global_settings.BrandingMask = (uint8_t)t->value->int32;
    else if (key == MESSAGE_KEY_BatteryHide)   global_settings.BatteryHide = (uint8_t)t->value->int32;
    else if (key == MESSAGE_KEY_Seconds)       global_settings.Seconds = (uint8_t)t->value->int32;
    else if (key == MESSAGE_KEY_PowerSave)     global_settings.PowerSave = (uint8_t)t->value->int32;

    t = dict_read_next(iter);
  }

  if (selectedSet == 0) {
    settings_load_colorSet1();
  } else {
    settings_load_colorSet2();
  }

  update_settings();

  if (delayed_save) {
    app_timer_cancel(delayed_save);
  }
  delayed_save = app_timer_register(100, settings_save, NULL);
}

void clay_wrapper_init() {
  settings_core_init();
  app_message_register_inbox_received(clay_wrapper_inbox);
  app_message_open(2048, 512);
}

void clay_wrapper_deinit() {
  settings_deinit();
}
