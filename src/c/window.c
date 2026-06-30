#include <pebble.h>
#include "_globals.h"
#include "window.h"

Window *my_window;
Layer *my_window_layer, *my_shifting_layer;

void window_init() {
  my_window = window_create();
  my_window_layer = window_get_root_layer(my_window);

  window_set_background_color(my_window, GColorBlack);
  window_stack_push(my_window, false);
}

void window_deinit() {
  window_destroy(my_window);
}
