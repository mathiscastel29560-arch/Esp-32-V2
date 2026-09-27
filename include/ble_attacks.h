#ifndef BLE_ATTACKS_H
#define BLE_ATTACKS_H

#include "attack_framework.h"

// ============= BLE SCANNER =============
class BLEScanner : public Attack {
public:
  BLEScanner() : Attack("BLE Scanner"), devicesFound(0), scanTimeout(15000) {}

  bool begin() override;
  bool start() override;
  void update() override;
  bool stop() override;
  bool cleanup() override;

  bool setParameter(const char* key, const char* value) override;
  const char* getParameter(const char* key) override;

private:
  uint16_t devicesFound;
  uint32_t scanTimeout;
};

// ============= BLE DISCONNECT ATTACK =============
class BLEDisconnectAttack : public Attack {
public:
  BLEDisconnectAttack() : Attack("BLE Disconnect"), targetAddress(""),
                          disconnectsAttempted(0), attackDuration(5000) {}

  bool begin() override;
  bool start() override;
  void update() override;
  bool stop() override;

  bool setParameter(const char* key, const char* value) override;
  const char* getParameter(const char* key) override;

private:
  char targetAddress[18];
  uint16_t disconnectsAttempted;
  uint32_t attackDuration;
  uint32_t lastAttemptTime;
};

// ============= BLE ADVERTISEMENT INJECTION =============
class BLEAdvertisementInjection : public Attack {
public:
  BLEAdvertisementInjection() : Attack("BLE Advertisement Injection"),
                                advertisementsCreated(0), injectionDuration(10000) {}

  bool begin() override;
  bool start() override;
  void update() override;
  bool stop() override;

private:
  uint16_t advertisementsCreated;
  uint32_t injectionDuration;
  uint32_t lastAdvertisementTime;
};

// ============= BLE GATT ENUMERATION =============
class BLEGATTEnumeration : public Attack {
public:
  BLEGATTEnumeration() : Attack("BLE GATT Enumeration"),
                         servicesFound(0), characteristicsFound(0),
                         enumerationTimeout(20000) {}

  bool begin() override;
  bool start() override;
  void update() override;
  bool stop() override;

  bool setParameter(const char* key, const char* value) override;

private:
  uint16_t servicesFound;
  uint16_t characteristicsFound;
  uint32_t enumerationTimeout;
};

// ============= BLE PAIRING REPLAY =============
class BLEPairingReplay : public Attack {
public:
  BLEPairingReplay() : Attack("BLE Pairing Replay"),
                       keysReplayed(0), replayDuration(15000) {}

  bool begin() override;
  bool start() override;
  void update() override;
  bool stop() override;

  bool setParameter(const char* key, const char* value) override;

private:
  uint16_t keysReplayed;
  uint32_t replayDuration;
  char targetAddress[18];
};

// ============= BLE SWEEPER =============
class BLESweeper : public Attack {
public:
  BLESweeper() : Attack("BLE Sweeper"), channelsSwept(0),
                 vulnerableDevicesFound(0), sweepTimeout(30000) {}

  bool begin() override;
  bool start() override;
  void update() override;
  bool stop() override;

private:
  uint16_t channelsSwept;
  uint16_t vulnerableDevicesFound;
  uint32_t sweepTimeout;
};

// ============= BLE SNIFFER =============
class BLESniffer : public Attack {
public:
  BLESniffer() : Attack("BLE Sniffer"), packetsSniffed(0),
                 sniffDuration(20000) {}

  bool begin() override;
  bool start() override;
  void update() override;
  bool stop() override;

private:
  uint16_t packetsSniffed;
  uint32_t sniffDuration;
};

#endif // BLE_ATTACKS_H
