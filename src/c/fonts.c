#include <pebble.h>
#include "fonts.h"
#include "settings.h"

GFont font_big, font_small, font_tiny;

static FFont *fctx_font = NULL;

FFont *get_fctx_font() {
  return fctx_font;
}

static FFont *load_fctx_font() {
  uint32_t res_id;
  if (global_settings.FontFaceDigital == 0) {
    res_id = RESOURCE_ID_FONT_DIGITALE_FFONT;
  } else if (global_settings.FontFaceDigital == 1) {
    res_id = RESOURCE_ID_FONT_DSEG_FFONT;
  } else {
    res_id = RESOURCE_ID_FONT_DSEG_BOLD_FFONT;
  }
  return ffont_create_from_resource(res_id);
}

void fonts_fctx_load() {
  if (!fctx_font) {
    fctx_font = load_fctx_font();
  }
}

void fonts_fctx_unload() {
  if (fctx_font) {
    ffont_destroy(fctx_font);
    fctx_font = NULL;
  }
}

void fonts_settings_callback() {

  //APP_LOG(APP_LOG_LEVEL_DEBUG, "fonts_settings_callback()");

  fonts_unload_custom_font(font_big);
  fonts_unload_custom_font(font_small);
  int32_t big_res, small_res;
  if (global_settings.FontFaceDigital == 0) {
    big_res = (global_settings.Seconds && !powerSaveEngaged) ? RESOURCE_ID_FONT_DIGITALE_68 : RESOURCE_ID_FONT_DIGITALE_80;
    small_res = RESOURCE_ID_FONT_DIGITALE_28;
  } else if (global_settings.FontFaceDigital == 1) {
    big_res = (global_settings.Seconds && !powerSaveEngaged) ? RESOURCE_ID_FONT_DSEG_44 : RESOURCE_ID_FONT_DSEG_52;
    small_res = RESOURCE_ID_FONT_DSEG_18;
  } else {
    big_res = (global_settings.Seconds && !powerSaveEngaged) ? RESOURCE_ID_FONT_DSEG_BOLD_44 : RESOURCE_ID_FONT_DSEG_BOLD_52;
    small_res = RESOURCE_ID_FONT_DSEG_BOLD_18;
  }
  font_big = fonts_load_custom_font(resource_get_handle(big_res));
  font_small = fonts_load_custom_font(resource_get_handle(small_res));
  fonts_fctx_unload();
  fctx_font = load_fctx_font();
}
void fonts_init() {

  //APP_LOG(APP_LOG_LEVEL_DEBUG, "fonts_init()");

  int32_t big_res, small_res;
  if (global_settings.FontFaceDigital == 0) {
    big_res = (global_settings.Seconds && !powerSaveEngaged) ? RESOURCE_ID_FONT_DIGITALE_68 : RESOURCE_ID_FONT_DIGITALE_80;
    small_res = RESOURCE_ID_FONT_DIGITALE_28;
  } else if (global_settings.FontFaceDigital == 1) {
    big_res = (global_settings.Seconds && !powerSaveEngaged) ? RESOURCE_ID_FONT_DSEG_44 : RESOURCE_ID_FONT_DSEG_52;
    small_res = RESOURCE_ID_FONT_DSEG_18;
  } else {
    big_res = (global_settings.Seconds && !powerSaveEngaged) ? RESOURCE_ID_FONT_DSEG_BOLD_44 : RESOURCE_ID_FONT_DSEG_BOLD_52;
    small_res = RESOURCE_ID_FONT_DSEG_BOLD_18;
  }
  font_big = fonts_load_custom_font(resource_get_handle(big_res));
  font_small = fonts_load_custom_font(resource_get_handle(small_res));
  font_tiny = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_LUCIDIA_14));
  fonts_fctx_load();
  settings_register_callback(fonts_settings_callback, SETTINGS_CALLBACK_FONT);

}
void fonts_deinit() {

  //APP_LOG(APP_LOG_LEVEL_DEBUG, "fonts_deinit()");

  fonts_unload_custom_font(font_big);
  fonts_unload_custom_font(font_small);
  fonts_unload_custom_font(font_tiny);
  fonts_fctx_unload();
}