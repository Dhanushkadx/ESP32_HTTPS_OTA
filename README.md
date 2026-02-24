# ESP32 HTTPS OTA Update (Secure) — Arduino Example

This project demonstrates how to perform **secure OTA firmware updates** on an **ESP32** using **HTTPS (TLS)** with `HTTPUpdate`, including:

- NTP time sync (required for X.509 certificate validation)
- A pinned Root CA certificate (`ISRG Root X1` / Let’s Encrypt chain example)
- Optional HTTP Basic Authentication for the firmware download endpoint

> ✅ Target: ESP32 (Arduino framework)

---

## How it Works

1. ESP32 connects to Wi-Fi using `WiFiMulti`.
2. Device syncs time via NTP (`pool.ntp.org`, `time.nist.gov`) so TLS certificate validation works.
3. Creates a `WiFiClientSecure` and loads a Root CA certificate.
4. Calls `httpUpdate.update()` to download `file.bin` over HTTPS and flash it.

---

## Requirements

### Hardware
- Any ESP32 dev board (ESP32-WROOM, ESP32-S3, etc.)

### Software
- Arduino IDE (or PlatformIO)
- ESP32 Arduino core installed

### Libraries (usually included with ESP32 core)
- `WiFi.h`
- `WiFiMulti.h`
- `HTTPClient.h`
- `HTTPUpdate.h`

---

## Project Files

- `httpUpdateSecure.ino` — main sketch (secure OTA update example)

---

## Configuration

Update these placeholders in the code:

### 1) Wi-Fi credentials
```cpp
```
WiFiMulti.addAP("SSID", "PASSWORD");

httpUpdate.update(client, "https://server/file.bin", "", [](HTTPClient *client) {
  client->setAuthorization("test", "password");
});

client.setCACert(rootCACertificate);

```
