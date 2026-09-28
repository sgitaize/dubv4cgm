#include <pebble.h>
#include "main.h"
#include "helpers.h"
#include "clay_wrapper.h"
#include "bluetooth.h"
#include "health.h"
#include "battery.h"
#include "timedigits.h"
#include "decorations.h"
#include "fonts.h"
#include "window.h"
#include "background.h"
#include "unobstructed.h"
#include "complications.h"

#define HANDSHAKE_KEY 9999

void reload(void){
  background_deinit();
  fonts_deinit();
  timedigits_deinit();
  battery_deinit();
  bluetooth_deinit();
  decorations_deinit();
  unobstructed_deinit();
  complications_deinit();
  fonts_init();
  background_init();
  decorations_init();
  timedigits_init();
  battery_init();
  #if defined (PBL_HEALTH)
    health_init();
  #endif
  bluetooth_init();
  complications_init();
  appStarted = true;
}

void handle_init(void) {
  appStarted = false;

  setlocale(LC_ALL, "");

  clay_wrapper_init();
  window_init();
  fonts_init();
  background_init();
  decorations_init();
  unobstructed_init();
  timedigits_init();
  if (!powerSaveEngaged) {
    battery_init();
    #if defined (PBL_HEALTH)
      health_init();
    #endif
    bluetooth_init();
  }
  complications_init();

  appStarted = true;
  update_settings();

  // Signal phone that watch is ready for settings
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  dict_write_uint8(iter, HANDSHAKE_KEY, 1);
  app_message_outbox_send();
}

void handle_deinit(void) {
  clay_wrapper_deinit();
  complications_deinit();
  background_deinit();
  fonts_deinit();
  timedigits_deinit();
  if (!powerSaveEngaged) {
    battery_deinit();
    #if defined (PBL_HEALTH)
      health_deinit();
    #endif
    bluetooth_deinit();
  }
  decorations_deinit();
  unobstructed_deinit();
  window_deinit();
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}
