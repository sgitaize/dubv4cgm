#!/usr/bin/env bash
# Uploads the current build to the Pebble appstore and publishes the release
# publicly (--is-published). Requires a prior `pebble login`.
# Usage: store/publish.sh [extra pebble publish args, e.g. --replace-screenshots]
set -euo pipefail
export PATH=$HOME/.local/bin:$PATH
cd "$(dirname "$0")/.."
pebble login --status
pebble publish \
  --non-interactive \
  --is-published \
  --name "91 Dub CGM" \
  --description "$(cat store/description.txt)" \
  --source "https://github.com/sgitaize/dubv4cgm" \
  --release-notes "$(cat store/release-notes.txt)" \
  --screenshots store/emery_1_classic.png store/emery_2_indiglow_high.png \
                store/emery_3_gameboy_mmol_de.png store/emery_4_retro_future_low.png \
  "$@"
