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
    bg: ['Background Outer', 'Background Mid', 'Background Inner', 'Screen Background'],
    bi: ['Battery Normal', 'Battery Warning', 'Battery Critical', 'Battery Charging'],
    bl: ['BT Circle Connected', 'BT Icon Connected', 'BT Circle Disconnected', 'BT Icon Disconnected'],
    d:  ['Line Top', 'Line Bottom', 'WR Box Stroke', 'WR Box Fill',
         'WR Letters', 'WR Text', 'Button Labels', 'Button Icons', 'Branding'],
    t:  ['Date', 'AM/PM Indicator', 'Digit Shadow', 'Digit Foreground']
  };

  var shortNames = {
    bg: ['bg1', 'bg2', 'bg3', 'bg4'],
    bi: ['bi1', 'bi2', 'bi3', 'bi4'],
    bl: ['bl1', 'bl2', 'bl3', 'bl4'],
    d:  ['d1', 'd2', 'd3', 'd4', 'd5', 'd6', 'd7', 'd8', 'd9'],
    t:  ['t1', 't2', 't3', 't4']
  };

  function colorPickers(prefix, labels, defaults, shorts) {
    var items = [];
    for (var i = 0; i < defaults.length; i++) {
      items.push({
        type: 'color',
        messageKey: prefix + '_' + shorts[i],
        defaultValue: defaults[i],
        label: labels[i]
      });
    }
    return items;
  }

  var set1Colors = [];
  set1Colors = set1Colors.concat(colorPickers('Set1', labels1.bg, defaults1.bg, shortNames.bg));
  set1Colors = set1Colors.concat(colorPickers('Set1', labels1.bi, defaults1.bi, shortNames.bi));
  set1Colors = set1Colors.concat(colorPickers('Set1', labels1.bl, defaults1.bl, shortNames.bl));
  set1Colors = set1Colors.concat(colorPickers('Set1', labels1.d, defaults1.d, shortNames.d));
  set1Colors = set1Colors.concat(colorPickers('Set1', labels1.t, defaults1.t, shortNames.t));

  var set2Colors = [];
  set2Colors = set2Colors.concat(colorPickers('Set2', labels1.bg, defaults1.bg, shortNames.bg));
  set2Colors = set2Colors.concat(colorPickers('Set2', labels1.bi, defaults1.bi, shortNames.bi));
  set2Colors = set2Colors.concat(colorPickers('Set2', labels1.bl, defaults1.bl, shortNames.bl));
  set2Colors = set2Colors.concat(colorPickers('Set2', labels1.d, defaults1.d, shortNames.d));
  set2Colors = set2Colors.concat(colorPickers('Set2', labels1.t, defaults1.t, shortNames.t));

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
        { type: 'heading', defaultValue: 'Colors' },
        {
          type: 'radiogroup',
          messageKey: 'ColorSet',
          label: 'Edit Color Set',
          defaultValue: '1',
          options: [
            { label: 'Set 1', value: '1' },
            { label: 'Set 2', value: '2' }
          ]
        },
        {
          type: 'select',
          messageKey: 'Theme',
          label: 'Theme',
          defaultValue: '',
          options: [
            { label: '(Custom)', value: '' }
          ].concat(themes.map(function(t) { return { label: t.theme[0].name, value: t.theme[0].name }; }))
        }
      ]
    },

    {
      type: 'section',
      items: [
        { type: 'heading', defaultValue: 'Set 1 Colors' }
      ].concat(set1Colors)
    },

    {
      type: 'section',
      items: [
        { type: 'heading', defaultValue: 'Set 2 Colors' }
      ].concat(set2Colors)
    },

    {
      type: 'submit',
      defaultValue: 'Save Settings'
    }
  ];

  module.exports = config;
})();
