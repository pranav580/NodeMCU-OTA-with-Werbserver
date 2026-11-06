/*
  HomeInvasion
  NodeMCU page for 4 relays, plus firmware upgrade from a .bin you choose.

  1. Edit config.h (Wi-Fi name and password).
  2. Board: NodeMCU 1.0 (ESP-12E).
     Flash Size: 4MB (FS:2MB OTA:~1019KB). This OTA slot is required.
  3. Upload over USB once. Later images can be sent from the web page.
*/

#ifndef ESP8266
#error Select the board "NodeMCU 1.0 (ESP-12E Module)" and install the ESP8266 board package.
#endif

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <EEPROM.h>
#include <Updater.h>
#include <stdio.h>
#include <string.h>

#include "config.h"
#include "web_page.h"

ESP8266WebServer server(80);

bool relayOn[4] = {false, false, false, false};
bool apMode = false;

bool otaBusy = false;
bool otaStarted = false;
bool otaAccepted = false;
bool otaHasError = false;
bool otaSawMagic = false;
char otaError[120];

void applyRelay(uint8_t index, bool on);
void saveRelays();
void initRelays();
void startAccessPoint();
void connectWifi();
void startMdns();
void startServer();
void printReady();
void blinkReady();
bool ensureSiteAccess();
bool credentialsOk();
bool otaPasswordOk();
bool endsWithBin(const char *name);
void setOtaError(const char *msg);
const char *updateErrorName(uint8_t code);
void currentIp(char *out, size_t len);
void handleRoot();
void handleStatus();
void handleRelay();
void handleAllOff();
void handleUpdateDone();
void handleUpdateUpload();
void handleFavicon();
void handleNotFound();

void applyRelay(uint8_t index, bool on) {
  relayOn[index] = on;
  const uint8_t level = on
                            ? (RELAY_ACTIVE_LOW ? LOW : HIGH)
                            : (RELAY_ACTIVE_LOW ? HIGH : LOW);
  digitalWrite(RELAY_PINS[index], level);
}

void saveRelays() {
  EEPROM.write(0, EEPROM_MAGIC);
  for (uint8_t i = 0; i < 4; i++) EEPROM.write(1 + i, relayOn[i] ? 1 : 0);
  EEPROM.commit();
}

void initRelays() {
  EEPROM.begin(8);
  const bool haveSaved = EEPROM.read(0) == EEPROM_MAGIC;
  for (uint8_t i = 0; i < 4; i++) {
    const bool on = haveSaved && EEPROM.read(1 + i) == 1;
    applyRelay(i, on);
    pinMode(RELAY_PINS[i], OUTPUT);
    Serial.printf("Relay %u (%s) %s\n", i + 1, RELAY_PIN_LABELS[i], on ? "ON" : "OFF");
  }
  if (!haveSaved) saveRelays();
}

void currentIp(char *out, size_t len) {
  const IPAddress a = (WiFi.status() == WL_CONNECTED) ? WiFi.localIP() : WiFi.softAPIP();
  snprintf(out, len, "%u.%u.%u.%u", a[0], a[1], a[2], a[3]);
}

void startAccessPoint() {
  WiFi.disconnect(true);
  WiFi.mode(WIFI_AP);
  WiFi.setSleepMode(WIFI_NONE_SLEEP);
  delay(100);
  WiFi.softAPConfig(IPAddress(192, 168, 4, 1), IPAddress(192, 168, 4, 1), IPAddress(255, 255, 255, 0));
  const bool secured = strlen(AP_PASSWORD) >= 8;
  const bool ok = secured ? WiFi.softAP(AP_SSID, AP_PASSWORD) : WiFi.softAP(AP_SSID);
  apMode = ok;
  if (!ok) Serial.println("Could not start the setup hotspot.");
}

void connectWifi() {
  apMode = false;
  const bool placeholder = (strcmp(WIFI_SSID, "YOUR_WIFI_NAME") == 0) || (WIFI_SSID[0] == '\0');
  if (placeholder) {
    Serial.println("config.h still has YOUR_WIFI_NAME. Starting the setup hotspot.");
    startAccessPoint();
    return;
  }

  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.setSleepMode(WIFI_NONE_SLEEP);
  WiFi.setAutoReconnect(true);
  WiFi.hostname(HOSTNAME);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to ");
  Serial.print(WIFI_SSID);
  const uint32_t start = millis();
  while (WiFi.status() != WL_CONNECTED && (millis() - start) < 20000UL) {
    delay(250);
    Serial.print(".");
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) return;

  Serial.println("Home Wi-Fi did not connect. Starting the setup hotspot.");
  startAccessPoint();
}

void startMdns() {
  if (MDNS.begin(HOSTNAME)) {
    MDNS.addService("http", "tcp", 80);
  } else {
    Serial.println("mDNS did not start. Use the IP address.");
  }
}

bool credentialsOk() {
  if (WEB_PASSWORD[0] == '\0') return true;
  return server.authenticate(WEB_USER, WEB_PASSWORD);
}

bool ensureSiteAccess() {
  if (credentialsOk()) return true;
  server.requestAuthentication(BASIC_AUTH, "HomeInvasion", "Sign in");
  return false;
}

bool otaPasswordOk() {
  if (OTA_PASSWORD[0] == '\0') return true;
  if (!server.hasArg("password")) return false;
  return server.arg("password") == OTA_PASSWORD;
}

bool endsWithBin(const char *name) {
  if (!name) return false;
  const size_t len = strlen(name);
  if (len < 4) return false;
  const char *e = name + len - 4;
  return e[0] == '.' &&
         (e[1] == 'b' || e[1] == 'B') &&
         (e[2] == 'i' || e[2] == 'I') &&
         (e[3] == 'n' || e[3] == 'N');
}

void setOtaError(const char *msg) {
  otaHasError = true;
  snprintf(otaError, sizeof(otaError), "%s", msg);
  Serial.println(otaError);
}

const char *updateErrorName(uint8_t code) {
  switch (code) {
    case UPDATE_ERROR_OK: return "ok";
    case UPDATE_ERROR_WRITE: return "flash write failed";
    case UPDATE_ERROR_ERASE: return "flash erase failed";
    case UPDATE_ERROR_READ: return "flash read failed";
    case UPDATE_ERROR_SPACE: return "not enough flash space";
    case UPDATE_ERROR_SIZE: return "size mismatch";
    case UPDATE_ERROR_STREAM: return "stream error";
    case UPDATE_ERROR_MD5: return "checksum mismatch";
    case UPDATE_ERROR_FLASH_CONFIG: return "flash size does not match this board";
    case UPDATE_ERROR_NEW_FLASH_CONFIG: return "flash size does not match this board";
    case UPDATE_ERROR_MAGIC_BYTE: return "not a valid NodeMCU firmware image";
    default: return "upgrade failed";
  }
}

void handleRoot() {
  if (!ensureSiteAccess()) return;
  server.sendHeader("Cache-Control", "no-store, max-age=0");
  // PSTR and INDEX_HTML both live in flash. send_P must not be given a RAM pointer.
  server.send_P(200, PSTR("text/html; charset=utf-8"), INDEX_HTML);
}

void handleStatus() {
  if (!ensureSiteAccess()) return;

  char ip[16];
  currentIp(ip, sizeof(ip));
  const bool onWifi = WiFi.status() == WL_CONNECTED;
  const char *mode = onWifi ? "wifi" : (apMode ? "hotspot" : "offline");

  char body[768];
  int n = snprintf(
      body, sizeof(body),
      "{\"ok\":true,\"device\":\"%s\",\"version\":\"" FW_VERSION "\",\"built\":\"" __DATE__ " " __TIME__
      "\",\"mode\":\"%s\",\"ip\":\"%s\",\"rssi\":%d,\"heap\":%u,\"otaLock\":%s,\"relays\":[",
      DEVICE_NAME, mode, ip, onWifi ? WiFi.RSSI() : 0, ESP.getFreeHeap(),
      (OTA_PASSWORD[0] != '\0') ? "true" : "false");

  for (uint8_t i = 0; i < 4; i++) {
    if (n < 0 || n >= (int)sizeof(body)) break;
    const int w = snprintf(
        body + n, sizeof(body) - (size_t)n,
        "%s{\"id\":%u,\"name\":\"%s\",\"pin\":\"%s\",\"on\":%s}",
        (i ? "," : ""), i, RELAY_NAMES[i], RELAY_PIN_LABELS[i], relayOn[i] ? "true" : "false");
    if (w < 0) {
      n = (int)sizeof(body);
      break;
    }
    n += w;
  }

  if (n < 0 || n >= (int)sizeof(body) - 3) {
    server.send(500, "application/json", "{\"ok\":false,\"error\":\"status too long\"}");
    return;
  }
  snprintf(body + n, sizeof(body) - (size_t)n, "]}");
  server.sendHeader("Cache-Control", "no-store");
  server.send(200, "application/json", String(body));
}

void handleRelay() {
  if (!ensureSiteAccess()) return;
  if (otaBusy) {
    server.send(409, "application/json", "{\"ok\":false,\"error\":\"Upgrade in progress\"}");
    return;
  }
  if (!server.hasArg("id") || !server.hasArg("on")) {
    server.send(400, "application/json", "{\"ok\":false,\"error\":\"Missing id or on\"}");
    return;
  }

  const int id = server.arg("id").toInt();
  if (id < 0 || id > 3) {
    server.send(400, "application/json", "{\"ok\":false,\"error\":\"Relay must be 0 to 3\"}");
    return;
  }

  String value = server.arg("on");
  value.toLowerCase();
  const bool on = (value == "1" || value == "true" || value == "on");

  if (relayOn[id] != on) {
    applyRelay((uint8_t)id, on);
    saveRelays();
    Serial.printf("Relay %u %s\n", id + 1, on ? "ON" : "OFF");
  }

  char body[64];
  snprintf(body, sizeof(body), "{\"ok\":true,\"id\":%d,\"on\":%s}", id, on ? "true" : "false");
  server.send(200, "application/json", String(body));
}

void handleAllOff() {
  if (!ensureSiteAccess()) return;
  if (otaBusy) {
    server.send(409, "application/json", "{\"ok\":false,\"error\":\"Upgrade in progress\"}");
    return;
  }
  for (uint8_t i = 0; i < 4; i++) applyRelay(i, false);
  saveRelays();
  Serial.println("All relays OFF");
  server.send(200, "application/json", "{\"ok\":true}");
}

void handleFavicon() {
  server.send(204);
}

void handleNotFound() {
  server.send(404, "text/plain", "Not found");
}

void handleUpdateUpload() {
  HTTPUpload &upload = server.upload();

  if (upload.status == UPLOAD_FILE_START) {
    otaBusy = true;
    otaAccepted = false;
    otaHasError = false;
    otaStarted = false;
    otaSawMagic = false;
    otaError[0] = '\0';

    if (!credentialsOk()) {
      setOtaError("Sign-in required");
      return;
    }
    if (!otaPasswordOk()) {
      setOtaError("Wrong upgrade password");
      return;
    }
    if (!endsWithBin(upload.filename.c_str())) {
      setOtaError("Choose a .bin firmware image");
      return;
    }

    const size_t space = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
    Update.clearError();
    // Erase flash while the upload arrives, so Wi-Fi is not stalled first.
    Update.runAsync(true);
    if (!Update.begin(space)) {
      char msg[80];
      snprintf(msg, sizeof(msg), "Not enough flash space: %s", updateErrorName(Update.getError()));
      setOtaError(msg);
      return;
    }

    otaStarted = true;
    Serial.printf("OTA start: %s (%u bytes free)\n", upload.filename.c_str(), (unsigned)space);
    return;
  }

  if (upload.status == UPLOAD_FILE_WRITE) {
    if (otaHasError || !otaStarted || upload.currentSize == 0) return;

    if (!otaSawMagic) {
      otaSawMagic = true;
      if (upload.buf[0] != 0xE9) {
        Update.end(false);
        otaStarted = false;
        setOtaError("This file is not a NodeMCU firmware image");
        return;
      }
    }

    if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
      const uint8_t code = Update.getError();
      Update.end(false);
      otaStarted = false;
      char msg[80];
      snprintf(msg, sizeof(msg), "Flash write failed: %s", updateErrorName(code));
      setOtaError(msg);
      return;
    }

    if ((upload.totalSize % 65536) < upload.currentSize) {
      Serial.printf("OTA wrote %u bytes\n", upload.totalSize);
    }
    yield();
    return;
  }

  if (upload.status == UPLOAD_FILE_END) {
    if (!otaStarted || otaHasError) return;
    if (upload.totalSize < 10000) {
      Update.end(false);
      otaStarted = false;
      setOtaError("File is too small to be firmware");
      return;
    }
    if (!Update.end(true)) {
      const uint8_t code = Update.getError();
      otaStarted = false;
      char msg[96];
      snprintf(msg, sizeof(msg), "Firmware rejected: %s (%u)", updateErrorName(code), code);
      setOtaError(msg);
      return;
    }
    otaAccepted = true;
    otaStarted = false;
    Serial.printf("OTA stored %u bytes. Rebooting.\n", upload.totalSize);
    return;
  }

  if (upload.status == UPLOAD_FILE_ABORTED) {
    if (otaStarted) Update.end(false);
    otaStarted = false;
    setOtaError("Upload aborted");
  }
}

void handleUpdateDone() {
  if (!credentialsOk()) {
    otaBusy = false;
    server.requestAuthentication(BASIC_AUTH, "HomeInvasion", "Sign in");
    return;
  }

  if (otaAccepted && !otaHasError) {
    server.sendHeader("Connection", "close");
    server.send(200, "application/json", "{\"ok\":true}");
    // flush() with no argument does not wait. Give the phone time to read the reply.
    server.client().flush(1000);
    delay(500);
    ESP.restart();
    return;
  }

  char body[180];
  snprintf(body, sizeof(body), "{\"ok\":false,\"error\":\"%s\"}", otaError[0] ? otaError : "Upgrade failed");
  const int code = (strstr(otaError, "password") != nullptr) ? 401 : 400;
  server.send(code, "application/json", String(body));
  otaBusy = false;
}

void startServer() {
  server.on("/", HTTP_GET, handleRoot);
  server.on("/api/status", HTTP_GET, handleStatus);
  server.on("/api/relay", HTTP_POST, handleRelay);
  server.on("/api/alloff", HTTP_POST, handleAllOff);
  server.on("/api/update", HTTP_POST, handleUpdateDone, handleUpdateUpload);
  server.on("/favicon.ico", HTTP_GET, handleFavicon);
  server.onNotFound(handleNotFound);
  server.begin();
}

void printReady() {
  char ip[16];
  currentIp(ip, sizeof(ip));
  Serial.println();
  Serial.println("----------------------------------------");
  Serial.print("HomeInvasion ");
  Serial.println(FW_VERSION);
  const uint32_t slot = ESP.getFreeSketchSpace();
  Serial.printf("OTA slot: %u bytes\n", slot);
  if (slot < 500000) {
    Serial.println("OTA slot is small. Set Flash Size to 4MB (FS:2MB OTA:~1019KB).");
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("Wi-Fi: ");
    Serial.println(WIFI_SSID);
  } else if (apMode) {
    Serial.print("Hotspot: ");
    Serial.println(AP_SSID);
    if (strlen(AP_PASSWORD) >= 8) {
      Serial.print("Hotspot password: ");
      Serial.println(AP_PASSWORD);
    }
  }
  Serial.print("Open http://");
  Serial.println(ip);
  Serial.print("Or http://");
  Serial.print(HOSTNAME);
  Serial.println(".local");
  if (WEB_PASSWORD[0] != '\0') {
    Serial.print("Page sign-in: ");
    Serial.print(WEB_USER);
    Serial.print(" / ");
    Serial.println(WEB_PASSWORD);
  }
  if (OTA_PASSWORD[0] != '\0') {
    Serial.print("Upgrade password: ");
    Serial.println(OTA_PASSWORD);
  }
  Serial.println("----------------------------------------");
}

void blinkReady() {
  pinMode(LED_BUILTIN, OUTPUT);
  for (uint8_t i = 0; i < 3; i++) {
    digitalWrite(LED_BUILTIN, LOW);
    delay(120);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(120);
  }
}

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println();
  Serial.println("HomeInvasion " FW_VERSION);
  initRelays();
  connectWifi();
  startMdns();
  startServer();
  blinkReady();
  printReady();
}

void loop() {
  server.handleClient();
#if ARDUINO_ESP8266_MAJOR >= 3
  MDNS.update();
#endif
  yield();
}
