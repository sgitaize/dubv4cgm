var Clay = require('@rebble/clay');
var clayConfig = require('./config');
var customClay = require('./custom-clay');
var messageKeys = require('../../build/js/message_keys.json');
var themes = require('./themes.json');

var clay = new Clay(clayConfig, customClay, { autoHandleEvents: false });
clay.registerComponent(require('./components/preview'));
clay.registerComponent(require('./components/themeexport'));

var SETTINGS_STORAGE_KEY = '91dub_settings';
var HANDSHAKE_KEY = 9999;
var watchReady = false;

function prepareValue(val) {
  if (val && typeof val === 'object' && !Array.isArray(val)) {
    if (typeof val.value === 'number') {
      return Math.floor(val.value * Math.pow(10, val.precision || 0));
    }
    if (Array.isArray(val.value)) {
      return val.value.map(function(v) {
        return typeof v === 'number' ? Math.floor(v * Math.pow(10, val.precision || 0)) : v;
      });
    }
    return prepareValue(val.value);
  }
  if (Array.isArray(val)) return val.map(prepareValue);
  if (typeof val === 'boolean') return val ? 1 : 0;
  return val;
}

function convertSettings(raw) {
  var result = {};
  Object.keys(raw).forEach(function(key) {
    var val = raw[key];
    if (val && typeof val === 'object' && !Array.isArray(val) && 'value' in val) {
      val = val.value;
    }
    var numericKey = messageKeys[key];
    if (numericKey !== undefined) {
      result[numericKey] = prepareValue(val);
    }
  });
  return result;
}

function sendSettingsToWatch(raw) {
  var settings = convertSettings(raw);
  Pebble.sendAppMessage(settings, function() {
    console.log('settings sent to watch');
  }, function(error) {
    console.log('sendAppMessage failed: ' + error);
  });
}

function trySendSettings() {
  if (watchReady) {
    var saved = localStorage.getItem(SETTINGS_STORAGE_KEY);
    if (saved) {
      var raw;
      try {
        raw = JSON.parse(saved);
      } catch(e) {
        console.log('failed to parse saved settings: ' + e);
        return;
      }
      console.log('sending saved settings to watch');
      sendSettingsToWatch(raw);
    } else {
      console.log('no saved settings found');
    }
  }
}

Pebble.addEventListener('ready', function(e) {
  console.log('PebbleKit JS ready!');
  clay.meta = {
    activeWatchInfo: Pebble.getActiveWatchInfo ? Pebble.getActiveWatchInfo() : null,
    accountToken: Pebble.getAccountToken(),
    watchToken: Pebble.getWatchToken(),
    userData: { themes: themes }
  };

  if (watchReady) {
    trySendSettings();
  }

  setTimeout(function() {
    if (!watchReady) {
      console.log('handshake timeout, sending settings');
      watchReady = true;
      trySendSettings();
    }
  }, 3000);
});

Pebble.addEventListener('appmessage', function(e) {
  if (e && e.payload && e.payload[HANDSHAKE_KEY] !== undefined) {
    console.log('watch ready, sending settings');
    watchReady = true;
    trySendSettings();
  }
});

Pebble.addEventListener('showConfiguration', function() {
  Pebble.openURL(clay.generateUrl());
});

Pebble.addEventListener('webviewclosed', function(e) {
  if (e && e.response) {
    var raw = clay.getSettings(e.response, false);
    localStorage.setItem(SETTINGS_STORAGE_KEY, JSON.stringify(raw));
    sendSettingsToWatch(raw);
  }
});
