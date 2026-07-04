(function() {
  var themes = require('./themes.json');
  var timeOptions = [];
  for (var i = 0; i < 24; i++) {
    for (var j = 0; j < 2; j++) {
      timeOptions.push(String(i).replace(/^/, '0').slice(-2) + ':' + (j === 0 ? '00' : '30'));
    }
  }

  var defaults1 = {
    bg: ['0xFFFFFF', '0x000000', '0xFFFFFF', '0x000000'],
    bi: ['0x000000', '0xFFA500', '0xFF0000', '0xFF0000'],
    bl: ['0x00009C', '0xD3D3D3', '0xFF0000', '0xFFFFFF'],
    d:  ['0xFFFFFF', '0xFFFFFF', '0xFFFFFF', '0x000000',
        '0xFFFFFF', '0xFFFFFF', '0xFFFFFF', '0xFFFFFF', '0xFFFFFF'],
    t:  ['0x000000', '0x000000', '0xFFFFFF', '0x000000']
  };

  var labels1 = {
    bg: ['Outer Border', 'Inner Border', 'Inner Panel', 'Background'],
    bi: ['Battery', 'Battery Warning', 'Battery Critical', 'Battery Charging'],
    bl: ['Circle Connected', 'Icon Connected', 'Circle Disconnected', 'Icon Disconnected'],
    d:  ['Line Top', 'Line Bottom', 'Box Border', 'Box Fill',
          'WR Letters', 'Water Resist Text', 'Button Labels', 'Button Arrows', 'Branding Text'],
    t:  ['Date', 'AM/PM/24H', 'Time Shadow', 'Time']
  };

  var shortNames = {
    bg: ['bg1', 'bg2', 'bg3', 'bg4'],
    bi: ['bi1', 'bi2', 'bi3', 'bi4'],
    bl: ['bl1', 'bl2', 'bl3', 'bl4'],
    d:  ['d1', 'd2', 'd3', 'd4', 'd5', 'd6', 'd7', 'd8', 'd9'],
    t:  ['t1', 't2', 't3', 't4']
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

  var config = [
    { type: 'heading', defaultValue: 'Settings' },

    {
      type: 'section',
      items: [
        { type: 'heading', defaultValue: 'Display' },
        {
          type: 'toggle',
          messageKey: 'Health',
          label: 'Show Health Data',
          defaultValue: false
        },
        {
          type: 'toggle',
          messageKey: 'Seconds',
          label: 'Show Seconds',
          defaultValue: false
        },
        {
          type: 'select',
          messageKey: 'FontFaceDigital',
          label: 'Digital Font',
          defaultValue: '0',
          options: [
            { label: 'DS-Digital', value: '0' },
            { label: 'DSEG-Classic-Mini', value: '1' },
            { label: 'DSEG-Classic-Bold', value: '2' }
          ]
        },
        {
          type: 'select',
          messageKey: 'Blink',
          label: 'Separator Blink',
          defaultValue: 1,
          options: [
            { label: 'Off', value: '0' },
            { label: 'Normal', value: '1' },
            { label: 'Double Rate', value: '2' }
          ]
        },
        {
          type: 'toggle',
          messageKey: 'Invert',
          label: 'Invert Colors',
          defaultValue: false
        }
      ]
    },

    {
      type: 'section',
      items: [
        { type: 'heading', defaultValue: 'Anti-aliasing' },
        {
          type: 'toggle',
          messageKey: 'UseAntialiasing',
          label: 'Use Anti-aliasing',
          defaultValue: false
        },
        {
          type: 'select',
          messageKey: 'ColorBias',
          label: 'Intensity',
          defaultValue: '0',
          options: [
            { label: '-1', value: '-1' },
            { label: '0', value: '0' },
            { label: '+1', value: '1' }
          ]
        }
      ]
    },

    {
      type: 'section',
      items: [
        { type: 'heading', defaultValue: 'Notifications' },
        {
          type: 'toggle',
          messageKey: 'BluetoothVibe',
          label: 'Bluetooth Disconnect Vibe',
          defaultValue: true
        },
        {
          type: 'toggle',
          messageKey: 'HourlyVibe',
          label: 'Hourly Vibe Pulse',
          defaultValue: true
        }
      ]
    },

    {
      type: 'section',
      items: [
        { type: 'heading', defaultValue: 'Appearance' },
        {
          type: 'select',
          messageKey: 'Logo',
          label: 'Branding Logo',
          defaultValue: '1',
          options: [
            { label: 'Pebble old', value: '0' },
            { label: 'Pebble new', value: '1' }
          ]
        },
        {
          type: 'toggle',
          messageKey: 'BrandingMask',
          label: 'Hide Branding',
          defaultValue: false
        },
        {
          type: 'toggle',
          messageKey: 'BatteryHide',
          label: 'Hide Battery',
          defaultValue: false
        }
      ]
    },

    {
      type: 'section',
      items: [
        { type: 'heading', defaultValue: 'Power Save' },
        {
          type: 'toggle',
          messageKey: 'PowerSave',
          label: 'Enable Power Save',
          defaultValue: false
        },
        {
          type: 'select',
          messageKey: 'PS_Start',
          label: 'Power Save Start',
          defaultValue: '23:00',
          options: timeOptions.map(function(t) { return {label: t, value: t}; })
        },
        {
          type: 'select',
          messageKey: 'PS_End',
          label: 'Power Save End',
          defaultValue: '07:00',
          options: timeOptions.map(function(t) { return {label: t, value: t}; })
        }
      ]
    },

    {
      type: 'section',
      items: [
        { type: 'heading', defaultValue: 'Color Set Switching' },
        {
          type: 'select',
          messageKey: 'SwitchSet',
          label: 'Switch Mode',
          defaultValue: '0',
          options: [
            { label: 'Inactive', value: '0' },
            { label: 'By Time', value: '1' },
            { label: 'By Tap', value: '2' }
          ]
        },
        {
          type: 'select',
          messageKey: 'SwitchStart',
          label: 'Switch To Set 2 At',
          defaultValue: '23:00',
          options: timeOptions.map(function(t) { return {label: t, value: t}; })
        },
        {
          type: 'select',
          messageKey: 'SwitchEnd',
          label: 'Switch To Set 1 At',
          defaultValue: '07:00',
          options: timeOptions.map(function(t) { return {label: t, value: t}; })
        }
      ]
    },

    {
      type: 'section',
      items: [
        { type: 'heading', defaultValue: 'Backlight Color' },
        {
          type: 'toggle',
          messageKey: 'Set1_lightColorEnabled',
          label: 'Set 1 Custom Backlight',
          defaultValue: false
        },
        {
          type: 'color',
          messageKey: 'Set1_lightColor',
          label: 'Set 1 Backlight Color',
          allowGray: false,
          sunlight: true,
          defaultValue: '0xFFFFFF'
        },
        {
          type: 'toggle',
          messageKey: 'Set2_lightColorEnabled',
          label: 'Set 2 Custom Backlight',
          defaultValue: false
        },
        {
          type: 'color',
          messageKey: 'Set2_lightColor',
          label: 'Set 2 Backlight Color',
          allowGray: false,
          sunlight: true,
          defaultValue: '0xFFFFFF'
        }
      ]
    },

    {
      type: 'section',
      items: [
        { type: 'heading', defaultValue: 'Color Theme' },
        {
          type: 'radiogroup',
          id: 'ColorSet',
          label: 'Apply to Color Set',
          defaultValue: '1',
          options: [
            { label: 'Set 1', value: '1' },
            { label: 'Set 2', value: '2' }
          ]
        },
        {
          type: 'select',
          id: 'Theme',
          label: 'Theme',
          defaultValue: '',
          options: [
             { label: 'Select a Theme', value: '' }
          ].concat(themes.map(function(t) { return { label: t.theme[0].name, value: t.theme[0].name }; }))
        }
      ]
    },

    {
      type: 'section',
      items: [
        { type: 'heading', defaultValue: 'Color Set 1' },
        {
          type: 'preview',
          id: 'PreviewSet1',
          defaultValue: ''
        }
      ].concat(set1Items)
    },

    {
      type: 'section',
      items: [
        { type: 'heading', defaultValue: 'Color Set 2' },
        {
          type: 'preview',
          id: 'PreviewSet2',
          defaultValue: ''
        }
      ].concat(set2Items)
    },

    {
      type: 'section',
      items: [
        { type: 'heading', defaultValue: 'Export Themes' },
        { type: 'text', defaultValue: '<p>Tap the Export button, then select all text in the area below and copy.</p>' },
        { type: 'themeexport', id: 'ThemeExport', defaultValue: '' },
        { type: 'button', id: 'ExportThemes', defaultValue: 'Export' },
        { type: 'text', defaultValue: '<p>If you want to add your theme to the built-in themes, submit a PR or an issue at <a href="https://codeberg.org/lightrush/91-dub-v5" target="_blank">https://codeberg.org/lightrush/91-dub-v5</a> and paste it there.</p>' }
      ]
    },

    {
      type: 'submit',
      defaultValue: 'Save Settings'
    }
  ];

  module.exports = config;
})();
