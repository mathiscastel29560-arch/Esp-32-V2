#ifndef WIFI_ATTACKS_H
#define WIFI_ATTACKS_H

#include "attack_framework.h"

// ============= WIFI NETWORK SCAN =============
class WiFiNetworkScan : public Attack {
public:
  WiFiNetworkScan() : Attack("WiFi Network Scan"), networksFound(0) {}

  bool begin() override;
  bool start() override;
  void update() override;
  bool stop() override;
  bool cleanup() override;

  bool setParameter(const char* key, const char* value) override;
  const char* getParameter(const char* key) override;

private:
  uint16_t networksFound;
  uint32_t scanStartTime;
  uint32_t SCAN_TIMEOUT = 15000; // 15 secondes
};

// ============= WIFI DEAUTH ATTACK =============
class WiFiDeauthAttack : public Attack {
public:
  WiFiDeauthAttack() : Attack("WiFi Deauth"), targetBSSID(""),
                       channel(1), packetsTransmitted(0) {
    memset(targetBSSID, 0, 18);
  }

  bool begin() override;
  bool start() override;
  void update() override;
  bool stop() override;

  bool setParameter(const char* key, const char* value) override;
  const char* getParameter(const char* key) override;

private:
  char targetBSSID[18];
  uint8_t channel;
  uint16_t packetsTransmitted;
  uint32_t PACKET_INTERVAL = 100; // ms entre paquets
};

// ============= WIFI BEACON FLOOD =============
class WiFiBeaconFlood : public Attack {
public:
  WiFiBeaconFlood() : Attack("WiFi Beacon Flood"),
                      beaconsPerSecond(50), transmitPower(20.0f) {}

  bool begin() override;
  bool start() override;
  void update() override;
  bool stop() override;

private:
  uint16_t beaconsPerSecond;
  float transmitPower;
  uint32_t lastBeaconTime;
};

// ============= WIFI PMKID CAPTURE =============
class WiFiPMKIDCapture : public Attack {
public:
  WiFiPMKIDCapture() : Attack("WiFi PMKID Capture"),
                       pmkidsFound(0), captureTimeout(30000) {}

  bool begin() override;
  bool start() override;
  void update() override;
  bool stop() override;

private:
  uint16_t pmkidsFound;
  uint32_t captureTimeout;
};

// ============= WIFI HANDSHAKE CAPTURE =============
class WiFiHandshakeCapture : public Attack {
public:
  WiFiHandshakeCapture() : Attack("WiFi Handshake Capture"),
                           handshakesFound(0), captureTimeout(60000) {}

  bool begin() override;
  bool start() override;
  void update() override;
  bool stop() override;

private:
  uint16_t handshakesFound;
  uint32_t captureTimeout;
};

// ============= WIFI EVIL TWIN =============
class WiFiEvilTwin : public Attack {
public:
  WiFiEvilTwin() : Attack("WiFi Evil Twin"), clientsConnected(0) {
    memset(ssid, 0, 33);
    strncpy(ssid, "FakeNetwork", 32);
  }

  bool begin() override;
  bool start() override;
  void update() override;
  bool stop() override;

  bool setParameter(const char* key, const char* value) override;

private:
  char ssid[33];
  uint16_t clientsConnected;
};

// ============= WIFI JAMMING =============
class WiFiJamming : public Attack {
public:
  WiFiJamming() : Attack("WiFi Jamming"),
                  transmitPower(20.0f), jammingDuration(10000) {}

  bool begin() override;
  bool start() override;
  void update() override;
  bool stop() override;

private:
  float transmitPower;
  uint32_t jammingDuration;
};

#endif // WIFI_ATTACKS_H
