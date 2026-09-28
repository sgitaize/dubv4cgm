// Settings schema (Clay format, from 91 Dub v5 plus) + dubv4cgm sections.
// Rendered by index.html; window.DUB_THEMES must be loaded first.
(function() {
  var themes = window.DUB_THEMES || [];
  var timeOptions = [];
  for (var i = 0; i < 24; i++) {
    for (var j = 0; j < 2; j++) {
      timeOptions.push(String(i).replace(/^/, '0').slice(-2) + ':' + (j === 0 ? '00' : '30'));
    }
  }

  var defaults1 = {
    // 91 Dub v5 plus theme
    bg: ['0xFFAA00', '0x000000', '0xFFFFFF', '0x000000'],
    bi: ['0x00AA00', '0xFFAA00', '0xAA0000', '0x00009C'],
    bl: ['0x0000FF', '0xFFFFFF', '0xAA0000', '0xFFFFFF'],
    d:  ['0xFFAA00', '0xFFAA00', '0xFFAA00', '0x000000',
        '0xFFAA00', '0xFFAA00', '0xFFAA00', '0xFFAA00', '0xFFFFFF'],
    t:  ['0x000000', '0x000000', '0xFFFFFF', '0x000000'],
    h:  ['0x000000', '0xAA0000']
  };

  var labels1 = {
    bg: ['Outer Border', 'Inner Border', 'Inner Panel', 'Background'],
    bi: ['Battery', 'Battery Warning', 'Battery Critical', 'Battery Charging'],
    bl: ['Circle Connected', 'Icon Connected', 'Circle Disconnected', 'Icon Disconnected'],
    d:  ['Line Top', 'Line Bottom', 'Box Border', 'Box Fill',
          'WR Letters', 'Water Resist Text', 'Button Labels', 'Button Arrows', 'Branding Text'],
    t:  ['Date', 'AM/PM/24H', 'Time Shadow', 'Time'],
    h:  ['Steps / CGM', 'Heart Rate']
  };

  var shortNames = {
    bg: ['bg1', 'bg2', 'bg3', 'bg4'],
    bi: ['bi1', 'bi2', 'bi3', 'bi4'],
    bl: ['bl1', 'bl2', 'bl3', 'bl4'],
    d:  ['d1', 'd2', 'd3', 'd4', 'd5', 'd6', 'd7', 'd8', 'd9'],
    t:  ['t1', 't2', 't3', 't4'],
    h:  ['h1', 'h2']
  };

  function colorPicker(prefix, short, label, defaultVal) {
    return {
      type: 'color',
      messageKey: prefix + '_' + short,
      defaultValue: defaultVal,
      allowGray: false,
      sunlight: true,
      label: label
    };
  }

  function buildColorItems(prefix, defs, lbls, shorts) {
    var items = [];

    items.push(colorPicker(prefix, shorts.bg[3], lbls.bg[3], defs.bg[3]));
    items.push(colorPicker(prefix, shorts.d[8], lbls.d[8], defs.d[8]));

    items.push({ type: 'heading', defaultValue: 'Centre Panel', size: 4 });
    items.push(colorPicker(prefix, shorts.bg[0], lbls.bg[0], defs.bg[0]));
    items.push(colorPicker(prefix, shorts.bg[1], lbls.bg[1], defs.bg[1]));
    items.push(colorPicker(prefix, shorts.bg[2], lbls.bg[2], defs.bg[2]));

    items.push(colorPicker(prefix, shorts.t[0], lbls.t[0], defs.t[0]));
    items.push(colorPicker(prefix, shorts.t[1], lbls.t[1], defs.t[1]));
    items.push(colorPicker(prefix, shorts.t[3], lbls.t[3], defs.t[3]));
    items.push(colorPicker(prefix, shorts.t[2], lbls.t[2], defs.t[2]));

    items.push({ type: 'heading', defaultValue: 'Health', size: 4 });
    items.push(colorPicker(prefix, shorts.h[0], lbls.h[0], defs.h[0]));
    items.push(colorPicker(prefix, shorts.h[1], lbls.h[1], defs.h[1]));

    items.push({ type: 'heading', defaultValue: 'Lines', size: 4 });
    items.push(colorPicker(prefix, shorts.d[0], lbls.d[0], defs.d[0]));
    items.push(colorPicker(prefix, shorts.d[1], lbls.d[1], defs.d[1]));

    items.push({ type: 'heading', defaultValue: 'Water Resist', size: 4 });
    items.push(colorPicker(prefix, shorts.d[2], lbls.d[2], defs.d[2]));
    items.push(colorPicker(prefix, shorts.d[3], lbls.d[3], defs.d[3]));
    items.push(colorPicker(prefix, shorts.d[4], lbls.d[4], defs.d[4]));
    items.push(colorPicker(prefix, shorts.d[5], lbls.d[5], defs.d[5]));

    items.push({ type: 'heading', defaultValue: 'Buttons', size: 4 });
    items.push(colorPicker(prefix, shorts.d[6], lbls.d[6], defs.d[6]));
    items.push(colorPicker(prefix, shorts.d[7], lbls.d[7], defs.d[7]));

    items.push({ type: 'heading', defaultValue: 'Battery', size: 4 });
    items.push(colorPicker(prefix, shorts.bi[0], lbls.bi[0], defs.bi[0]));
    items.push(colorPicker(prefix, shorts.bi[1], lbls.bi[1], defs.bi[1]));
    items.push(colorPicker(prefix, shorts.bi[2], lbls.bi[2], defs.bi[2]));
    items.push(colorPicker(prefix, shorts.bi[3], lbls.bi[3], defs.bi[3]));

    items.push({ type: 'heading', defaultValue: 'Bluetooth', size: 4 });
    items.push(colorPicker(prefix, shorts.bl[0], lbls.bl[0], defs.bl[0]));
    items.push(colorPicker(prefix, shorts.bl[1], lbls.bl[1], defs.bl[1]));
    items.push(colorPicker(prefix, shorts.bl[2], lbls.bl[2], defs.bl[2]));
    items.push(colorPicker(prefix, shorts.bl[3], lbls.bl[3], defs.bl[3]));

    return items;
  }

  var set1Items = buildColorItems('Set1', defaults1, labels1, shortNames);
  var set2Items = buildColorItems('Set2', defaults1, labels1, shortNames);

  var slotOptions = [
            { label: 'Original label', value: '0' },
            { label: 'Empty', value: '1' },
            { label: 'CGM value + delta', value: '2' },
            { label: 'CGM age', value: '3' },
            { label: 'Weather', value: '4' },
            { label: 'Steps', value: '5' },
            { label: 'Heart rate', value: '6' },
            { label: 'Battery', value: '7' }
          ];

  var timeSel = timeOptions.map(function(t) { return { label: t, value: t }; });

  // Order: quick setup first (preset, Nightscout, complications), everything
  // else in collapsed sections. Same message keys and values as before.
  var config = [
    { type: 'heading', defaultValue: '91 Dub CGM' },
    {
      type: 'section',
      items: [
        { type: 'heading', defaultValue: 'Quick setup' },
        { type: 'select', messageKey: 'Language', label: 'Language', defaultValue: '0',
          options: [ { label: 'English', value: '0' }, { label: 'Deutsch', value: '1' } ],
          description: 'Watch texts (weather, date, status) and this page. Button labels stay as printed.' },
        { type: 'preview', id: 'PreviewTheme', defaultValue: '' },
        { type: 'themepicker', id: 'Theme', label: 'Colour preset', defaultValue: '',
          options: [ { label: 'Select a Theme', value: '' } ]
            .concat(themes.map(function(t) { return { label: t.theme[0].name, value: t.theme[0].name }; })) },
        { type: 'radiogroup', id: 'ColorSet', label: 'Apply to Color Set', defaultValue: '1',
          options: [ { label: 'Set 1', value: '1' }, { label: 'Set 2', value: '2' } ] }
      ]
    },
    {
      type: 'section',
      items: [
        { type: 'heading', defaultValue: 'Nightscout CGM' },
        { type: 'input', messageKey: 'NsUrl', label: 'Nightscout URL', defaultValue: '',
          attributes: { placeholder: 'https://my-site.herokuapp.com', type: 'url', autocapitalize: 'off', autocorrect: 'off' } },
        { type: 'input', messageKey: 'NsToken', label: 'Access token (optional)', defaultValue: '',
          attributes: { placeholder: 'readable-1234abcd', autocapitalize: 'off', autocorrect: 'off' } },
        { type: 'select', messageKey: 'CgmUnits', label: 'Units', defaultValue: '0',
          options: [ { label: 'mg/dL', value: '0' }, { label: 'mmol/L', value: '1' } ] },
        { type: 'input', messageKey: 'CgmHigh', label: 'High above', defaultValue: '180', attributes: { type: 'number', step: 'any' } },
        { type: 'input', messageKey: 'CgmLow', label: 'Low below', defaultValue: '70', attributes: { type: 'number', step: 'any' } }
      ]
    },
    {
      type: 'section',
      items: [
        { type: 'heading', defaultValue: 'Complications' },
        { type: 'select', messageKey: 'SlotMain', label: 'Panel top row', defaultValue: '0',
          options: [ { label: 'CGM', value: '0' }, { label: 'Steps / sleep (original)', value: '1' } ] },
        { type: 'select', messageKey: 'SlotTL', label: 'Top left (LIGHT)', defaultValue: '0', options: slotOptions },
        { type: 'select', messageKey: 'SlotTR', label: 'Top right (PREV)', defaultValue: '4', options: slotOptions },
        { type: 'select', messageKey: 'SlotBL', label: 'Bottom left', defaultValue: '3', options: slotOptions },
        { type: 'select', messageKey: 'SlotBR', label: 'Bottom right (NEXT)', defaultValue: '5', options: slotOptions },
        { type: 'select', messageKey: 'WeatherUnits', label: 'Temperature', defaultValue: '0',
          options: [ { label: '°C', value: '0' }, { label: '°F', value: '1' } ] },
        { type: 'text', defaultValue: 'Left-handed mode swaps the corners. Keep the two labels of a row short enough to fit next to each other. Weather: Open-Meteo, needs location access for the Pebble app.' }
      ]
    },
    {
      type: 'section', collapsed: true,
      items: [
        { type: 'heading', defaultValue: 'CGM: colours & alarms' },
        { type: 'select', messageKey: 'CgmInterval', label: 'Sensor interval', defaultValue: '5',
          options: [ { label: '5 minutes (Dexcom, Libre via xDrip …)', value: '5' }, { label: '1 minute (Libre 3 …)', value: '1' } ],
          description: 'Values older than 2x the interval are shown struck through.' },
        { type: 'toggle', messageKey: 'CgmColorize', label: 'Colour high / low values', defaultValue: true },
        { type: 'color', messageKey: 'CgmColorHigh', label: 'High colour', defaultValue: '0xFF5500', sunlight: true },
        { type: 'color', messageKey: 'CgmColorLow', label: 'Low colour', defaultValue: '0xFF0000', sunlight: true },
        { type: 'toggle', messageKey: 'CgmBacklight', label: 'Backlight in high / low colour', defaultValue: false,
          description: 'Pebble Time 2: while the value is out of range the backlight lights up in the high or low colour.' },
        { type: 'toggle', messageKey: 'VibeLow', label: 'Vibrate on low (every 10 min)', defaultValue: false },
        { type: 'toggle', messageKey: 'VibeHigh', label: 'Vibrate on high (every 10 min)', defaultValue: false }
      ]
    },
    {
      type: 'section', collapsed: true,
      items: [
        { type: 'heading', defaultValue: 'Display' },
        { type: 'toggle', messageKey: 'Health', label: 'Show Health Data', defaultValue: true },
        { type: 'toggle', messageKey: 'Seconds', label: 'Show Seconds', defaultValue: true },
        { type: 'select', messageKey: 'FontFaceDigital', label: 'Digital Font', defaultValue: '0',
          options: [ { label: 'DS-Digital', value: '0' }, { label: 'DSEG-Classic-Mini', value: '1' }, { label: 'DSEG-Classic-Bold', value: '2' } ] },
        { type: 'select', messageKey: 'Blink', label: 'Separator Blink', defaultValue: '0',
          options: [ { label: 'Off', value: '0' }, { label: 'Normal', value: '1' }, { label: 'Double Rate', value: '2' } ] },
        { type: 'toggle', messageKey: 'Invert', label: 'Invert Colors', defaultValue: false },
        { type: 'toggle', messageKey: 'LeftHand', label: 'Left Handed', defaultValue: false },
        { type: 'heading', defaultValue: 'Anti-aliasing', size: 4 },
        { type: 'toggle', messageKey: 'UseAntialiasing', label: 'Use Anti-aliasing', defaultValue: false },
        { type: 'select', messageKey: 'ColorBias', label: 'Intensity', defaultValue: '0',
          options: [ { label: '-1', value: '-1' }, { label: '0', value: '0' }, { label: '+1', value: '1' } ] },
        { type: 'heading', defaultValue: 'Appearance', size: 4 },
        { type: 'select', messageKey: 'Logo', label: 'Branding Logo', defaultValue: '1',
          options: [ { label: 'Pebble old', value: '0' }, { label: 'Pebble new', value: '1' } ] },
        { type: 'toggle', messageKey: 'BrandingMask', label: 'Hide Branding', defaultValue: false },
        { type: 'toggle', messageKey: 'BatteryHide', label: 'Hide Battery', defaultValue: false }
      ]
    },
    {
      type: 'section', collapsed: true,
      items: [
        { type: 'heading', defaultValue: 'Notifications' },
        { type: 'toggle', messageKey: 'BluetoothVibe', label: 'Bluetooth Disconnect Vibe', defaultValue: true },
        { type: 'toggle', messageKey: 'HourlyVibe', label: 'Hourly Vibe Pulse', defaultValue: false }
      ]
    },
    {
      type: 'section', collapsed: true,
      items: [
        { type: 'heading', defaultValue: 'Power Save' },
        { type: 'toggle', messageKey: 'PowerSave', label: 'Enable Power Save', defaultValue: false },
        { type: 'select', messageKey: 'PS_Start', label: 'Power Save Start', defaultValue: '23:00', options: timeSel },
        { type: 'select', messageKey: 'PS_End', label: 'Power Save End', defaultValue: '07:00', options: timeSel }
      ]
    },
    {
      type: 'section', collapsed: true,
      items: [
        { type: 'heading', defaultValue: 'Color Set Switching' },
        { type: 'select', messageKey: 'SwitchSet', label: 'Switch Mode', defaultValue: '0',
          options: [ { label: 'Inactive', value: '0' }, { label: 'By Time', value: '1' }, { label: 'By Tap', value: '2' } ] },
        { type: 'select', messageKey: 'SwitchStart', label: 'Switch To Set 2 At', defaultValue: '23:00', options: timeSel },
        { type: 'select', messageKey: 'SwitchEnd', label: 'Switch To Set 1 At', defaultValue: '07:00', options: timeSel }
      ]
    },
    {
      type: 'section', collapsed: true,
      items: [
        { type: 'heading', defaultValue: 'Backlight Color' },
        { type: 'toggle', messageKey: 'Set1_lightColorEnabled', label: 'Set 1 Custom Backlight', defaultValue: false },
        { type: 'color', messageKey: 'Set1_lightColor', label: 'Set 1 Backlight Color', allowGray: false, sunlight: true, defaultValue: '0xFFFFFF' },
        { type: 'toggle', messageKey: 'Set2_lightColorEnabled', label: 'Set 2 Custom Backlight', defaultValue: false },
        { type: 'color', messageKey: 'Set2_lightColor', label: 'Set 2 Backlight Color', allowGray: false, sunlight: true, defaultValue: '0xFFFFFF' }
      ]
    },
    {
      type: 'section', collapsed: true,
      items: [
        { type: 'heading', defaultValue: 'Color Set 1 (custom colours)' },
        { type: 'preview', id: 'PreviewSet1', defaultValue: '' }
      ].concat(set1Items)
    },
    {
      type: 'section', collapsed: true,
      items: [
        { type: 'heading', defaultValue: 'Color Set 2 (custom colours)' },
        { type: 'preview', id: 'PreviewSet2', defaultValue: '' }
      ].concat(set2Items)
    },
    {
      type: 'section', collapsed: true,
      items: [
        { type: 'heading', defaultValue: 'Export Themes' },
        { type: 'text', defaultValue: 'Tap the Export button, then select all text in the area below and copy.' },
        { type: 'themeexport', id: 'ThemeExport', defaultValue: '' },
        { type: 'button', id: 'ExportThemes', defaultValue: 'Export' },
        { type: 'text', defaultValue: 'If you want to add your theme to the built-in themes, submit a PR or an issue at <a href="https://github.com/sgitaize/dubv4cgm" target="_blank">github.com/sgitaize/dubv4cgm</a> and paste it there.' }
      ]
    }
  ];

  window.DUB_CONFIG = config;
})();
