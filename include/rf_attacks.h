#ifndef RF_ATTACKS_H
#define RF_ATTACKS_H

#include "attack_framework.h"

// ============= NRF24 SCANNER (2.4GHz) =============
class NRF24Scanner : public Attack {
public:
  NRF24Scanner() : Attack("NRF24 Scanner"), channelsScanned(0),
                   devicesDetected(0), scanTimeout(10000) {}

  bool begin() override;
  bool start() override;
  void update() override;
  bool stop() override;

private:
  uint16_t channelsScanned;
  uint16_t devicesDetected;
  uint32_t scanTimeout;
};

// ============= NRF24 JAMMER (2.4GHz) =============
class NRF24Jammer : public Attack {
public:
  NRF24Jammer() : Attack("NRF24 Jammer"), packetsTransmitted(0),
                  jamDuration(5000), transmitPower(0) {}

  bool begin() override;
  bool start() override;
  void update() override;
  bool stop() override;

private:
  uint16_t packetsTransmitted;
  uint32_t jamDuration;
  uint8_t transmitPower;
};

// ============= CC1101 SCANNER (433MHz SubGHz) =============
class CC1101Scanner : public Attack {
public:
  CC1101Scanner() : Attack("CC1101 Scanner"), frequenciesScanned(0),
                    signalDetected(0), scanTimeout(15000) {}

  bool begin() override;
  bool start() override;
  void update() override;
  bool stop() override;

  bool setParameter(const char* key, const char* value) override;

private:
  uint16_t frequenciesScanned;
  uint16_t signalDetected;
  uint32_t scanTimeout;
};

// ============= CC1101 TRANSMITTER (433MHz) =============
class CC1101Transmitter : public Attack {
public:
  CC1101Transmitter() : Attack("CC1101 Transmitter"), messagesToSend(0),
                        transmitDuration(8000), frequency(433920000) {}

  bool begin() override;
  bool start() override;
  void update() override;
  bool stop() override;

  bool setParameter(const char* key, const char* value) override;

private:
  uint16_t messagesToSend;
  uint32_t transmitDuration;
  uint32_t frequency;
  uint32_t lastTransmitTime;
};

// ============= DRONE PROTOCOL ANALYZER =============
class DroneProtocolAnalyzer : public Attack {
public:
  DroneProtocolAnalyzer() : Attack("Drone Protocol Analyzer"),
                            commandsCaptured(0), analysisTimeout(20000) {}

  bool begin() override;
  bool start() override;
  void update() override;
  bool stop() override;

private:
  uint16_t commandsCaptured;
  uint32_t analysisTimeout;
};

// ============= IR (INFRARED) SPOOFER =============
class IRSpoofer : public Attack {
public:
  IRSpoofer() : Attack("IR Spoofer"), commandsTransmitted(0),
                spoofDuration(10000) {}

  bool begin() override;
  bool start() override;
  void update() override;
  bool stop() override;

  bool setParameter(const char* key, const char* value) override;

private:
  uint16_t commandsTransmitted;
  uint32_t spoofDuration;
  char deviceType[32];
};

// ============= LORA SNIFFER (868MHz) =============
class LoRaSniffer : public Attack {
public:
  LoRaSniffer() : Attack("LoRa Sniffer"), packetsIntercepted(0),
                  sniffDuration(25000) {}

  bool begin() override;
  bool start() override;
  void update() override;
  bool stop() override;

private:
  uint16_t packetsIntercepted;
  uint32_t sniffDuration;
};

// ============= ISM BAND SWEEPER =============
class ISMBandSweeper : public Attack {
public:
  ISMBandSweeper() : Attack("ISM Band Sweeper"), freqPointsScanned(0),
                     anomaliesDetected(0), sweepTimeout(30000) {}

  bool begin() override;
  bool start() override;
  void update() override;
  bool stop() override;

private:
  uint16_t freqPointsScanned;
  uint16_t anomaliesDetected;
  uint32_t sweepTimeout;
};

#endif // RF_ATTACKS_H
