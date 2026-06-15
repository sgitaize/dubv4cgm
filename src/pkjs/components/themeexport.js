// Selectable text area for copying settings. Uses user-select CSS to allow
// tap-and-hold copy on mobile WebViews where navigator.clipboard is unavailable.
module.exports = {
  name: 'themeexport',

  template: '<div style="padding:8px">' +
    '<textarea data-manipulator-target="true" style="width:100%;height:120px;font-size:11px;font-family:monospace;border:1px solid #666;border-radius:4px;background:#1a1a1a;color:#ccc;resize:vertical;user-select:text;-webkit-user-select:text"></textarea>' +
    '</div>',

  style: '[data-manipulator-target]{outline:none}',

  manipulator: 'val',

  defaults: {
    defaultValue: ''
  }
};
