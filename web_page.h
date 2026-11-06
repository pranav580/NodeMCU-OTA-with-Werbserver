#pragma once

#include <Arduino.h>

// Served from flash. The phone does not need internet, only the same LAN.
static const char INDEX_HTML[] PROGMEM = R"WEB(<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<meta name="theme-color" content="#101513">
<meta name="robots" content="noindex">
<meta name="mobile-web-app-capable" content="yes">
<meta name="apple-mobile-web-app-capable" content="yes">
<meta name="apple-mobile-web-app-title" content="HomeInvasion">
<title>HomeInvasion</title>
<style>
:root {
  --bg: #101513;
  --card: #1a2420;
  --card-2: #121a17;
  --line: #2c4038;
  --text: #e8f2ec;
  --muted: #93a79c;
  --green: #2ee59d;
  --amber: #ffc857;
  --red: #ff8d8d;
  --shadow: 0 12px 32px rgba(0, 0, 0, .28);
}
* { box-sizing: border-box; }
[hidden] { display: none !important; }
html, body { margin: 0; min-height: 100%; }
body {
  color: var(--text);
  font: 16px/1.45 "Segoe UI", system-ui, sans-serif;
  background:
    radial-gradient(900px 420px at 0% -10%, #1b3b2e 0%, transparent 55%),
    radial-gradient(700px 380px at 100% 0%, #2c2616 0%, transparent 46%),
    var(--bg);
  -webkit-tap-highlight-color: transparent;
}
.wrap { max-width: 520px; margin: 0 auto; padding: 22px 16px calc(28px + env(safe-area-inset-bottom)); }
header { display: flex; justify-content: space-between; gap: 12px; align-items: flex-start; }
h1 { margin: 0 0 4px; font-size: 1.6rem; letter-spacing: -0.03em; }
.sub, .note, .foot, .meta, .steps { color: var(--muted); }
.sub { font-size: .95rem; }
.pill {
  flex: 0 0 auto;
  min-width: 108px;
  text-align: right;
  border: 1px solid var(--line);
  background: rgba(0, 0, 0, .22);
  border-radius: 16px;
  padding: 8px 10px;
}
.line { display: flex; justify-content: flex-end; align-items: center; gap: 6px; color: var(--muted); font-size: .78rem; }
.pill strong { display: block; margin-top: 2px; font-size: .95rem; }
.dot { width: 8px; height: 8px; border-radius: 50%; background: #66786f; }
.dot.on { background: var(--green); animation: pulse 2.2s infinite; }
@keyframes pulse {
  0% { box-shadow: 0 0 0 0 rgba(46, 229, 157, .65); }
  70% { box-shadow: 0 0 0 8px rgba(46, 229, 157, 0); }
  100% { box-shadow: 0 0 0 0 rgba(46, 229, 157, 0); }
}
.message { display: none; margin: 14px 0 0; padding: 12px; border-radius: 14px; border: 1px solid var(--line); background: #13241c; }
.message.show { display: block; }
.message.bad { background: #2a1616; color: #ffd0d0; border-color: #5c3030; }
.message.good { background: #10281c; color: #c8ffe8; border-color: #1f6b49; }
section { margin-top: 22px; }
.section-title { display: flex; justify-content: space-between; align-items: baseline; margin-bottom: 10px; }
h2 { margin: 0; font-size: .78rem; letter-spacing: .08em; text-transform: uppercase; color: var(--muted); }
#relayCount { color: var(--muted); font-size: .85rem; }
.grid { display: grid; grid-template-columns: 1fr 1fr; gap: 12px; }
.card {
  background: linear-gradient(180deg, var(--card), var(--card-2));
  border: 1px solid var(--line);
  border-radius: 18px;
  padding: 14px;
  box-shadow: var(--shadow);
}
.card.live { border-color: #1f6b49; }
.row { display: flex; align-items: center; gap: 10px; margin-bottom: 12px; }
.mark {
  width: 28px; height: 28px; border-radius: 50%;
  display: flex; align-items: center; justify-content: center; flex: 0 0 auto;
  background: #10281c; color: var(--green); font-size: .85rem; font-weight: 700;
}
.card.live .mark { background: var(--green); color: #06281a; }
.name h3 { margin: 0; font-size: 1.02rem; }
.pin { font-size: .78rem; }
.btns { display: grid; grid-template-columns: 1fr 1fr; gap: 8px; }
button, input { font: inherit; }
button {
  appearance: none;
  border: 1px solid var(--line);
  background: #0e1512;
  color: var(--text);
  border-radius: 12px;
  min-height: 46px;
  padding: 10px 8px;
  font-weight: 700;
  cursor: pointer;
  user-select: none;
}
button:active { transform: translateY(1px); }
button:disabled { opacity: .45; cursor: default; transform: none; }
button.on-active { background: var(--green); color: #06281a; border-color: transparent; }
button.off-active { background: #31423b; color: var(--text); border-color: transparent; }
button.ghost, button.primary { width: 100%; margin-top: 12px; }
button.ghost { background: transparent; }
button.primary {
  position: relative;
  overflow: hidden;
  isolation: isolate;
  background: var(--amber);
  color: #2a2108;
  border-color: transparent;
}
button.primary.progressing { opacity: 1; background: #c99410; color: #fff8e4; }
button.primary .fill {
  position: absolute;
  left: 0; top: 0; bottom: 0;
  width: 0;
  background: var(--green);
  z-index: 0;
}
button.primary .label { position: relative; z-index: 1; }
button:focus-visible, input:focus-visible { outline: 2px solid var(--green); outline-offset: 2px; }
.panel { padding: 16px; }
.pw { display: block; margin-top: 12px; color: var(--muted); font-size: .85rem; }
.pw input { width: 100%; margin-top: 6px; color: var(--text); }
.pw input {
  background: #0e1512;
  border: 1px solid var(--line);
  border-radius: 12px;
  padding: 12px;
}
.sr-only {
  position: absolute;
  width: 1px; height: 1px;
  padding: 0; margin: -1px;
  overflow: hidden;
  clip: rect(0, 0, 0, 0);
  white-space: nowrap;
  border: 0;
}
.meta { min-height: 1.2em; margin: 8px 0 0; font-size: .85rem; word-break: break-word; }
.meta.err { color: var(--red); }
.steps { margin: 10px 0 0; padding-left: 18px; font-size: .82rem; }
.steps li { margin: 4px 0; }
.note, .foot { font-size: .8rem; }
.foot { margin-top: 18px; }
</style>
</head>
<body>
<main class="wrap">
  <header>
    <div>
      <h1 id="deviceName">HomeInvasion</h1>
      <div class="sub">Relay control on this NodeMCU</div>
    </div>
    <div class="pill">
      <div class="line"><span id="statusDot" class="dot"></span><span id="modeText">Offline</span></div>
      <strong id="ipText">No IP yet</strong>
    </div>
  </header>
  <div id="message" class="message" role="status"></div>

  <section>
    <div class="section-title">
      <h2>Relays</h2>
      <span id="relayCount"></span>
    </div>
    <p id="loading" class="note">Connecting to the board...</p>
    <div class="grid" id="grid"></div>
    <button type="button" class="ghost" id="allOff" disabled>All off</button>
    <p class="note">Pins D1, D2, D5, and D6. If ON and OFF are backwards, set RELAY_ACTIVE_LOW to 0 in config.h and upload again.</p>
  </section>

  <section>
    <div class="section-title"><h2>Firmware upgrade</h2></div>
    <div class="card panel">
      <ol class="steps">
        <li>Build a .bin with the same Wi-Fi. Raise FW_VERSION first. Use the app image, not a 4 MB flash dump.</li>
        <li>Tap Firmware upgrade, choose that .bin, then confirm. Leave the board powered.</li>
      </ol>
      <label class="pw" id="pwWrap" hidden>
        Upgrade password
        <input id="otaPassword" type="password" autocomplete="current-password" placeholder="From config.h">
      </label>
      <input id="binFile" class="sr-only" type="file" accept=".bin,application/octet-stream" tabindex="-1">
      <button type="button" class="primary" id="startBtn" aria-valuemin="0" aria-valuemax="100">
        <span id="startFill" class="fill"></span>
        <span id="startLabel" class="label">Firmware upgrade</span>
      </button>
      <div id="fileMeta" class="meta">Tap the button to choose a .bin and start the upgrade.</div>
    </div>
  </section>
  <p id="foot" class="foot"></p>
</main>
<script>
"use strict";
var imageFile = null;
var relays = [];
var otaLock = false;
var busy = false;
var pollTimer = null;
var seenVersion = "";
var seenBuilt = "";
var upgradeFromVersion = "";
var upgradeFromBuilt = "";
var upgradeConfirmed = false;

function byId(id) { return document.getElementById(id); }

function showMessage(text, kind) {
  var el = byId("message");
  if (!text) {
    el.className = "message";
    el.textContent = "";
    return;
  }
  el.className = "message show" + (kind ? " " + kind : "");
  el.textContent = text;
}

function signalWord(rssi) {
  if (rssi >= -60) return "strong";
  if (rssi >= -75) return "fair";
  return "weak";
}

function formatSize(n) {
  if (n < 1024) return n + " B";
  if (n < 1024 * 1024) return (n / 1024).toFixed(1) + " KB";
  return (n / 1024 / 1024).toFixed(2) + " MB";
}

function updateCount() {
  var n = 0;
  for (var i = 0; i < relays.length; i++) if (relays[i].on) n++;
  byId("relayCount").textContent = n === 0 ? "All off" : (n === 1 ? "1 on" : n + " on");
}

function paintRelay(id, on) {
  if (!relays[id]) return;
  relays[id].on = on;
  var card = byId("grid").children[id];
  if (!card) return;
  card.classList.toggle("live", !!on);
  var onBtn = card.querySelector(".on");
  var offBtn = card.querySelector(".off");
  onBtn.classList.toggle("on-active", !!on);
  offBtn.classList.toggle("off-active", !on);
  onBtn.setAttribute("aria-pressed", on ? "true" : "false");
  offBtn.setAttribute("aria-pressed", on ? "false" : "true");
  updateCount();
}

function renderStatus(data) {
  if (!data || !data.relays || data.relays.length !== 4) throw new Error("Bad status from board");
  relays = data.relays;
  otaLock = !!data.otaLock;
  seenVersion = data.version || "";
  seenBuilt = data.built || "";
  document.title = data.device || "HomeInvasion";
  byId("deviceName").textContent = data.device || "HomeInvasion";
  byId("ipText").textContent = data.ip || "No IP";
  byId("pwWrap").hidden = !otaLock;
  if (data.mode === "wifi") {
    byId("modeText").textContent = "Wi-Fi, " + signalWord(data.rssi) + " signal";
    byId("statusDot").className = "dot on";
  } else if (data.mode === "hotspot") {
    byId("modeText").textContent = "Setup hotspot";
    byId("statusDot").className = "dot on";
  } else {
    byId("modeText").textContent = "Offline";
    byId("statusDot").className = "dot";
  }
  var grid = byId("grid");
  var existing = grid.children.length === 4;
  for (var i = 0; i < 4; i++) {
    var relay = data.relays[i];
    var card = existing ? grid.children[i] : null;
    if (!card) {
      card = document.createElement("article");
      card.className = "card";
      card.innerHTML = '<div class="row"><div class="mark"></div><div class="name"><h3></h3><div class="pin"></div></div></div><div class="btns"><button type="button" class="on">ON</button><button type="button" class="off">OFF</button></div>';
      (function (index) {
        card.querySelector(".on").addEventListener("click", function () { setRelay(index, true); });
        card.querySelector(".off").addEventListener("click", function () { setRelay(index, false); });
      })(i);
      grid.appendChild(card);
    }
    card.querySelector(".mark").textContent = String(i + 1);
    card.querySelector("h3").textContent = relay.name;
    card.querySelector(".pin").textContent = relay.pin;
    card.querySelector(".on").setAttribute("aria-label", "Turn " + relay.name + " on");
    card.querySelector(".off").setAttribute("aria-label", "Turn " + relay.name + " off");
    paintRelay(i, !!relay.on);
  }
  byId("loading").hidden = true;
  byId("allOff").disabled = busy;
  byId("foot").textContent = "Version " + data.version + "  |  Built " + data.built + "  |  Free RAM " + Math.round(data.heap / 1024) + " KB";
}

function showOffline() {
  byId("statusDot").className = "dot";
  byId("modeText").textContent = "Offline";
  if (!byId("grid").children.length) {
    byId("loading").hidden = false;
    byId("loading").textContent = "Cannot reach the board. Use the same Wi-Fi as the NodeMCU, turn off any VPN, then refresh.";
  } else {
    showMessage("Lost contact with the board.", "bad");
  }
}

function refreshStatus() {
  return fetch("/api/status", {cache: "no-store"}).then(function (r) {
    if (!r.ok) throw new Error("Status failed");
    return r.json();
  }).then(renderStatus);
}

function resumePoll() {
  if (pollTimer) clearInterval(pollTimer);
  pollTimer = setInterval(function () {
    if (busy) return;
    refreshStatus().catch(showOffline);
  }, 4000);
}

function setBusy(isBusy) {
  busy = isBusy;
  byId("binFile").disabled = isBusy;
  byId("otaPassword").disabled = isBusy;
  byId("allOff").disabled = isBusy || !byId("grid").children.length;
  var buttons = byId("grid").querySelectorAll("button");
  for (var i = 0; i < buttons.length; i++) buttons[i].disabled = isBusy;
}

function setUpgradeButton(text, pct) {
  byId("startLabel").textContent = text;
  var fill = byId("startFill");
  var btn = byId("startBtn");
  if (pct == null) {
    fill.style.width = "0%";
    btn.classList.remove("progressing");
    btn.removeAttribute("aria-valuenow");
    return;
  }
  btn.classList.add("progressing");
  fill.style.width = Math.max(0, Math.min(100, pct)) + "%";
  btn.setAttribute("aria-valuenow", String(pct));
}

function setRelay(id, on) {
  if (busy || !relays[id]) return;
  var previous = !!relays[id].on;
  setBusy(true);
  paintRelay(id, on);
  showMessage("Switching " + relays[id].name + "...", "");
  var fields = "id=" + encodeURIComponent(id) + "&on=" + (on ? "1" : "0");
  fetch("/api/relay?" + fields, {
    method: "POST",
    headers: {"Content-Type": "application/x-www-form-urlencoded"},
    body: fields,
    cache: "no-store"
  })
    .then(function (r) {
      return r.json().then(function (data) {
        if (!r.ok || !data.ok) throw new Error((data && data.error) || "Could not switch relay");
      });
    })
    .then(function () { return refreshStatus(); })
    .then(function () { showMessage(""); })
    .catch(function (err) {
      paintRelay(id, previous);
      showMessage(err.message || "Could not switch relay", "bad");
    })
    .then(function () { setBusy(false); });
}

function turnAllOff() {
  if (busy) return;
  setBusy(true);
  showMessage("Turning all relays off...", "");
  fetch("/api/alloff", {method: "POST", cache: "no-store"})
    .then(function (r) {
      return r.json().then(function (data) {
        if (!r.ok || !data.ok) throw new Error((data && data.error) || "Could not turn relays off");
      });
    })
    .then(function () { return refreshStatus(); })
    .then(function () { showMessage(""); })
    .catch(function (err) { showMessage(err.message || "Could not turn relays off", "bad"); })
    .then(function () { setBusy(false); });
}

function chooseFile(f) {
  if (!f || busy) return;
  imageFile = f;
  var badExt = !/\.bin$/i.test(f.name);
  var tooBig = f.size > 1024 * 1024;
  byId("fileMeta").className = (badExt || tooBig) ? "meta err" : "meta";
  var text = f.name + " - " + formatSize(f.size);
  if (badExt) {
    byId("fileMeta").textContent = text + " - choose a .bin firmware image";
    imageFile = null;
    byId("binFile").value = "";
    setUpgradeButton("Firmware upgrade", null);
    return;
  }
  if (tooBig) {
    byId("fileMeta").textContent = text + " - larger than the 1019 KB OTA slot";
    imageFile = null;
    byId("binFile").value = "";
    setUpgradeButton("Firmware upgrade", null);
    return;
  }
  byId("fileMeta").textContent = text;
  if (otaLock && !byId("otaPassword").value) {
    showMessage("Enter the upgrade password, then tap Firmware upgrade again.", "bad");
    setUpgradeButton("Start firmware upgrade", null);
    return;
  }
  if (!window.confirm("Start firmware upgrade with " + f.name + " (" + formatSize(f.size) + ")? The board reboots. Relays switch off for a few seconds, then return to the saved state.")) {
    imageFile = null;
    byId("binFile").value = "";
    setUpgradeButton("Firmware upgrade", null);
    byId("fileMeta").textContent = "Tap the button to choose a .bin and start the upgrade.";
    return;
  }
  beginUpgrade();
}

function setProgress(pct, label) {
  setUpgradeButton(label, pct);
  byId("fileMeta").className = "meta";
  byId("fileMeta").textContent = label;
}

function waitForBoard() {
  var tries = 0;
  var fromVersion = upgradeFromVersion;
  var fromBuilt = upgradeFromBuilt;
  var confirmed = upgradeConfirmed;
  var timer = setInterval(function () {
    tries++;
    fetch("/api/status", {cache: "no-store"}).then(function (r) {
      if (!r.ok) throw new Error("down");
      return r.json();
    }).then(function (data) {
      var changed = fromVersion && (data.version !== fromVersion || data.built !== fromBuilt);
      clearInterval(timer);
      renderStatus(data);
      imageFile = null;
      byId("binFile").value = "";
      setBusy(false);
      setProgress(100, "Done 100%");
      setUpgradeButton("Firmware upgrade", null);
      byId("fileMeta").textContent = "Now running version " + data.version + ".";
      if (confirmed || changed) {
        showMessage("Firmware upgrade finished. Now running " + data.version + ".", "good");
      } else {
        showMessage("The board is back on version " + data.version + " (" + data.built + "). It did not confirm the new image. Try the upload again.", "bad");
      }
      resumePoll();
    }).catch(function () {
      if (tries >= 30) {
        clearInterval(timer);
        setBusy(false);
        resumePoll();
        setUpgradeButton("Firmware upgrade", null);
        showMessage("The board did not come back. Check power and Wi-Fi. If needed, flash this project over USB again.", "bad");
      } else {
        setUpgradeButton("Waiting " + tries + "...", 100);
        byId("fileMeta").textContent = "Waiting for the board... " + tries;
      }
    });
  }, 2000);
}

function beginUpgrade() {
  upgradeFromVersion = seenVersion;
  upgradeFromBuilt = seenBuilt;
  upgradeConfirmed = false;
  setBusy(true);
  if (pollTimer) { clearInterval(pollTimer); pollTimer = null; }
  showMessage("Uploading firmware image...", "");
  setProgress(0, "Uploading 0%");
  var sawAll = false;
  var xhr = new XMLHttpRequest();
  xhr.open("POST", "/api/update?password=" + encodeURIComponent(byId("otaPassword").value || ""));
  xhr.timeout = 120000;
  xhr.upload.onprogress = function (e) {
    if (!e.lengthComputable || e.total <= 0) return;
    var pct = Math.round((e.loaded * 100) / e.total);
    if (e.loaded >= e.total) sawAll = true;
    setProgress(Math.min(pct, 99), "Uploading " + pct + "%");
  };
  xhr.onloadend = function () {
    var data = {};
    try { data = JSON.parse(xhr.responseText); } catch (e) { data = {}; }
    if (xhr.status === 200 && data.ok) upgradeConfirmed = true;
    if (upgradeConfirmed || (xhr.status === 0 && sawAll)) {
      setProgress(100, "Installing 100%");
      showMessage(upgradeConfirmed
        ? "Upgrade accepted. Rebooting. This usually takes 15 to 30 seconds."
        : "The connection closed at the end of the upload. Waiting to see if the board reboots.", "");
      setTimeout(waitForBoard, 5000);
      return;
    }
    imageFile = null;
    byId("binFile").value = "";
    setBusy(false);
    resumePoll();
    setUpgradeButton("Firmware upgrade", null);
    var msg = (data && data.error) ? data.error : (xhr.status === 0
      ? "Lost the connection during upload. Stay near the Wi-Fi and try again."
      : "Upgrade failed (" + xhr.status + ")");
    showMessage(msg, "bad");
    byId("fileMeta").textContent = msg;
    byId("fileMeta").className = "meta err";
  };
  var body = new FormData();
  body.append("firmware", imageFile, imageFile.name);
  xhr.send(body);
}

byId("allOff").addEventListener("click", turnAllOff);
byId("binFile").addEventListener("change", function () {
  chooseFile(this.files && this.files[0]);
});
byId("startBtn").addEventListener("click", function () {
  if (busy) return;
  if (imageFile && otaLock && !byId("otaPassword").value) {
    showMessage("Enter the upgrade password, then tap Firmware upgrade again.", "bad");
    return;
  }
  if (imageFile && otaLock && byId("otaPassword").value) {
    if (!window.confirm("Start firmware upgrade with " + imageFile.name + "? The board reboots.")) return;
    beginUpgrade();
    return;
  }
  byId("binFile").value = "";
  byId("binFile").click();
});
window.addEventListener("dragover", function (e) { e.preventDefault(); });
window.addEventListener("drop", function (e) {
  e.preventDefault();
  var dropped = e.dataTransfer && e.dataTransfer.files && e.dataTransfer.files[0];
  if (dropped && !busy) chooseFile(dropped);
});
window.addEventListener("beforeunload", function (e) {
  if (!busy) return;
  e.preventDefault();
  e.returnValue = "";
});

refreshStatus().then(resumePoll).catch(function () {
  showOffline();
  resumePoll();
});
</script>
</body>
</html>
)WEB";
