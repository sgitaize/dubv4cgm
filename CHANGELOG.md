# Changelog

## dubv4cgm v1.1.0 (2026-09-28)

- Language setting English / Deutsch: weather, date, status/complication texts and the settings page itself (button labels stay English) (default: phone language)
- Settings page reorganised: quick setup on top (language, colour preset with preview and ◀ ▶ / random, Nightscout, complications), everything else in collapsible sections

## dubv4cgm v1.0.0 (2026-09-28)

- Fork of 91 Dub v5 plus 6.0.20 as "91 Dub CGM" (new UUID)
- Nightscout CGM in the panel's top row (value, trend arrow, delta, high/low colours, stale = struck through, vibration and backlight colour on high/low)
- Configurable edge labels: CGM, CGM age, weather (Open-Meteo), steps, heart rate, battery
- Settings page moved from Clay to GitHub Pages (same options, themes and preview)

## v5.5.4

- Add option for left hand usage (180° rotated)

## v5.5.3

- Change default branding logo to Pebble new

## v5.5.2

- Fix time and date not shifting when Timeline Peek is active (#2)

## v5.5.1

- Add theme "W-36 Marlin Yellow Dot" by finnbc (#3)

## v5.5.0

- Add new fonts for time and date along with anti-aliasing

## v5.4.1
- Updated both Pebble logos

## v5.4.0
- Add the ability to export themes

## v5.3.0
- Introduce live color preview to the new settings

## v5.2.3
- Reset Theme select when ColorSet radio changes
- Remove unused Theme and ColorSet message keys, use Clay id instead
- Explicitly set sunlight and allowGray in the color pickers

## v5.2.2
- Touched up the new Pebble logo

## v5.2.1
- Clarify a few settings labels

## v5.2.0
- Add configurable RGB backlight color per color set

## v5.1.2
- Clarify color set selection label

## v5.1.1
- Fix color selections resetting to theme value when Settings is reopened

## v5.1.0
- Add switchable branding logo setting (Pebble old / Pebble new)

## v5.0.0
- Built targeting the PT2 platform
- Optimized for the new PT2 display resolution with improved digit and icon sizing. Looks better than auto-scaling 91 Dub v4
- Redesigned settings page, no longer dependent on external web site
- Preserves all v4.0 customizability, including color sets and themes
