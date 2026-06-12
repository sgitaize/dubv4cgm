# 91-dub-v5 Architecture & Patterns

## Overview

91-dub-v5 is a highly customizable digital watchface for Pebble Time 2 (Emery, 200x228). It features large digit time display, configurable color themes, battery/health/bluetooth indicators, decorative UI elements, and Clay-based phone configuration.

## Project Structure

```
91-dub-v5/
├── package.json          # Pebble SDK v3 config, emery target, multi-JS enabled
├── wscript               # Waf build script (pbl_build, bin_type='app')
├── src/
│   ├── c/
│   │   ├── main.c        # Entry point: init/deinit orchestration
│   │   ├── main.h        # Public declarations
│   │   ├── _globals.h    # Platform-specific dimension macros (emery/basalt/chalk)
│   │   ├── window.c      # Window creation and layer management
│   │   ├── window.h
│   │   ├── timedigits.c  # Time digits, date, AM/PM, separator blink, seconds
│   │   ├── timedigits.h
│   │   ├── battery.c     # Battery percentage, charging bolt icon, color thresholds
│   │   ├── battery.h
│   │   ├── bluetooth.c   # Connection status circle + BT symbol path
│   │   ├── bluetooth.h
│   │   ├── health.c      # Step count or sleep duration (PBL_HEALTH gate)
│   │   ├── health.h
│   │   ├── background.c  # Concentric panel fills with configurable colors
│   │   ├── background.h
│   │   ├── decorations.c # Lines, button labels, WR badge, branding logo
│   │   ├── decorations.h
│   │   ├── settings.c    # AppMessage inbox, persist, color sets, callbacks
│   │   ├── settings.h
│   │   ├── fonts.c       # Custom font loading (DS-Digital, Lucida)
│   │   ├── fonts.h
│   │   ├── helpers.c     # Utility functions (color invert, formatting, etc.)
│   │   ├── helpers.h
│   │   ├── unobstructed.c # Watch face obstruction handling (Pebble OS 5+)
│   │   ├── unobstructed.h
│   │   ├── clay_wrapper.c # Clay config AppMessage handler
│   │   └── clay_wrapper.h
│   └── pkjs/
│       ├── index.js      # PebbleKit JS: Clay config, handshake, settings sync
│       ├── config.js     # Clay configuration schema
│       ├── custom-clay.js # Clay customization overrides
│       └── themes.json   # Theme definitions for Clay UI
├── resources/
│   ├── fonts/            # DS-Digital-Bold.ttf, lucon.ttf
│   ├── images/           # logo2.png, menu_icon_91w.png
│   └── screenshots/      # Captured emulator screenshots
└── build/                # Compiled PBW artifacts
```

## Module Responsibilities

### main.c
Orchestrates the init/deinit lifecycle. Calls each module's `*_init()` on startup and `*_deinit()` on shutdown. Sends a handshake message (key `9999`) to the phone via AppMessage to signal readiness.

### window.c
Creates the root window and window layer. Exposes `my_window` and `my_window_layer` globally for child modules to attach layers. **Note: Uses flat `window_create()` + `window_stack_push()` pattern, not `window_set_window_handlers()`.**

### _globals.h
Defines all platform-specific dimensions as `#define` macros. Uses `#if defined(PBL_PLATFORM_EMERY)` / `#elif defined(PBL_RECT)` / `#else` guards for emery (200x228), basalt/diorite (144x168), and chalk (180x180). Contains positioning constants for every visual element: background panels, battery, health, bluetooth, time digits, seconds, and decorations.

### timedigits.c
Renders the primary time display as individual digit text layers. Supports:
- 12/24 hour format
- Optional seconds display (6 additional digits)
- Blinking separator (off, single rate, double rate)
- Date display (day abbreviation + day of month)
- AM/PM or "24H" indicator
- Color set switching via `timed_colorset()`

Subscribes to `tick_timer_service` with either `MINUTE_UNIT` or `SECOND_UNIT` depending on settings.

### battery.c
Draws a battery icon with fill level, charging bolt, and percentage text. Color thresholds: normal (>30%), warning (20-30%), critical (<20%). GPath bolt icon is pre-allocated.

### bluetooth.c
Draws a circle + BT symbol. Changes color on connection/disconnection. Optional vibration on reconnect. GPath BT symbol is pre-allocated.

### health.c
Conditionally compiled with `#ifdef PBL_HEALTH`. Shows either step count or sleep duration with corresponding icons (footprint or ZZZ). Subscribes to `health_service_events_subscribe()`.

### background.c
Draws the base background fill and concentric rounded panels. Three nested panels with configurable colors.

### decorations.c
Renders decorative elements: horizontal lines, button labels (LIGHT/NEXT/PREV), arrow icons, "WR" water resistance badge with custom GPath letters, and branding logo bitmap. All GPaths pre-allocated.

### settings.c
Central settings management:
- `global_settings` struct with all configurable options
- Two color sets (`colorsSet1`, `colorsSet2`) with automatic switching
- AppMessage inbox handler for receiving settings from phone
- Persistent storage via `persist_read_data()` / `persist_write_data()`
- Callback registry pattern: modules register callbacks via `settings_register_callback()`
- Power save mode with configurable start/end times
- Tap-to-switch color sets (when `SwitchSet==2`)

### fonts.c
Loads three custom fonts: `font_big` (time digits, size varies with seconds setting), `font_small` (date/seconds), `font_tiny` (labels). Fonts are resource-loaded, not bundled TTF.

### helpers.c
Utility functions: `color_helper()` (invert support), `color_inverted()`, `format_commas()`, `upcase()`, `hex_to_num()`, `text_layer_create_detailed()` convenience wrapper.

### unobstructed.c
Handles Pebble OS 5+ watch face obstruction (always-on complications). Subscribes to `unobstructed_area_service`. Adjusts window layer position and hides decorations when obstructed.

### clay_wrapper.c
Wraps the Clay configuration framework. Receives color and setting values from Clay web UI via AppMessage. Parses color hex values, toggle settings, select settings, and time-range settings. Registers inbox callback before `app_message_open()`.

**Important: `clay_wrapper.c` is the actual inbox handler for settings sent from the phone.** `settings.c`'s `settings_inbox()` is registered but overshadowed by `clay_wrapper_inbox()` which opens the AppMessage channel last. When adding a new setting, you must update all of:

1. `settings.h` — add field to `Settings` struct, add message key to enum
2. `settings.c` — add default value in `settings_default_values()`, add case in `settings_process_tuple()` (for fallback/compat)
3. `clay_wrapper.c` — add case in `clay_wrapper_inbox()` to parse the incoming tuple into `global_settings`
4. `config.js` — add the Clay UI control with the matching `messageKey`
5. `package.json` — add the key to the `messageKeys` array (required for the SDK to generate the C `MESSAGE_KEY_*` macro)

Select controls send values as cstrings (e.g. `"0"`, `"1"`), toggles send as int32 (0 or 1), colors send as int32 hex values.

## Architecture Patterns

### Callback Registry
Modules register callbacks with the settings system. When settings change, `update_settings()` fires all registered callbacks. This decouples modules from the settings system.

```c
// Module registers callback
settings_register_callback(my_settings_callback, SETTINGS_CALLBACK_MYMODULE);

// Settings system fires all callbacks
void update_settings() {
    for (int i = 0; i < SETTINGS_CALLBACKS_COUNT; i++) {
        if (settings_callbacks[i] != NULL) {
            settings_callbacks[i]();
        }
    }
}
```

### Module Lifecycle
Each module follows the pattern:
```c
void module_init() {
    // Create layers, GPaths, text layers
    // Subscribe to system services
    // Register settings callback
}

void module_deinit() {
    // Unregister settings callback
    // Unsubscribe from system services
    // Destroy GPaths
    // Remove layers from parent
    // Destroy layers
}
```

### Platform-Specific Dimensions
All positioning uses `#define` macros in `_globals.h` with platform guards. Each element has separate coordinates for emery, basalt/rect, and chalk/round.

## Build & Test Commands

```bash
cd 91-dub-v5
pebble build                                    # Build PBW
pebble install --emulator emery                 # Test in QEMU
pebble logs --emulator emery                    # View emulator logs
pebble screenshot --no-open --emulator emery    # Capture screenshot
pebble install --phone [PHONE IP/HOST]          # Install on phone
pebble install --logs --phone [PHONE IP/HOST]   # Install on phone with live logs
```

## Settings Message Keys

The watchface communicates with the phone via AppMessage using these key categories:
- **Toggles**: Health, Blink, Invert, BluetoothVibe, HourlyVibe, BrandingMask, BatteryHide, Seconds, PowerSave
- **Time ranges**: PS_Start, PS_End, SwitchStart, SwitchEnd
- **Color sets**: Set1_bg1..bg4, Set1_bi1..bi4, Set1_bl1..bl4, Set1_d1..d9, Set1_t1..t4 (same for Set2)
- **Selectors**: SwitchSet (0=manual, 1=timed, 2=tap), Theme, ColorSet
- **Handshake**: Key 9999 signals watch readiness

## Versioning

When updating the project version, you must update it in **both** `package.json` and `package-lock.json`. Updating only one will cause version mismatches.
