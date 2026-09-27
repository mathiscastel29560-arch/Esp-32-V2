#ifndef NFC_ATTACKS_H
#define NFC_ATTACKS_H

#include "attack_framework.h"

// ============= NFC TAG READER =============
class NFCTagReader : public Attack {
public:
  NFCTagReader() : Attack("NFC Tag Reader"), tagsFound(0),
                   readTimeout(15000) {}

  bool begin() override;
  bool start() override;
  void update() override;
  bool stop() override;
  bool cleanup() override;

private:
  uint16_t tagsFound;
  uint32_t readTimeout;
};

// ============= MIFARE CLONER =============
class MIFARECloner : public Attack {
public:
  MIFARECloner() : Attack("MIFARE Cloner"), blocksCloned(0),
                   cloneTimeout(10000) {}

  bool begin() override;
  bool start() override;
  void update() override;
  bool stop() override;

  bool setParameter(const char* key, const char* value) override;

private:
  uint16_t blocksCloned;
  uint32_t cloneTimeout;
  char targetUID[16];
};

// ============= NFC EMULATOR =============
class NFCEmulator : public Attack {
public:
  NFCEmulator() : Attack("NFC Emulator"), interactionsCount(0),
                  emulateDuration(20000) {}

  bool begin() override;
  bool start() override;
  void update() override;
  bool stop() override;

private:
  uint16_t interactionsCount;
  uint32_t emulateDuration;
};

// ============= RFID CLONE DETECTOR =============
class RFIDCloneDetector : public Attack {
public:
  RFIDCloneDetector() : Attack("RFID Clone Detector"),
                        cardsScanned(0), clonesDetected(0),
                        detectionTimeout(25000) {}

  bool begin() override;
  bool start() override;
  void update() override;
  bool stop() override;

private:
  uint16_t cardsScanned;
  uint16_t clonesDetected;
  uint32_t detectionTimeout;
};

// ============= ZIGBEE SNIFFER =============
class ZigbeeSniffer : public Attack {
public:
  ZigbeeSniffer() : Attack("Zigbee Sniffer"), packetsSniffed(0),
                    sniffDuration(20000) {}

  bool begin() override;
  bool start() override;
  void update() override;
  bool stop() override;

private:
  uint16_t packetsSniffed;
  uint32_t sniffDuration;
};

// ============= MQTT INTERCEPTOR =============
class MQTTInterceptor : public Attack {
public:
  MQTTInterceptor() : Attack("MQTT Interceptor"), messagesIntercepted(0),
                      interceptTimeout(15000) {}

  bool begin() override;
  bool start() override;
  void update() override;
  bool stop() override;

  bool setParameter(const char* key, const char* value) override;

private:
  uint16_t messagesIntercepted;
  uint32_t interceptTimeout;
  char broker[64];
};

// ============= SMART HOME SCANNER =============
class SmartHomeScanner : public Attack {
public:
  SmartHomeScanner() : Attack("Smart Home Scanner"),
                       devicesFound(0), scanTimeout(30000) {}

  bool begin() override;
  bool start() override;
  void update() override;
  bool stop() override;

private:
  uint16_t devicesFound;
  uint32_t scanTimeout;
};

#endif // NFC_ATTACKS_H
