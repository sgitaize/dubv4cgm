#pragma once
#include <pebble.h>

// Set to 1 to enable debug logging (zero cost when 0)
#define DEBUG_LOGGING 0

#if DEBUG_LOGGING
  #define DBG_LOG(...) APP_LOG(APP_LOG_LEVEL_INFO, __VA_ARGS__)
#else
  #define DBG_LOG(...)
#endif

// Set to 1 to enable debug drawing (virtual line overlays)
#define DEBUG_DRAW_LINES 0

#define ANIM_DURATION 700

typedef enum {UP, DOWN, LEFT, RIGHT} direction_t;

// SCREEN
#define FULLSCREEN GRect(0, 0, 200, 228)

// BACKGROUND — concentric panels, full width, 3px inset each (round(2*1.35))
#define BACKGROUND_PANEL GRect(0, 46, 200, 138)
#define BACKGROUND_PANEL_OUTER GRect(0, 0, 200, 138)
#define BACKGROUND_PANEL_MIDDLE GRect(2, 2, 196, 134)
#define BACKGROUND_PANEL_INNER GRect(4, 4, 192, 130)

// BATTERY — terminal position DERIVED from scaled icon dimensions
#define BATTERY_LAYER GRect(166, 58, 23, 12)
#define BATTERY_PERCENT GRect(116, 55, 47, 22)
#define BATTERY_ICON GRect(0, 0, 22, 12)
// terminal: 1px line at right edge, height matches fill, vertically centered
#define BATTERY_ICON_TERMINAL GRect(22, 2, 1, 8)
#define BATTERY_FILL_OFFSET_X 2
#define BATTERY_FILL_OFFSET_Y 2
#define BATTERY_FILL_HEIGHT 8
// fill_max at 100%: icon_w - terminal_w - 1 - 2*offset = 22-3-1-4 = 14, then +1 = 15... but we want fill to reach x=20 (terminal at 21, gap 1)
// fill starts at x=2, so max_width = 20-2 = 18, ceil = 19
#define BATTERY_FILL_MAX_W 18
#define BATTERY_FILL_CEIL 19

// HEALTH
#define HEALTH_LAYER GRect(37, 55, 95, 22)
#define HEALTH_TEXT_LAYER GRect(20, 0, 90, 22)
#define HEALTH_ZEE_LAYER GRect(0, 4, 19, 9)
#define HEALTH_FOOT_LAYER GRect(4, 0, 12, 16)
#define HEALTH_FOOT2_LAYER GRect(11, 4, 12, 16)
#define HEALTH_STEP_MIN 400

// BLUETOOTH — circle center/radius scaled from original (5,5,r5) in 13x13 layer
#define BLUETOOTH_LAYER GRect(15, 57, 18, 18)
#define BLUETOOTH_ICON_CIRCLE GRect(0, 0, 18, 18)
#define BLUETOOTH_ICON_SYMBOL GRect(3, 1, 9, 12)
// circle center: round(5*1.35)=7, radius: round(5*1.35)=7
#define BT_CIRCLE_CENTER GPoint(7, 7)
#define BT_CIRCLE_RADIUS 7

// TIMEDIGITS — virtual line offsets from BACKGROUND_PANEL edges
#define TIMEDIGITS_VIRTUAL_TOP_OFFSET 54
#define TIMEDIGITS_VIRTUAL_BOTTOM_OFFSET 5

// TIMEDIGITS — per-font layout parameters
typedef struct {
  int16_t group_center_dy;  // vertical offset of group center from virtual center line
  int16_t ss_dy;     // SS offset from HH:MM top
  int16_t ss_gap;    // Gap between HH:MM and SS
  int16_t date_bottom_dy;  // date bottom offset from BACKGROUND_PANEL bottom (+ = below)
  int16_t h_offset;  // Horizontal offset applied after centering
} FontLayoutParams;

// TIMEDIGITS — base references for dynamic positioning
#define TIMEDIGITS_CENTER GRect(2, 20, 194, 227)
#define TIMEDIGITS_DATE GRect(2, 51, 186, 40)
#define TIMEDIGITS_AMPM GRect(23, 66, 77, 40)

// TIMEDIGITS — per-font layout parameters (emery)
static const FontLayoutParams font_layouts[] = {
  [0] = { .group_center_dy = -4, .ss_dy = 23, .ss_gap = 0, .date_bottom_dy = -81, .h_offset = -3 },  // DS-Digital
  [1] = { .group_center_dy = 1, .ss_dy = 13, .ss_gap = 0, .date_bottom_dy = -84, .h_offset = -2 },  // DSEG
  [2] = { .group_center_dy = 1, .ss_dy = 13, .ss_gap = 0, .date_bottom_dy = -84, .h_offset = -2 },  // DSEG Bold
};

// DECORATIONS — full-width lines, scaled positions for buttons/WR
#define DECORATIONS_LINE_TOP_START GPoint(0, 26)
#define DECORATIONS_LINE_TOP_END GPoint(200, 26)
#define DECORATIONS_LINE_BOTTOM_START GPoint(0, 203)
#define DECORATIONS_LINE_BOTTOM_END GPoint(200, 203)
#define DECORATIONS_BUTTON_BACK_LABEL GRect(21, 28, 47, 14)
#define DECORATIONS_BUTTON_NEXT_LABEL GRect(146, 185, 34, 14)
#define DECORATIONS_BUTTON_PREV_LABEL GRect(146, 28, 34, 14)
#define DECORATIONS_BUTTON_BACK_ICON GRect(10, 33, 7, 8)
#define DECORATIONS_BUTTON_NEXT_ICON GRect(183, 33, 7, 8)
#define DECORATIONS_BUTTON_PREV_ICON GRect(183, 191, 7, 8)
#define DECORATIONS_BRANDING GRect(0, -5, 200, 40)
#define DECORATIONS_WR_OUTER GRect(72, 200, 57, 22)
#define DECORATIONS_WR_WATER GRect(3, 205, 62, 27)
#define DECORATIONS_WR_RESIST GRect(137, 205, 65, 27)
#define DECORATIONS_LOGO GRect(18, 3, 161, 20)
// WR hole is relative to WR layer, NOT screen — no +3 offset
#define DECORATIONS_WR_HOLE GRect(36, 7, 4, 3)
