module.exports = function(minified) {
  var clayConfig = this;
  var $ = minified.$;
  var themes = clayConfig.meta.userData.themes;

  var colorKeys = [
    'bg1', 'bg2', 'bg3', 'bg4',
    'bi1', 'bi2', 'bi3', 'bi4',
    'bl1', 'bl2', 'bl3', 'bl4',
    'd1', 'd2', 'd3', 'd4', 'd5', 'd6', 'd7', 'd8', 'd9',
    't1', 't2', 't3', 't4'
  ];

  var sunlightColorMap = {
    '000000': '000000', '000055': '001e41', '0000aa': '004387', '0000ff': '0068ca',
    '005500': '2b4a2c', '005555': '27514f', '0055aa': '16638d', '0055ff': '007dce',
    '00aa00': '5e9860', '00aa55': '5c9b72', '00aaaa': '57a5a2', '00aaff': '4cb4db',
    '00ff00': '8ee391', '00ff55': '8ee69e', '00ffaa': '8aebc0', '00ffff': '84f5f1',
    '550000': '4a161b', '550055': '482748', '5500aa': '40488a', '5500ff': '2f6bcc',
    '555500': '564e36', '555555': '545454', '5555aa': '4f6790', '5555ff': '4180d0',
    '55aa00': '759a64', '55aa55': '759d76', '55aaaa': '71a6a4', '55aaff': '69b5dd',
    '55ff00': '9ee594', '55ff55': '9de7a0', '55ffaa': '9becc2', '55ffff': '95f6f2',
    'aa0000': '99353f', 'aa0055': '983e5a', 'aa00aa': '955694', 'aa00ff': '8f74d2',
    'aa5500': '9d5b4d', 'aa5555': '9d6064', 'aa55aa': '9a7099', 'aa55ff': '9587d5',
    'aaaa00': 'afa072', 'aaaa55': 'aea382', 'aaaaaa': 'ababab', 'ffffff': 'ffffff',
    'aaaaff': 'a7bae2', 'aaff00': 'c9e89d', 'aaff55': 'c9eaa7', 'aaffaa': 'c7f0c8',
    'aaffff': 'c3f9f7', 'ff0000': 'e35462', 'ff0055': 'e25874', 'ff00aa': 'e16aa3',
    'ff00ff': 'de83dc', 'ff5500': 'e66e6b', 'ff5555': 'e6727c', 'ff55aa': 'e37fa7',
    'ff55ff': 'e194df', 'ffaa00': 'f1aa86', 'ffaa55': 'f1ad93', 'ffaaaa': 'efb5b8',
    'ffaaff': 'ecc3eb', 'ffff00': 'ffeeab', 'ffff55': 'fff1b5', 'ffffaa': 'fff6d3'
  };

  var SVG_FILL = ['bl1', 'd4', 'd5'];
  var SVG_STROKE = ['bl2', 'd3'];
  var BORDER_KEYS = ['d1', 'd2'];

  function intToHex(val, applySunlight) {
    if (val === undefined || val === null || val === '') return '';
    var num;
    if (typeof val === 'number') {
      num = val;
    } else {
      var str = String(val).replace(/^(0x|#)/, '');
      num = parseInt(str, 16);
      if (isNaN(num)) {
        num = parseInt(val, 10);
      }
    }
    if (isNaN(num)) return '';
    var hex = num.toString(16).padStart(6, '0').toLowerCase();
    if (applySunlight) {
      hex = sunlightColorMap[hex] || hex;
    }
    return '#' + hex.toUpperCase();
  }

  function updatePreview(previewId, setNum) {
    var previewItem = clayConfig.getItemById(previewId);
    if (!previewItem || !previewItem.$element || !previewItem.$element[0]) return;
    var root = previewItem.$element[0];
    var colorMap = {};
    for (var i = 0; i < colorKeys.length; i++) {
      var item = clayConfig.getItemByMessageKey('Set' + setNum + '_' + colorKeys[i]);
      if (item) {
        colorMap[colorKeys[i]] = intToHex(item.get(), item.config && item.config.sunlight !== false);
      }
    }
    for (var i = 0; i < colorKeys.length; i++) {
      var key = colorKeys[i];
      var hex = colorMap[key];
      if (!hex) continue;
      var elems = root.querySelectorAll('.color-' + key);
      for (var e = 0; e < elems.length; e++) {
        var prop = elems[e].getAttribute('data-preview-prop');
        if (prop) {
          elems[e].style[prop] = hex;
        } else {
          var isFill = SVG_FILL.indexOf(key) !== -1;
          var isStroke = SVG_STROKE.indexOf(key) !== -1;
          var isBorder = BORDER_KEYS.indexOf(key) !== -1;
          if (isFill) elems[e].setAttribute('fill', hex);
          else if (isStroke) elems[e].setAttribute('stroke', hex);
          else if (isBorder) elems[e].style.borderTopColor = hex;
          else if (key.indexOf('bg') === 0) elems[e].style.backgroundColor = hex;
          else elems[e].style.color = hex;
        }
      }
    }
  }

  function applyTheme(themeName) {
    if (!themeName) return;
    var colorSetRadio = clayConfig.getItemById('ColorSet');
    var activeSet = parseInt(colorSetRadio.get());
    for (var i = 0; i < themes.length; i++) {
      if (themes[i].theme[0].name === themeName) {
        var td = themes[i].theme[0];
        for (var j = 0; j < colorKeys.length; j++) {
          var colorVal = td[colorKeys[j] + 'color'];
          if (colorVal) {
            clayConfig.getItemByMessageKey('Set' + activeSet + '_' + colorKeys[j]).set(parseInt(colorVal.replace('#', ''), 16));
          }
        }
        break;
      }
    }
    updatePreview('PreviewSet1', 1);
    updatePreview('PreviewSet2', 2);
  }

  var onPicker1Change = function() { updatePreview('PreviewSet1', 1); };
  var onPicker2Change = function() { updatePreview('PreviewSet2', 2); };

  clayConfig.on(clayConfig.EVENTS.AFTER_BUILD, function() {
    var themeSelect = clayConfig.getItemById('Theme');
    var currentTheme = themeSelect.get();
    if (currentTheme) applyTheme(currentTheme);
    themeSelect.on('change', function() { applyTheme(this.get()); });
    var colorSetRadio = clayConfig.getItemById('ColorSet');
    colorSetRadio.on('change', function() { themeSelect.set(themeSelect.config.defaultValue); });
    for (var i = 0; i < colorKeys.length; i++) {
      var key = colorKeys[i];
      var picker1 = clayConfig.getItemByMessageKey('Set1_' + key);
      var picker2 = clayConfig.getItemByMessageKey('Set2_' + key);
      if (picker1) picker1.on('change', onPicker1Change);
      if (picker2) picker2.on('change', onPicker2Change);
    }
    updatePreview('PreviewSet1', 1);
    updatePreview('PreviewSet2', 2);
  });
};
