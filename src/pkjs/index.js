/*
 * 91 Dub CGM – phone side
 * - Settings from the GitHub Pages config page → watch (message keys by name,
 *   same value format as the former Clay page of 91 Dub v5 plus)
 * - CGM via the shared library pebble-cgm (src/pkjs/cgm.js): Nightscout (default) or
 *   iPump-mini (local iPhone app, wake schedule, optional Nightscout fallback)
 * - Weather from Open-Meteo (no API key), every 30 min
 */
var messageKeys = require('message_keys');

var CONFIG_URL = 'https://sgitaize.github.io/dubv4cgm/config/';
var SETTINGS_STORAGE_KEY = 'dubv4cgm_settings';
var HANDSHAKE_KEY = 9999;

// Keys handled here, not passed through 1:1
var PHONE_ONLY = { CgmHigh: 1, CgmLow: 1, CgmStaleMin: 1, CgmSourceSel: 1, OmpUrl: 1, OmpFallback: 1 };

var BG_STATUS = { OK: 0, NO_DATA: 1, NO_CONN: 2, OLD: 3, NO_APP: 4, NO_URL: 5 };   // = pebble-cgm CgmStatus

var settings = {};
var weatherTimer = null;
var watchReady = false;

function loadSettings() {
  try { settings = JSON.parse(localStorage.getItem(SETTINGS_STORAGE_KEY) || '{}') || {}; }
  catch (e) { settings = {}; }
}

function setting(name, def) {
  var v = settings[name];
  return (v === undefined || v === null || v === '') ? def : v;
}

function isOn(v) { return v === true || v === 1 || v === '1' || v === 'true'; }
function mmol() { return String(setting('CgmUnits', '0')) === '1'; }
function sensorMin() { return Math.max(1, parseInt(setting('CgmInterval', '5'), 10) || 5); }

// ── AppMessage queue (pebble-cgm): one message in flight, ack timeout, retries ──
// A send without callback no longer blocks the queue; same key = newest message wins.
var outbox = require('./cgm').outbox('dubv4cgm');

function sendMsg(msg, label, key) {
  outbox.send(msg, label, { key: key });
}

// ── Settings → watch ─────────────────────────────────────────────────────
function toWatchValue(val) {
  if (typeof val === 'boolean') return val ? 1 : 0;
  if (typeof val === 'string' && /^(0x|#)[0-9a-fA-F]{6}$/.test(val)) return parseInt(val.replace(/^(0x|#)/, ''), 16);
  return val;
}

function thresholdMgdl(val, def) {
  var f = parseFloat(String(val).replace(',', '.'));
  if (!isFinite(f) || f <= 0) return def;
  return mmol() || f < 40 ? Math.round(f * 18) : Math.round(f);
}

function sendSettings() {
  var msg = {};
  Object.keys(settings).forEach(function(name) {
    if (PHONE_ONLY[name] || messageKeys[name] === undefined) return;
    msg[messageKeys[name]] = toWatchValue(settings[name]);
  });
  msg[messageKeys.CgmHigh] = thresholdMgdl(setting('CgmHigh', mmol() ? '10' : '180'), 180);
  msg[messageKeys.CgmLow] = thresholdMgdl(setting('CgmLow', mmol() ? '3.9' : '70'), 70);
  msg[messageKeys.CgmStaleMin] = sensorMin() * 2;
  // SlotMain marks the message as a settings message on the watch
  if (msg[messageKeys.SlotMain] === undefined) msg[messageKeys.SlotMain] = '0';
  sendMsg(msg, 'settings', 'settings');
}

// ── Nightscout ───────────────────────────────────────────────────────────
function trendCode(direction) {
  var dir = String(direction || '').toLowerCase();
  if (dir.indexOf('doubleup') >= 0) return 'U';
  if (dir.indexOf('singleup') >= 0 || dir === 'up') return 'u';
  if (dir.indexOf('fortyfiveup') >= 0) return 'r';
  if (dir.indexOf('fortyfivedown') >= 0) return 'f';
  if (dir.indexOf('singledown') >= 0 || dir === 'down') return 'd';
  if (dir.indexOf('doubledown') >= 0) return 'D';
  return '-';
}

// Auto-detect mmol (< 40) vs mg/dL; returns mg/dL


function formatDelta(deltaMgdl) {
  if (!isFinite(deltaMgdl)) return '';
  var m = mmol();
  var v = m ? Math.round(deltaMgdl / 18 * 10) / 10 : Math.round(deltaMgdl);
  if (v === 0) return m ? '+0.0' : '+0';
  return (v > 0 ? '+' : '-') + (m ? Math.abs(v).toFixed(1) : String(Math.abs(v)));
}

function sendBgStatus(status, extra) {
  var msg = extra || {};
  msg[messageKeys.CgmStatus] = status;
  sendMsg(msg, 'BG status ' + status, 'bg');
}

// ── CGM data source: shared library pebble-cgm ─────────────────────────────
function nsUrl() {
  var url = String(setting('NsUrl', '')).trim();
  if (url.length >= 4 && !/^https?:\/\//i.test(url)) url = 'https://' + url;
  return url;
}

var cgm = require('./cgm')({
  name: 'dubv4cgm',
  settings: function() {
    return {
      source: String(setting('CgmSourceSel', 'nightscout')) === 'ipumpmini' ? 'ipumpmini' : 'nightscout',
      nsUrl: nsUrl(), nsToken: String(setting('NsToken', '')).trim(),
      ompUrl: String(setting('OmpUrl', 'http://127.0.0.1:17580')).trim(), ompFallback: isOn(setting('OmpFallback', '0')),
      sensorMin: sensorMin(), sync: true, manualMin: sensorMin()
    };
  },
  onReading: function(r) {
    var msg = {};
    msg[messageKeys.CgmValue] = mmol() ? (r.sgvMgdl / 18).toFixed(1) : String(r.sgvMgdl);
    msg[messageKeys.CgmDelta] = formatDelta(r.deltaMgdl);
    msg[messageKeys.CgmTrend] = trendCode(r.direction);
    msg[messageKeys.CgmSgv] = r.sgvMgdl;
    // Watch works with its own clock: shift the timestamp by the skew
    msg[messageKeys.CgmTs] = r.tsSec + (Math.floor(Date.now() / 1000) - r.nowSec);
    msg[messageKeys.CgmSource] = r.fallback ? 1 : 0;
    sendBgStatus(r.stale ? BG_STATUS.OLD : BG_STATUS.OK, msg);
  },
  onStatus: function(code) { sendBgStatus(BG_STATUS[code] !== undefined ? BG_STATUS[code] : BG_STATUS.NO_DATA); },
  onWakeSchedule: function(next, interval) {
    var m = {};
    m[messageKeys.CgmWakeNext] = next;
    m[messageKeys.CgmWakeInterval] = interval;
    sendMsg(m, 'wake schedule', 'wake');
  }
});

// ── Weather (Open-Meteo) ─────────────────────────────────────────────────
function fetchWeather() {
  navigator.geolocation.getCurrentPosition(function(pos) {
    var unit = String(setting('WeatherUnits', '0')) === '1' ? 'fahrenheit' : 'celsius';
    var url = 'https://api.open-meteo.com/v1/forecast?latitude=' + pos.coords.latitude.toFixed(3) +
              '&longitude=' + pos.coords.longitude.toFixed(3) +
              '&current_weather=true&temperature_unit=' + unit;
    var req = new XMLHttpRequest();
    req.open('GET', url, true);
    req.timeout = 15000;
    req.onload = function() {
      if (req.status !== 200) return;
      try {
        var cw = JSON.parse(req.responseText).current_weather;
        var msg = {};
        msg[messageKeys.WeatherTemp] = Math.round(cw.temperature);
        msg[messageKeys.WeatherCode] = parseInt(cw.weathercode, 10) || 0;
        sendMsg(msg, 'weather', 'weather');
      } catch (e) { console.log('[dubv4cgm] weather parse error: ' + e); }
    };
    req.send();
  }, function(err) {
    console.log('[dubv4cgm] location error: ' + err.message);
  }, { timeout: 30000, maximumAge: 15 * 60 * 1000 });
}

function start() {
  if (!watchReady) return;
  sendSettings();
  cgm.start();
  fetchWeather();
  if (weatherTimer) clearInterval(weatherTimer);
  weatherTimer = setInterval(fetchWeather, 30 * 60 * 1000);
}

// ── Pebble events ────────────────────────────────────────────────────────
Pebble.addEventListener('ready', function() {
  loadSettings();
  // Older watch firmware may miss the handshake: start anyway after 3 s
  setTimeout(function() {
    if (!watchReady) { watchReady = true; start(); }
  }, 3000);
});

Pebble.addEventListener('appmessage', function(e) {
  var p = (e && e.payload) || {};
  if (p[HANDSHAKE_KEY] !== undefined || p['' + HANDSHAKE_KEY] !== undefined) {
    if (!watchReady) { watchReady = true; start(); }
  }
  if (p[messageKeys.RequestBg] !== undefined || p.RequestBg !== undefined) {
    cgm.fetchNow();
  }
});

Pebble.addEventListener('showConfiguration', function() {
  // Settings travel in the URL fragment: the token never reaches the server
  var stored = localStorage.getItem(SETTINGS_STORAGE_KEY) || '{}';
  Pebble.openURL(CONFIG_URL + '#config=' + encodeURIComponent(stored));
});

Pebble.addEventListener('webviewclosed', function(e) {
  if (!e || !e.response || e.response === 'CANCELLED') return;
  try {
    var raw = e.response;
    settings = JSON.parse(raw.charAt(0) === '{' ? raw : decodeURIComponent(raw));
    localStorage.setItem(SETTINGS_STORAGE_KEY, JSON.stringify(settings));
    watchReady = true;
    start();
  } catch (ex) {
    console.log('[dubv4cgm] config parse error: ' + ex);
  }
});
