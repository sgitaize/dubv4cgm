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
            clayConfig.getItemByMessageKey('Set' + activeSet + '_' + colorKeys[j]).set(colorVal.replace('#', ''));
          }
        }
        break;
      }
    }
  }

 clayConfig.on(clayConfig.EVENTS.AFTER_BUILD, function() {
    var themeSelect = clayConfig.getItemById('Theme');
    var colorSetRadio = clayConfig.getItemById('ColorSet');
    var currentTheme = themeSelect.get();
    if (currentTheme) applyTheme(currentTheme);
    themeSelect.on('change', function() { applyTheme(this.get()); });
    colorSetRadio.on('change', function() { themeSelect.set(themeSelect.config.defaultValue); });
  });
};
