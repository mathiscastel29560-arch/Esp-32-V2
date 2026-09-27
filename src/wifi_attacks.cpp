#include "wifi_attacks.h"
#include "logging_system.h"

// ============= WIFI NETWORK SCAN IMPLEMENTATION =============

bool WiFiNetworkScan::begin() {
  Logger::getInstance().info("WiFi", "Scannérisation WiFi initialisée");
  return true;
}

bool WiFiNetworkScan::start() {
  Attack::start();
  scanStartTime = millis();
  networksFound = 0;
  Logger::getInstance().logAttackStart(getName(), "Réseaux WiFi locaux");
  return true;
}

void WiFiNetworkScan::update() {
  if (!isRunning) return;

  uint32_t elapsed = millis() - scanStartTime;

  // Simulation: découverte progressive de réseaux
  if (elapsed < 5000) {
    if (networksFound < 3 && elapsed % 1500 == 0) {
      networksFound++;
      char ssid[32];
      snprintf(ssid, 31, "Reseau_%u", networksFound);

      AttackResult* result = ResultBuilder::createScan(ssid, -45 - (networksFound * 5));
      addResult(result);
      Logger::getInstance().logResult("WiFi", result);
    }
  }

  if (elapsed > SCAN_TIMEOUT) {
    setStatus(AttackStatus::SUCCESS);
    isRunning = false;

    char msg[128];
    snprintf(msg, 127, "Trouvé %u réseaux", networksFound);
    Logger::getInstance().logAttackEnd(getName(), currentStatus);
  }
}

bool WiFiNetworkScan::stop() {
  Attack::stop();
  return true;
}

bool WiFiNetworkScan::cleanup() {
  clearResults();
  return true;
}

bool WiFiNetworkScan::setParameter(const char* key, const char* value) {
  if (strcmp(key, "timeout") == 0) {
    SCAN_TIMEOUT = atoi(value);
    return true;
  }
  return false;
}

const char* WiFiNetworkScan::getParameter(const char* key) {
  static char buf[16];
  if (strcmp(key, "timeout") == 0) {
    snprintf(buf, 15, "%u", SCAN_TIMEOUT);
    return buf;
  }
  return nullptr;
}

// ============= WIFI DEAUTH ATTACK IMPLEMENTATION =============

bool WiFiDeauthAttack::begin() {
  Logger::getInstance().info("WiFi", "Attaque Deauth initialisée");
  return true;
}

bool WiFiDeauthAttack::start() {
  Attack::start();
  if (strlen(targetBSSID) == 0) {
    Logger::getInstance().error("WiFi", "BSSID cible non défini");
    return false;
  }

  packetsTransmitted = 0;
  char msg[128];
  snprintf(msg, 127, "Déauthentification de %s sur canal %u", targetBSSID, channel);
  Logger::getInstance().logAttackStart(getName(), msg);
  return true;
}

void WiFiDeauthAttack::update() {
  if (!isRunning) return;

  static uint32_t lastPacketTime = 0;
  uint32_t now = millis();

  if (now - lastPacketTime > PACKET_INTERVAL) {
    packetsTransmitted++;
    lastPacketTime = now;

    // Créer résultat tous les 10 paquets
    if (packetsTransmitted % 10 == 0) {
      char desc[64];
      snprintf(desc, 63, "Paquet Deauth %u", packetsTransmitted);
      AttackResult* result = ResultBuilder::createPacket(
        (uint8_t*)"DEAUTH_FRAME", 12, desc);
      addResult(result);
    }

    // Arrêt après 50 paquets
    if (packetsTransmitted >= 50) {
      setStatus(AttackStatus::SUCCESS);
      isRunning = false;
      Logger::getInstance().logAttackEnd(getName(), currentStatus);
    }
  }
}

bool WiFiDeauthAttack::stop() {
  Attack::stop();
  return true;
}

bool WiFiDeauthAttack::setParameter(const char* key, const char* value) {
  if (strcmp(key, "bssid") == 0) {
    strncpy(targetBSSID, value, 17);
    targetBSSID[17] = '\0';
    return true;
  }
  if (strcmp(key, "channel") == 0) {
    channel = atoi(value);
    return true;
  }
  return false;
}

const char* WiFiDeauthAttack::getParameter(const char* key) {
  if (strcmp(key, "bssid") == 0) return targetBSSID;
  if (strcmp(key, "channel") == 0) {
    static char buf[4];
    snprintf(buf, 3, "%u", channel);
    return buf;
  }
  return nullptr;
}

// ============= WIFI BEACON FLOOD IMPLEMENTATION =============

bool WiFiBeaconFlood::begin() {
  Logger::getInstance().info("WiFi", "Inondation Beacon initialisée");
  return true;
}

bool WiFiBeaconFlood::start() {
  Attack::start();
  lastBeaconTime = millis();
  Logger::getInstance().logAttackStart(getName(), "Diffusion de faux beacons");
  return true;
}

void WiFiBeaconFlood::update() {
  if (!isRunning) return;

  uint32_t now = millis();
  uint32_t intervalMs = 1000 / beaconsPerSecond;

  if (now - lastBeaconTime > intervalMs) {
    lastBeaconTime = now;

    char desc[64];
    snprintf(desc, 63, "Beacon %u@%u dBm", getResultCount(), (int)transmitPower);
    AttackResult* result = ResultBuilder::createPacket(
      (uint8_t*)"BEACON_FRAME", 12, desc);
    addResult(result);

    if (getResultCount() >= 100) {
      setStatus(AttackStatus::SUCCESS);
      isRunning = false;
      Logger::getInstance().logAttackEnd(getName(), currentStatus);
    }
  }
}

bool WiFiBeaconFlood::stop() {
  Attack::stop();
  return true;
}

// ============= WIFI PMKID CAPTURE IMPLEMENTATION =============

bool WiFiPMKIDCapture::begin() {
  Logger::getInstance().info("WiFi", "Capture PMKID initialisée");
  return true;
}

bool WiFiPMKIDCapture::start() {
  Attack::start();
  pmkidsFound = 0;
  Logger::getInstance().logAttackStart(getName(), "Écoute PMKID");
  return true;
}

void WiFiPMKIDCapture::update() {
  if (!isRunning) return;

  uint32_t elapsed = millis() - startTime;

  // Simulation: capture PMKID
  if (elapsed < 10000 && elapsed % 3000 == 0 && pmkidsFound < 2) {
    pmkidsFound++;
    uint8_t pmkidData[16] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                              0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10};

    char desc[64];
    snprintf(desc, 63, "PMKID #%u capturé", pmkidsFound);
    AttackResult* result = ResultBuilder::createPacket(pmkidData, 16, desc);
    addResult(result);
    Logger::getInstance().logResult("WiFi", result);
  }

  if (elapsed > captureTimeout) {
    if (pmkidsFound > 0) {
      setStatus(AttackStatus::SUCCESS);
    } else {
      setStatus(AttackStatus::FAILED);
    }
    isRunning = false;
    Logger::getInstance().logAttackEnd(getName(), currentStatus);
  }
}

bool WiFiPMKIDCapture::stop() {
  Attack::stop();
  return true;
}

// ============= WIFI HANDSHAKE CAPTURE IMPLEMENTATION =============

bool WiFiHandshakeCapture::begin() {
  Logger::getInstance().info("WiFi", "Capture Handshake initialisée");
  return true;
}

bool WiFiHandshakeCapture::start() {
  Attack::start();
  handshakesFound = 0;
  Logger::getInstance().logAttackStart(getName(), "Écoute WPA handshake");
  return true;
}

void WiFiHandshakeCapture::update() {
  if (!isRunning) return;

  uint32_t elapsed = millis() - startTime;

  // Simulation: capture handshake 4-way
  if (elapsed > 15000 && handshakesFound == 0) {
    handshakesFound = 1;
    uint8_t handshakeData[256] = {0}; // Données handshake 4-way

    AttackResult* result = ResultBuilder::createPacket(
      handshakeData, 256, "WPA handshake 4-way capturé");
    addResult(result);
    Logger::getInstance().logResult("WiFi", result);
  }

  if (elapsed > captureTimeout) {
    setStatus(handshakesFound > 0 ? AttackStatus::SUCCESS : AttackStatus::FAILED);
    isRunning = false;
    Logger::getInstance().logAttackEnd(getName(), currentStatus);
  }
}

bool WiFiHandshakeCapture::stop() {
  Attack::stop();
  return true;
}

// ============= WIFI EVIL TWIN IMPLEMENTATION =============

bool WiFiEvilTwin::begin() {
  Logger::getInstance().info("WiFi", "Evil Twin initialisé");
  return true;
}

bool WiFiEvilTwin::start() {
  Attack::start();
  clientsConnected = 0;
  char msg[128];
  snprintf(msg, 127, "Hébergement faux AP: %s", ssid);
  Logger::getInstance().logAttackStart(getName(), msg);
  return true;
}

void WiFiEvilTwin::update() {
  if (!isRunning) return;

  uint32_t elapsed = millis() - startTime;

  // Simulation: connexions client
  if (elapsed % 5000 == 0 && elapsed > 0 && clientsConnected < 3) {
    clientsConnected++;

    char desc[64];
    snprintf(desc, 63, "Client %u connecté", clientsConnected);
    AttackResult* result = ResultBuilder::createScan(desc, -65);
    addResult(result);
  }

  if (elapsed > 30000) {
    setStatus(AttackStatus::SUCCESS);
    isRunning = false;
    Logger::getInstance().logAttackEnd(getName(), currentStatus);
  }
}

bool WiFiEvilTwin::stop() {
  Attack::stop();
  return true;
}

bool WiFiEvilTwin::setParameter(const char* key, const char* value) {
  if (strcmp(key, "ssid") == 0) {
    strncpy(ssid, value, 32);
    ssid[32] = '\0';
    return true;
  }
  return false;
}

// ============= WIFI JAMMING IMPLEMENTATION =============

bool WiFiJamming::begin() {
  Logger::getInstance().info("WiFi", "Brouillage initialisé");
  return true;
}

bool WiFiJamming::start() {
  Attack::start();
  char msg[128];
  snprintf(msg, 127, "Brouillage bande 2.4GHz à %.1f dBm", transmitPower);
  Logger::getInstance().logAttackStart(getName(), msg);
  return true;
}

void WiFiJamming::update() {
  if (!isRunning) return;

  uint32_t elapsed = millis() - startTime;

  if (elapsed > jammingDuration) {
    setStatus(AttackStatus::SUCCESS);
    isRunning = false;
    Logger::getInstance().logAttackEnd(getName(), currentStatus);
  }
}

bool WiFiJamming::stop() {
  Attack::stop();
  return true;
}
