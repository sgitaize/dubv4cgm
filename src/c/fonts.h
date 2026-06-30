#pragma once
#include <pebble.h>
#include <pebble-fctx/ffont.h>

extern GFont font_big, font_small, font_tiny;

FFont *get_fctx_font();
void fonts_fctx_load();
void fonts_fctx_unload();
void fonts_settings_callback();
void fonts_init();
void fonts_deinit();