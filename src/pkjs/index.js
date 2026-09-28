/*
 * 91 Dub CGM – phone side
 * - Settings from the GitHub Pages config page → watch (message keys by name,
 *   same value format as the former Clay page of 91 Dub v5 plus)
 * - Nightscout /pebble fetch with the synced schedule of casiocgm
 *   (learned upload lag, probe, watchdog request from the watch)
 * - Weather from Open-Meteo (no API key), every 30 min
 */
var messageKeys = require('message_keys');

var CONFIG_URL = 'https://sgitaize.github.io/dubv4cgm/config/';
var SETTINGS_STORAGE_KEY = 'dubv4cgm_settings';
var LAG_KEY = 'dubv4cgm_upload_lag';
var HANDSHAKE_KEY = 9999;

// Keys handled here, not passed through 1:1
var PHONE_ONLY = { CgmHigh: 1, CgmLow: 1, CgmStaleMin: 1 };

var BG_STATUS = { OK: 0, NO_DATA: 1, NO_CONN: 2, OLD: 3, NO_URL: 4 };

var settings = {};
var fetchTimer = null;
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

// ── AppMessage queue: one message in flight, retry on NACK ────────────────
var outbox = [];
var sending = false;

function pump() {
  if (sending || outbox.length === 0) return;
  sending = true;
  var item = outbox[0];
  Pebble.sendAppMessage(item.msg, function() {
    outbox.shift();
    sending = false;
    pump();
  }, function() {
    item.tries++;
    if (item.tries >= 3) {
      outbox.shift();
      console.log('[dubv4cgm] ' + item.label + ' send failed');
    }
    sending = false;
    setTimeout(pump, 1000);
  });
}

function sendMsg(msg, label) {
  outbox.push({ msg: msg, label: label, tries: 0 });
  pump();
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
  sendMsg(msg, 'settings');
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
function parseSgvMgdl(raw) {
  var f = parseFloat(raw);
  if (!isFinite(f) || f <= 0) return NaN;
  return f < 40 ? Math.round(f * 18) : Math.round(f);
}

function parseDeltaMgdl(raw) {
  var f = parseFloat(raw);
  if (!isFinite(f)) return NaN;
  return (Math.abs(f) < 30 && String(raw).indexOf('.') >= 0) ? f * 18 : f;
}

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
  sendMsg(msg, 'BG status ' + status);
}

// Synced schedule (casiocgm v2.3.5): next fetch at reading ts + sensor
// interval + learned upload lag; probe at half the lag once per reading.
var lagSec = Math.max(0, parseInt(localStorage.getItem(LAG_KEY), 10) || 0);
var lastSeenTsSec = 0, prevFetchNow = 0, prevMissed = false;

function learnUploadLag(bgTsSec, serverNowSec, sensorSec) {
  if (lastSeenTsSec && bgTsSec > lastSeenTsSec) {
    var sample = Math.max(0, serverNowSec - bgTsSec);
    var lower = prevMissed ? prevFetchNow - bgTsSec : -1;
    if (prevMissed && serverNowSec - prevFetchNow <= 20) {
      lagSec = sample > lagSec ? sample : Math.round((lagSec + sample) / 2);
    } else if (lower > lagSec) {
      lagSec = Math.round((lower + sample) / 2);
    } else {
      lagSec = Math.min(lagSec, sample);
    }
    lagSec = Math.min(lagSec, sensorSec);
    try { localStorage.setItem(LAG_KEY, String(lagSec)); } catch (e) {}
  }
  prevMissed = (bgTsSec === lastSeenTsSec);
  lastSeenTsSec = bgTsSec;
  prevFetchNow = serverNowSec;
}

function planNextBGFetch(lastBgTsSec, serverNowSec) {
  if (fetchTimer) { clearTimeout(fetchTimer); fetchTimer = null; }
  var sensorSec = sensorMin() * 60;
  if (!lastBgTsSec) {
    fetchTimer = setTimeout(fetchNightscout, 60000);
    return;
  }
  var nowSec = serverNowSec > 0 ? serverNowSec : Math.floor(Date.now() / 1000);
  learnUploadLag(lastBgTsSec, nowSec, sensorSec);
  var dueSec = lastBgTsSec + sensorSec + Math.max(30, lagSec + 10);
  var probeSec = lastBgTsSec + sensorSec + Math.max(30, Math.round(lagSec / 2));
  var delay = (dueSec - nowSec) * 1000;
  if (dueSec - probeSec >= 30 && probeSec > nowSec + 5) delay = (probeSec - nowSec) * 1000;
  if (delay < 15000) delay = (nowSec - dueSec > 120) ? 60000 : 15000;
  if (delay > sensorSec * 3000) delay = sensorSec * 1000;
  console.log('[dubv4cgm] next BG fetch in ' + Math.round(delay / 1000) + ' s (lag ' + lagSec + ' s)');
  fetchTimer = setTimeout(fetchNightscout, delay);
}

function fetchNightscout() {
  if (fetchTimer) { clearTimeout(fetchTimer); fetchTimer = null; }
  var url = String(setting('NsUrl', '')).trim();
  if (url.length < 4) {
    sendBgStatus(BG_STATUS.NO_URL);
    return;   // no polling without a URL; a new config restarts it
  }
  if (!/^https?:\/\//i.test(url)) url = 'https://' + url;
  url = url.replace(/\/+$/, '');
  var apiUrl = url.indexOf('/pebble') >= 0 ? url : url + '/pebble';
  var token = String(setting('NsToken', '')).trim();
  if (token && apiUrl.indexOf('token=') < 0) {
    apiUrl += (apiUrl.indexOf('?') >= 0 ? '&' : '?') + 'token=' + encodeURIComponent(token);
  }

  var req = new XMLHttpRequest();
  req.open('GET', apiUrl, true);
  req.timeout = 15000;
  // Guard: some phones never call any XHR callback; keep the chain alive
  var done = false;
  var guard = setTimeout(function() {
    if (done) return;
    done = true;
    try { req.abort(); } catch (e) {}
    planNextBGFetch(null);
  }, 25000);
  function finish() {
    if (done) return false;
    done = true;
    clearTimeout(guard);
    return true;
  }
  function fail(status) {
    sendBgStatus(status);
    planNextBGFetch(null);
  }
  req.onload = function() {
    if (!finish()) return;
    if (req.status !== 200) { console.log('[dubv4cgm] HTTP ' + req.status); return fail(BG_STATUS.NO_CONN); }
    try {
      var data = JSON.parse(req.responseText);
      var bg = null, serverNow = 0;
      if (data && Array.isArray(data.bgs) && data.bgs.length > 0) {
        bg = data.bgs[0];
        if (Array.isArray(data.status) && data.status[0]) serverNow = parseInt(data.status[0].now || 0, 10) || 0;
      } else if (Array.isArray(data) && data.length > 0) {
        bg = data[0];
      } else if (data && (data.sgv || data.value || data.glucose)) {
        bg = data;
      }
      if (!bg) return fail(BG_STATUS.NO_DATA);
      var sgv = parseSgvMgdl(bg.sgv || bg.glucose || bg.value) || 0;
      if (sgv <= 0) return fail(BG_STATUS.NO_DATA);
      var bgTs = parseInt(bg.datetime || bg.date || bg.mills || bg.timestamp || 0, 10) || 0;
      if (bgTs > 1000000000000) bgTs = Math.floor(bgTs / 1000);
      if (serverNow > 1000000000000) serverNow = Math.floor(serverNow / 1000);
      var nowSec = serverNow > 0 ? serverNow : Math.floor(Date.now() / 1000);
      if (!bgTs) bgTs = nowSec;
      // Watch works with its own clock: shift the timestamp by the skew
      var tsWatch = bgTs + (Math.floor(Date.now() / 1000) - nowSec);
      var status = (nowSec - bgTs > sensorMin() * 2 * 60) ? BG_STATUS.OLD : BG_STATUS.OK;
      var msg = {};
      msg[messageKeys.CgmValue] = mmol() ? (sgv / 18).toFixed(1) : String(sgv);
      msg[messageKeys.CgmDelta] = formatDelta(parseDeltaMgdl(bg.bgdelta));
      msg[messageKeys.CgmTrend] = trendCode(bg.direction || bg.trend);
      msg[messageKeys.CgmSgv] = sgv;
      msg[messageKeys.CgmTs] = tsWatch;
      sendBgStatus(status, msg);
      planNextBGFetch(bgTs, nowSec);
    } catch (ex) {
      console.log('[dubv4cgm] parse error: ' + ex);
      fail(BG_STATUS.NO_DATA);
    }
  };
  req.onerror = function() { if (finish()) fail(BG_STATUS.NO_CONN); };
  req.ontimeout = function() { if (finish()) fail(BG_STATUS.NO_CONN); };
  req.send();
}

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
        sendMsg(msg, 'weather');
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
  fetchNightscout();
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
    fetchNightscout();
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
