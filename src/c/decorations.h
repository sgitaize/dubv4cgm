#pragma once
#include <pebble.h>
#include "settings.h"

// Edge labels (index for decorations_set_label)
enum { DEC_LABEL_BACK = 0, DEC_LABEL_PREV = 1, DEC_LABEL_NEXT = 2, DEC_LABEL_FREE = 3, DEC_LABEL_COUNT = 4 };
void decorations_set_label(uint8_t idx, const char *text);

void decorations_settings_callback();
void decorations_layer_update_callback(Layer *my_layer, GContext* ctx);
void decorations_wr_outer_layer_update_callback(Layer *my_layer, GContext* ctx);
void decorations_init();
void decorations_deinit();
void decorations_toggle(bool is_obstructed);
