#pragma once

#include <Arduino.h>

// Edit this file, then upload the sketch over USB.
// The phone must be on the same LAN as this Wi-Fi (not a guest network).
// NodeMCU radios are 2.4 GHz only. A normal dual-band home router is fine.

#define WIFI_SSID "JioFiber-YnKKG"
#define WIFI_PASSWORD "sharad41980"

// Used when the Wi-Fi above is missing or the name is still YOUR_WIFI_NAME.
// Join this hotspot and open http://192.168.4.1
// Use at least 8 characters, or "" for an open hotspot.
#define AP_SSID "HomeInvasion"
#define AP_PASSWORD "home12345"

// Optional sign-in for the whole page. Leave the password empty for none.
#define WEB_USER "admin"
#define WEB_PASSWORD ""

// Optional password for firmware upgrade only. Leave empty for none.
// Set one before this board stays on a shared network.
#define OTA_PASSWORD ""

// Change this before every upgrade. The page shows it after reboot.
#define FW_VERSION "1.0.0"

#define DEVICE_NAME "HomeInvasion"

// Phone URL: http://homeinvasion.local  (Windows often needs the IP instead)
#define HOSTNAME "homeinvasion"

// Most 4-channel relay boards turn the relay ON when the pin is LOW.
// If the page says ON while the relay is off, set this to 0 and upload again.
#define RELAY_ACTIVE_LOW 1

#define EEPROM_MAGIC 0xA5

// Silkscreen label -> GPIO. D3, D4, and D8 are boot pins; do not use them.
//   Relay 1  D1  GPIO5
//   Relay 2  D2  GPIO4
//   Relay 3  D5  GPIO14
//   Relay 4  D6  GPIO12
static const uint8_t RELAY_PINS[4] = {5, 4, 14, 12};
static const char *const RELAY_NAMES[4] = {"Relay 1", "Relay 2", "Relay 3", "Relay 4"};
static const char *const RELAY_PIN_LABELS[4] = {"D1", "D2", "D5", "D6"};
