#include "rf_attacks.h"
#include "logging_system.h"

// ============= NRF24 SCANNER IMPLEMENTATION =============

bool NRF24Scanner::begin() {
  Logger::getInstance().info("RF", "Scannérisation NRF24 2.4GHz initialisée");
  return true;
}

bool NRF24Scanner::start() {
  Attack::start();
  channelsScanned = 0;
  devicesDetected = 0;
  Logger::getInstance().logAttackStart(getName(), "Balayage 2.4GHz sur 125 canaux");
  return true;
}

void NRF24Scanner::update() {
  if (!isRunning) return;

  uint32_t elapsed = millis() - startTime;

  // Simulation: balayage de canaux NRF24 (2400-2525 MHz)
  if (elapsed % 80 == 0 && channelsScanned < 125) {
    channelsScanned++;

    if (channelsScanned % 15 == 0) { // Appareils trouvés périodiquement
      devicesDetected++;
      char desc[64];
      snprintf(desc, 63, "Appareil NRF24 canal %u", channelsScanned);
      AttackResult* result = ResultBuilder::createScan(desc, -60 - (channelsScanned % 10));
      addResult(result);
    }
  }

  if (elapsed > scanTimeout || channelsScanned >= 125) {
    setStatus(devicesDetected > 0 ? AttackStatus::SUCCESS : AttackStatus::PARTIAL);
    isRunning = false;
    Logger::getInstance().logAttackEnd(getName(), currentStatus);
  }
}

bool NRF24Scanner::stop() {
  Attack::stop();
  return true;
}

// ============= NRF24 JAMMER IMPLEMENTATION =============

bool NRF24Jammer::begin() {
  Logger::getInstance().info("RF", "Brouillage NRF24 initialisé");
  return true;
}

bool NRF24Jammer::start() {
  Attack::start();
  packetsTransmitted = 0;
  transmitPower = 0; // PA_MIN
  Logger::getInstance().logAttackStart(getName(), "Brouillage 2.4GHz NRF24");
  return true;
}

void NRF24Jammer::update() {
  if (!isRunning) return;

  uint32_t elapsed = millis() - startTime;
  uint32_t now = millis();
  static uint32_t lastTransmit = 0;

  // Transmission de paquets de brouillage tous les 50ms
  if (now - lastTransmit > 50 && packetsTransmitted < 100) {
    packetsTransmitted++;
    lastTransmit = now;

    if (packetsTransmitted % 20 == 0) {
      char desc[64];
      snprintf(desc, 63, "Paquet brouillage #%u", packetsTransmitted);
      AttackResult* result = ResultBuilder::createPacket(
        (uint8_t*)"JAM_PACKET", 10, desc);
      addResult(result);
    }
  }

  if (elapsed > jamDuration || packetsTransmitted >= 100) {
    setStatus(AttackStatus::SUCCESS);
    isRunning = false;
    Logger::getInstance().logAttackEnd(getName(), currentStatus);
  }
}

bool NRF24Jammer::stop() {
  Attack::stop();
  return true;
}

// ============= CC1101 SCANNER IMPLEMENTATION =============

bool CC1101Scanner::begin() {
  Logger::getInstance().info("RF", "Scannérisation CC1101 433MHz initialisée");
  return true;
}

bool CC1101Scanner::start() {
  Attack::start();
  frequenciesScanned = 0;
  signalDetected = 0;
  Logger::getInstance().logAttackStart(getName(), "Balayage SubGHz 433MHz");
  return true;
}

void CC1101Scanner::update() {
  if (!isRunning) return;

  uint32_t elapsed = millis() - startTime;

  // Simulation: balayage des fréquences SubGHz
  if (elapsed % 100 == 0 && frequenciesScanned < 50) {
    frequenciesScanned++;
    float freq = 433.0f + (frequenciesScanned * 0.1f);

    if (frequenciesScanned % 8 == 0) {
      signalDetected++;
      char desc[64];
      snprintf(desc, 63, "Signal %.2f MHz détecté", freq);
      AttackResult* result = ResultBuilder::createScan(desc, -75 + (frequenciesScanned % 15));
      addResult(result);
    }
  }

  if (elapsed > scanTimeout || frequenciesScanned >= 50) {
    setStatus(signalDetected > 0 ? AttackStatus::SUCCESS : AttackStatus::PARTIAL);
    isRunning = false;
    Logger::getInstance().logAttackEnd(getName(), currentStatus);
  }
}

bool CC1101Scanner::stop() {
  Attack::stop();
  return true;
}

bool CC1101Scanner::setParameter(const char* key, const char* value) {
  if (strcmp(key, "timeout") == 0) {
    scanTimeout = atoi(value);
    return true;
  }
  return false;
}

// ============= CC1101 TRANSMITTER IMPLEMENTATION =============

bool CC1101Transmitter::begin() {
  Logger::getInstance().info("RF", "Transmetteur CC1101 initialisé");
  return true;
}

bool CC1101Transmitter::start() {
  Attack::start();
  messagesToSend = 0;
  lastTransmitTime = millis();
  char msg[128];
  snprintf(msg, 127, "Transmission 433.92MHz en cours");
  Logger::getInstance().logAttackStart(getName(), msg);
  return true;
}

void CC1101Transmitter::update() {
  if (!isRunning) return;

  uint32_t now = millis();
  uint32_t elapsed = now - startTime;

  // Transmission tous les 300ms
  if (now - lastTransmitTime > 300 && messagesToSend < 25) {
    messagesToSend++;
    lastTransmitTime = now;

    if (messagesToSend % 5 == 0) {
      char desc[64];
      snprintf(desc, 63, "Message #%u transmis", messagesToSend);
      uint8_t payload[16];
      for (int i = 0; i < 16; i++) {
        payload[i] = (messagesToSend + i) & 0xFF;
      }
      AttackResult* result = ResultBuilder::createPacket(payload, 16, desc);
      addResult(result);
    }
  }

  if (elapsed > transmitDuration || messagesToSend >= 25) {
    setStatus(AttackStatus::SUCCESS);
    isRunning = false;
    Logger::getInstance().logAttackEnd(getName(), currentStatus);
  }
}

bool CC1101Transmitter::stop() {
  Attack::stop();
  return true;
}

bool CC1101Transmitter::setParameter(const char* key, const char* value) {
  if (strcmp(key, "frequency") == 0) {
    frequency = atoi(value);
    return true;
  }
  if (strcmp(key, "duration") == 0) {
    transmitDuration = atoi(value);
    return true;
  }
  return false;
}

// ============= DRONE PROTOCOL ANALYZER IMPLEMENTATION =============

bool DroneProtocolAnalyzer::begin() {
  Logger::getInstance().info("RF", "Analyseur protocole drone initialisé");
  return true;
}

bool DroneProtocolAnalyzer::start() {
  Attack::start();
  commandsCaptured = 0;
  Logger::getInstance().logAttackStart(getName(), "Analyse protocoles drone 2.4GHz");
  return true;
}

void DroneProtocolAnalyzer::update() {
  if (!isRunning) return;

  uint32_t elapsed = millis() - startTime;

  // Simulation: capture de commandes drone
  if (elapsed < 12000) {
    if (commandsCaptured < 8 && elapsed % 1500 == 0) {
      commandsCaptured++;
      char desc[64];
      snprintf(desc, 63, "Commande drone #%u", commandsCaptured);
      uint8_t cmdData[32];
      for (int i = 0; i < 32; i++) {
        cmdData[i] = (commandsCaptured * 32 + i) & 0xFF;
      }
      AttackResult* result = ResultBuilder::createPacket(cmdData, 32, desc);
      addResult(result);
    }
  }

  if (elapsed > analysisTimeout) {
    setStatus(commandsCaptured > 0 ? AttackStatus::SUCCESS : AttackStatus::FAILED);
    isRunning = false;
    Logger::getInstance().logAttackEnd(getName(), currentStatus);
  }
}

bool DroneProtocolAnalyzer::stop() {
  Attack::stop();
  return true;
}

// ============= IR SPOOFER IMPLEMENTATION =============

bool IRSpoofer::begin() {
  Logger::getInstance().info("IR", "Usurpation IR initialisée");
  return true;
}

bool IRSpoofer::start() {
  Attack::start();
  commandsTransmitted = 0;
  Logger::getInstance().logAttackStart(getName(), "Transmission IR usurpée");
  return true;
}

void IRSpoofer::update() {
  if (!isRunning) return;

  uint32_t elapsed = millis() - startTime;

  // Simulation: transmission de commandes IR
  if (elapsed % 200 == 0 && commandsTransmitted < 50) {
    commandsTransmitted++;

    if (commandsTransmitted % 10 == 0) {
      char desc[64];
      snprintf(desc, 63, "Commande IR #%u envoyée", commandsTransmitted);
      uint8_t irCode[4];
      for (int i = 0; i < 4; i++) {
        irCode[i] = (commandsTransmitted >> (i * 8)) & 0xFF;
      }
      AttackResult* result = ResultBuilder::createPacket(irCode, 4, desc);
      addResult(result);
    }
  }

  if (elapsed > spoofDuration || commandsTransmitted >= 50) {
    setStatus(AttackStatus::SUCCESS);
    isRunning = false;
    Logger::getInstance().logAttackEnd(getName(), currentStatus);
  }
}

bool IRSpoofer::stop() {
  Attack::stop();
  return true;
}

bool IRSpoofer::setParameter(const char* key, const char* value) {
  if (strcmp(key, "device") == 0) {
    strncpy(deviceType, value, 31);
    deviceType[31] = '\0';
    return true;
  }
  return false;
}

// ============= LORA SNIFFER IMPLEMENTATION =============

bool LoRaSniffer::begin() {
  Logger::getInstance().info("RF", "Interception LoRa initialisée");
  return true;
}

bool LoRaSniffer::start() {
  Attack::start();
  packetsIntercepted = 0;
  Logger::getInstance().logAttackStart(getName(), "Interception LoRa 868MHz");
  return true;
}

void LoRaSniffer::update() {
  if (!isRunning) return;

  uint32_t elapsed = millis() - startTime;

  // Simulation: interception de paquets LoRa
  if (elapsed % 250 == 0 && packetsIntercepted < 40) {
    packetsIntercepted++;

    if (packetsIntercepted % 8 == 0) {
      char desc[64];
      snprintf(desc, 63, "Paquet LoRa #%u", packetsIntercepted);
      uint8_t loraData[64];
      for (int i = 0; i < 64; i++) {
        loraData[i] = (packetsIntercepted * 8 + i) & 0xFF;
      }
      AttackResult* result = ResultBuilder::createPacket(loraData, 64, desc);
      addResult(result);
    }
  }

  if (elapsed > sniffDuration || packetsIntercepted >= 40) {
    setStatus(packetsIntercepted > 0 ? AttackStatus::SUCCESS : AttackStatus::FAILED);
    isRunning = false;
    Logger::getInstance().logAttackEnd(getName(), currentStatus);
  }
}

bool LoRaSniffer::stop() {
  Attack::stop();
  return true;
}

// ============= ISM BAND SWEEPER IMPLEMENTATION =============

bool ISMBandSweeper::begin() {
  Logger::getInstance().info("RF", "Balayage bande ISM initialisé");
  return true;
}

bool ISMBandSweeper::start() {
  Attack::start();
  freqPointsScanned = 0;
  anomaliesDetected = 0;
  Logger::getInstance().logAttackStart(getName(), "Balayage bande ISM 433/868/2400MHz");
  return true;
}

void ISMBandSweeper::update() {
  if (!isRunning) return;

  uint32_t elapsed = millis() - startTime;

  // Simulation: balayage multi-bande
  if (elapsed % 150 == 0 && freqPointsScanned < 100) {
    freqPointsScanned++;

    if (freqPointsScanned % 15 == 0) {
      anomaliesDetected++;
      float freq;
      if (freqPointsScanned < 30) freq = 433.0f + (freqPointsScanned * 0.05f);
      else if (freqPointsScanned < 60) freq = 868.0f + ((freqPointsScanned - 30) * 0.1f);
      else freq = 2400.0f + ((freqPointsScanned - 60) * 1.0f);

      char desc[64];
      snprintf(desc, 63, "Anomalie %.2f MHz", freq);
      AttackResult* result = ResultBuilder::createScan(desc, -70 + (freqPointsScanned % 20));
      addResult(result);
    }
  }

  if (elapsed > sweepTimeout || freqPointsScanned >= 100) {
    setStatus(anomaliesDetected > 0 ? AttackStatus::SUCCESS : AttackStatus::PARTIAL);
    isRunning = false;
    Logger::getInstance().logAttackEnd(getName(), currentStatus);
  }
}

bool ISMBandSweeper::stop() {
  Attack::stop();
  return true;
}
