#include "ble_attacks.h"
#include "logging_system.h"

// ============= BLE SCANNER IMPLEMENTATION =============

bool BLEScanner::begin() {
  Logger::getInstance().info("BLE", "Scannérisation BLE initialisée");
  return true;
}

bool BLEScanner::start() {
  Attack::start();
  devicesFound = 0;
  Logger::getInstance().logAttackStart(getName(), "Recherche d'appareils BLE");
  return true;
}

void BLEScanner::update() {
  if (!isRunning) return;

  uint32_t elapsed = millis() - startTime;

  // Simulation: découverte progressive d'appareils BLE
  if (elapsed < 5000) {
    if (devicesFound < 5 && elapsed % 1000 == 0) {
      devicesFound++;
      char deviceName[32];
      snprintf(deviceName, 31, "Device_BLE_%u", devicesFound);

      AttackResult* result = ResultBuilder::createScan(deviceName, -55 - (devicesFound * 3));
      addResult(result);
      Logger::getInstance().logResult("BLE", result);
    }
  }

  if (elapsed > scanTimeout) {
    setStatus(AttackStatus::SUCCESS);
    isRunning = false;
    char msg[128];
    snprintf(msg, 127, "Trouvé %u appareils BLE", devicesFound);
    Logger::getInstance().logAttackEnd(getName(), currentStatus);
  }
}

bool BLEScanner::stop() {
  Attack::stop();
  return true;
}

bool BLEScanner::cleanup() {
  clearResults();
  return true;
}

bool BLEScanner::setParameter(const char* key, const char* value) {
  if (strcmp(key, "timeout") == 0) {
    scanTimeout = atoi(value);
    return true;
  }
  return false;
}

const char* BLEScanner::getParameter(const char* key) {
  static char buf[16];
  if (strcmp(key, "timeout") == 0) {
    snprintf(buf, 15, "%u", scanTimeout);
    return buf;
  }
  return nullptr;
}

// ============= BLE DISCONNECT ATTACK IMPLEMENTATION =============

bool BLEDisconnectAttack::begin() {
  Logger::getInstance().info("BLE", "Attaque Disconnect initialisée");
  return true;
}

bool BLEDisconnectAttack::start() {
  Attack::start();
  if (strlen(targetAddress) == 0) {
    Logger::getInstance().error("BLE", "Adresse cible non définie");
    return false;
  }

  disconnectsAttempted = 0;
  lastAttemptTime = millis();
  char msg[128];
  snprintf(msg, 127, "Déconnexion de %s", targetAddress);
  Logger::getInstance().logAttackStart(getName(), msg);
  return true;
}

void BLEDisconnectAttack::update() {
  if (!isRunning) return;

  uint32_t now = millis();
  uint32_t elapsed = now - startTime;

  // Tentatives de déconnexion tous les 500ms
  if (now - lastAttemptTime > 500 && disconnectsAttempted < 10) {
    disconnectsAttempted++;
    lastAttemptTime = now;

    if (disconnectsAttempted % 5 == 0) {
      char desc[64];
      snprintf(desc, 63, "Tentative déconnexion #%u", disconnectsAttempted);
      AttackResult* result = ResultBuilder::createPacket(
        (uint8_t*)"DISCONNECT_REQ", 14, desc);
      addResult(result);
    }
  }

  if (elapsed > attackDuration || disconnectsAttempted >= 10) {
    setStatus(disconnectsAttempted > 0 ? AttackStatus::SUCCESS : AttackStatus::FAILED);
    isRunning = false;
    Logger::getInstance().logAttackEnd(getName(), currentStatus);
  }
}

bool BLEDisconnectAttack::stop() {
  Attack::stop();
  return true;
}

bool BLEDisconnectAttack::setParameter(const char* key, const char* value) {
  if (strcmp(key, "address") == 0) {
    strncpy(targetAddress, value, 17);
    return true;
  }
  if (strcmp(key, "duration") == 0) {
    attackDuration = atoi(value);
    return true;
  }
  return false;
}

const char* BLEDisconnectAttack::getParameter(const char* key) {
  if (strcmp(key, "address") == 0) return targetAddress;
  if (strcmp(key, "duration") == 0) {
    static char buf[8];
    snprintf(buf, 7, "%u", attackDuration);
    return buf;
  }
  return nullptr;
}

// ============= BLE ADVERTISEMENT INJECTION IMPLEMENTATION =============

bool BLEAdvertisementInjection::begin() {
  Logger::getInstance().info("BLE", "Injection Publicité initialisée");
  return true;
}

bool BLEAdvertisementInjection::start() {
  Attack::start();
  advertisementsCreated = 0;
  lastAdvertisementTime = millis();
  Logger::getInstance().logAttackStart(getName(), "Injection de fausses publicités BLE");
  return true;
}

void BLEAdvertisementInjection::update() {
  if (!isRunning) return;

  uint32_t now = millis();
  uint32_t elapsed = now - startTime;

  // Création de fausses publicités tous les 500ms
  if (now - lastAdvertisementTime > 500 && advertisementsCreated < 20) {
    advertisementsCreated++;
    lastAdvertisementTime = now;

    char desc[64];
    snprintf(desc, 63, "Publicité injectée #%u", advertisementsCreated);
    AttackResult* result = ResultBuilder::createPacket(
      (uint8_t*)"ADV_FRAME", 9, desc);
    addResult(result);
  }

  if (elapsed > injectionDuration || advertisementsCreated >= 20) {
    setStatus(AttackStatus::SUCCESS);
    isRunning = false;
    Logger::getInstance().logAttackEnd(getName(), currentStatus);
  }
}

bool BLEAdvertisementInjection::stop() {
  Attack::stop();
  return true;
}

// ============= BLE GATT ENUMERATION IMPLEMENTATION =============

bool BLEGATTEnumeration::begin() {
  Logger::getInstance().info("BLE", "Énumération GATT initialisée");
  return true;
}

bool BLEGATTEnumeration::start() {
  Attack::start();
  servicesFound = 0;
  characteristicsFound = 0;
  Logger::getInstance().logAttackStart(getName(), "Énumération GATT en cours");
  return true;
}

void BLEGATTEnumeration::update() {
  if (!isRunning) return;

  uint32_t elapsed = millis() - startTime;

  // Simulation: découverte de services et caractéristiques
  if (elapsed < 10000) {
    if (servicesFound < 4 && elapsed % 2000 == 0) {
      servicesFound++;
      char desc[64];
      snprintf(desc, 63, "Service GATT #%u découvert", servicesFound);
      AttackResult* result = ResultBuilder::createScan(desc, -40);
      addResult(result);
    }
  }

  if (elapsed > 10000 && elapsed < 15000) {
    if (characteristicsFound < 8 && elapsed % 1000 == 0) {
      characteristicsFound++;
      char desc[64];
      snprintf(desc, 63, "Caractéristique #%u", characteristicsFound);
      AttackResult* result = ResultBuilder::createPacket(
        (uint8_t*)"GATT_CHAR", 9, desc);
      addResult(result);
    }
  }

  if (elapsed > enumerationTimeout) {
    setStatus(servicesFound > 0 ? AttackStatus::SUCCESS : AttackStatus::FAILED);
    isRunning = false;
    Logger::getInstance().logAttackEnd(getName(), currentStatus);
  }
}

bool BLEGATTEnumeration::stop() {
  Attack::stop();
  return true;
}

bool BLEGATTEnumeration::setParameter(const char* key, const char* value) {
  if (strcmp(key, "timeout") == 0) {
    enumerationTimeout = atoi(value);
    return true;
  }
  return false;
}

// ============= BLE PAIRING REPLAY IMPLEMENTATION =============

bool BLEPairingReplay::begin() {
  Logger::getInstance().info("BLE", "Relecture Appairage initialisée");
  return true;
}

bool BLEPairingReplay::start() {
  Attack::start();
  keysReplayed = 0;
  memset(targetAddress, 0, 18);
  Logger::getInstance().logAttackStart(getName(), "Relecture de clés d'appairage BLE");
  return true;
}

void BLEPairingReplay::update() {
  if (!isRunning) return;

  uint32_t elapsed = millis() - startTime;

  // Simulation: relecture progressive de clés
  if (elapsed < 10000) {
    if (keysReplayed < 4 && elapsed % 2000 == 0) {
      keysReplayed++;
      uint8_t keyData[16] = {0};
      for (int i = 0; i < 16; i++) {
        keyData[i] = (keysReplayed * 16 + i) & 0xFF;
      }

      char desc[64];
      snprintf(desc, 63, "Clé appairage #%u rejouée", keysReplayed);
      AttackResult* result = ResultBuilder::createPacket(keyData, 16, desc);
      addResult(result);
    }
  }

  if (elapsed > replayDuration) {
    setStatus(keysReplayed > 0 ? AttackStatus::SUCCESS : AttackStatus::FAILED);
    isRunning = false;
    Logger::getInstance().logAttackEnd(getName(), currentStatus);
  }
}

bool BLEPairingReplay::stop() {
  Attack::stop();
  return true;
}

bool BLEPairingReplay::setParameter(const char* key, const char* value) {
  if (strcmp(key, "address") == 0) {
    strncpy(targetAddress, value, 17);
    return true;
  }
  return false;
}

// ============= BLE SWEEPER IMPLEMENTATION =============

bool BLESweeper::begin() {
  Logger::getInstance().info("BLE", "Balayage BLE initialisé");
  return true;
}

bool BLESweeper::start() {
  Attack::start();
  channelsSwept = 0;
  vulnerableDevicesFound = 0;
  Logger::getInstance().logAttackStart(getName(), "Balayage multi-canal BLE");
  return true;
}

void BLESweeper::update() {
  if (!isRunning) return;

  uint32_t elapsed = millis() - startTime;

  // Simulation: balayage des 37 canaux BLE
  if (elapsed < 15000) {
    if (channelsSwept < 37 && elapsed % 400 == 0) {
      channelsSwept++;

      if (channelsSwept % 12 == 0) { // Quelques appareils trouvés
        vulnerableDevicesFound++;
        char desc[64];
        snprintf(desc, 63, "Appareil vulnérable canal %u", channelsSwept);
        AttackResult* result = ResultBuilder::createScan(desc, -50 - (channelsSwept % 5));
        addResult(result);
      }
    }
  }

  if (elapsed > sweepTimeout) {
    setStatus(vulnerableDevicesFound > 0 ? AttackStatus::SUCCESS : AttackStatus::PARTIAL);
    isRunning = false;
    Logger::getInstance().logAttackEnd(getName(), currentStatus);
  }
}

bool BLESweeper::stop() {
  Attack::stop();
  return true;
}

// ============= BLE SNIFFER IMPLEMENTATION =============

bool BLESniffer::begin() {
  Logger::getInstance().info("BLE", "Interception BLE initialisée");
  return true;
}

bool BLESniffer::start() {
  Attack::start();
  packetsSniffed = 0;
  Logger::getInstance().logAttackStart(getName(), "Interception des paquets BLE");
  return true;
}

void BLESniffer::update() {
  if (!isRunning) return;

  uint32_t elapsed = millis() - startTime;

  // Simulation: interception progressive de paquets
  if (elapsed % 200 == 0 && packetsSniffed < 100) {
    packetsSniffed++;

    if (packetsSniffed % 25 == 0) {
      char desc[64];
      snprintf(desc, 63, "Paquet #%u intercepté", packetsSniffed);
      AttackResult* result = ResultBuilder::createPacket(
        (uint8_t*)"BLE_PDU", 7, desc);
      addResult(result);
    }
  }

  if (elapsed > sniffDuration || packetsSniffed >= 100) {
    setStatus(AttackStatus::SUCCESS);
    isRunning = false;
    char msg[128];
    snprintf(msg, 127, "%u paquets interceptés", packetsSniffed);
    Logger::getInstance().logAttackEnd(getName(), currentStatus);
  }
}

bool BLESniffer::stop() {
  Attack::stop();
  return true;
}
