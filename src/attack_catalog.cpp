#include "attack_catalog.h"
#include "logging_system.h"

// ============= ATTACK NAMES AND DESCRIPTIONS =============

const char* AttackCatalog::WIFI_ATTACKS[] = {
  "WiFi Network Scan",
  "WiFi Deauth",
  "WiFi Beacon Flood",
  "WiFi PMKID Capture",
  "WiFi Handshake Capture",
  "WiFi Evil Twin",
  "WiFi Jamming"
};

const char* AttackCatalog::BLE_ATTACKS[] = {
  "BLE Scanner",
  "BLE Disconnect",
  "BLE Advertisement Injection",
  "BLE GATT Enumeration",
  "BLE Pairing Replay",
  "BLE Sweeper",
  "BLE Sniffer"
};

const char* AttackCatalog::RF_ATTACKS[] = {
  "NRF24 Scanner",
  "NRF24 Jammer",
  "CC1101 Scanner",
  "CC1101 Transmitter",
  "Drone Protocol Analyzer",
  "IR Spoofer",
  "LoRa Sniffer",
  "ISM Band Sweeper"
};

const char* AttackCatalog::NFC_ATTACKS[] = {
  "NFC Tag Reader",
  "MIFARE Cloner",
  "NFC Emulator",
  "RFID Clone Detector",
  "Zigbee Sniffer",
  "MQTT Interceptor",
  "Smart Home Scanner"
};

const char* AttackCatalog::DOS_ATTACKS[] = {
  "Flood Attack",
  "Amplification Attack",
  "Slowloris Attack",
  "DNS Amplification",
  "NTP Reflection",
  "Resource Exhaustion"
};

const char* AttackCatalog::ADVANCED_ATTACKS[] = {
  "Packet Sniffer",
  "Man-in-the-Middle",
  "Credential Harvester",
  "SSL Stripping",
  "DNS Spoofing",
  "ARP Spoofing",
  "Vulnerability Scanner",
  "Brute Force"
};

AttackCatalog::AttackCatalog() {
  Logger::getInstance().info("AttackCatalog", "Catalogues d'attaques initialisé");
}

// ============= WIFI ATTACKS FACTORY =============

Attack* AttackCatalog::createWiFiAttack(uint8_t index) {
  switch (index) {
    case 0: return new WiFiNetworkScan();
    case 1: return new WiFiDeauthAttack();
    case 2: return new WiFiBeaconFlood();
    case 3: return new WiFiPMKIDCapture();
    case 4: return new WiFiHandshakeCapture();
    case 5: return new WiFiEvilTwin();
    case 6: return new WiFiJamming();
    default: return nullptr;
  }
}

// ============= BLE ATTACKS FACTORY =============

Attack* AttackCatalog::createBLEAttack(uint8_t index) {
  switch (index) {
    case 0: return new BLEScanner();
    case 1: return new BLEDisconnectAttack();
    case 2: return new BLEAdvertisementInjection();
    case 3: return new BLEGATTEnumeration();
    case 4: return new BLEPairingReplay();
    case 5: return new BLESweeper();
    case 6: return new BLESniffer();
    default: return nullptr;
  }
}

// ============= RF ATTACKS FACTORY =============

Attack* AttackCatalog::createRFAttack(uint8_t index) {
  switch (index) {
    case 0: return new NRF24Scanner();
    case 1: return new NRF24Jammer();
    case 2: return new CC1101Scanner();
    case 3: return new CC1101Transmitter();
    case 4: return new DroneProtocolAnalyzer();
    case 5: return new IRSpoofer();
    case 6: return new LoRaSniffer();
    case 7: return new ISMBandSweeper();
    default: return nullptr;
  }
}

// ============= NFC ATTACKS FACTORY =============

Attack* AttackCatalog::createNFCAttack(uint8_t index) {
  switch (index) {
    case 0: return new NFCTagReader();
    case 1: return new MIFARECloner();
    case 2: return new NFCEmulator();
    case 3: return new RFIDCloneDetector();
    case 4: return new ZigbeeSniffer();
    case 5: return new MQTTInterceptor();
    case 6: return new SmartHomeScanner();
    default: return nullptr;
  }
}

// ============= DOS ATTACKS FACTORY =============

Attack* AttackCatalog::createDOSAttack(uint8_t index) {
  switch (index) {
    case 0: return new FloodAttack();
    case 1: return new AmplificationAttack();
    case 2: return new SlowlorisAttack();
    case 3: return new DNSAmplification();
    case 4: return new NTPReflection();
    case 5: return new ResourceExhaustion();
    default: return nullptr;
  }
}

// ============= ADVANCED ATTACKS FACTORY =============

Attack* AttackCatalog::createAdvancedAttack(uint8_t index) {
  switch (index) {
    case 0: return new PacketSniffer();
    case 1: return new ManInTheMiddle();
    case 2: return new CredentialHarvester();
    case 3: return new SSLStripping();
    case 4: return new DNSSpoofing();
    case 5: return new ARPSpoofing();
    case 6: return new VulnerabilityScanner();
    case 7: return new BruteForceAttack();
    default: return nullptr;
  }
}

// ============= UNIFIED INTERFACE =============

Attack* AttackCatalog::createAttack(AttackCategory category, uint8_t attackIndex) {
  switch (category) {
    case AttackCategory::WIFI:
      return createWiFiAttack(attackIndex);
    case AttackCategory::BLE:
      return createBLEAttack(attackIndex);
    case AttackCategory::RF:
      return createRFAttack(attackIndex);
    case AttackCategory::NFC:
      return createNFCAttack(attackIndex);
    case AttackCategory::DOS:
      return createDOSAttack(attackIndex);
    case AttackCategory::ADVANCED:
      return createAdvancedAttack(attackIndex);
    default:
      return nullptr;
  }
}

Attack* AttackCatalog::createAttackByName(const char* name) {
  // Chercher dans WiFi
  for (int i = 0; i < WIFI_COUNT; i++) {
    if (strcmp(name, WIFI_ATTACKS[i]) == 0) {
      return createWiFiAttack(i);
    }
  }

  // Chercher dans BLE
  for (int i = 0; i < BLE_COUNT; i++) {
    if (strcmp(name, BLE_ATTACKS[i]) == 0) {
      return createBLEAttack(i);
    }
  }

  // Chercher dans RF
  for (int i = 0; i < RF_COUNT; i++) {
    if (strcmp(name, RF_ATTACKS[i]) == 0) {
      return createRFAttack(i);
    }
  }

  // Chercher dans NFC
  for (int i = 0; i < NFC_COUNT; i++) {
    if (strcmp(name, NFC_ATTACKS[i]) == 0) {
      return createNFCAttack(i);
    }
  }

  // Chercher dans DoS
  for (int i = 0; i < DOS_COUNT; i++) {
    if (strcmp(name, DOS_ATTACKS[i]) == 0) {
      return createDOSAttack(i);
    }
  }

  // Chercher dans Advanced
  for (int i = 0; i < ADVANCED_COUNT; i++) {
    if (strcmp(name, ADVANCED_ATTACKS[i]) == 0) {
      return createAdvancedAttack(i);
    }
  }

  return nullptr;
}

// ============= CATALOG INFORMATION =============

const char* AttackCatalog::getCategoryName(AttackCategory category) {
  switch (category) {
    case AttackCategory::WIFI: return "WiFi";
    case AttackCategory::BLE: return "Bluetooth Low Energy";
    case AttackCategory::RF: return "Radio Fréquence";
    case AttackCategory::NFC: return "NFC/RFID/IoT";
    case AttackCategory::DOS: return "Déni de Service";
    case AttackCategory::ADVANCED: return "Avancé";
    default: return "Inconnu";
  }
}

uint8_t AttackCatalog::getAttackCountByCategory(AttackCategory category) {
  switch (category) {
    case AttackCategory::WIFI: return WIFI_COUNT;
    case AttackCategory::BLE: return BLE_COUNT;
    case AttackCategory::RF: return RF_COUNT;
    case AttackCategory::NFC: return NFC_COUNT;
    case AttackCategory::DOS: return DOS_COUNT;
    case AttackCategory::ADVANCED: return ADVANCED_COUNT;
    default: return 0;
  }
}

const char* AttackCatalog::getAttackName(AttackCategory category, uint8_t index) {
  switch (category) {
    case AttackCategory::WIFI:
      return (index < WIFI_COUNT) ? WIFI_ATTACKS[index] : nullptr;
    case AttackCategory::BLE:
      return (index < BLE_COUNT) ? BLE_ATTACKS[index] : nullptr;
    case AttackCategory::RF:
      return (index < RF_COUNT) ? RF_ATTACKS[index] : nullptr;
    case AttackCategory::NFC:
      return (index < NFC_COUNT) ? NFC_ATTACKS[index] : nullptr;
    case AttackCategory::DOS:
      return (index < DOS_COUNT) ? DOS_ATTACKS[index] : nullptr;
    case AttackCategory::ADVANCED:
      return (index < ADVANCED_COUNT) ? ADVANCED_ATTACKS[index] : nullptr;
    default:
      return nullptr;
  }
}

const char* AttackCatalog::getAttackDescription(AttackCategory category, uint8_t index) {
  // Descriptions détaillées de chaque attaque
  const char* desc = getAttackName(category, index);
  return desc ? desc : "Attaque inconnue";
}

// ============= CATALOG PRINTING =============

void AttackCatalog::printCatalog() {
  Serial.println("\n========== CATALOGUE D'ATTAQUES ==========");
  Serial.printf("Total: %u attaques sur 6 catégories\n\n",
                WIFI_COUNT + BLE_COUNT + RF_COUNT + NFC_COUNT + DOS_COUNT + ADVANCED_COUNT);

  for (uint8_t i = 0; i < (uint8_t)AttackCategory::TOTAL_CATEGORIES; i++) {
    printCategory((AttackCategory)i);
  }

  Serial.println("==========================================\n");
}

void AttackCatalog::printCategory(AttackCategory category) {
  Serial.printf("\n📁 %s (%u attaques)\n", getCategoryName(category),
                getAttackCountByCategory(category));
  Serial.println("─────────────────────────────────────");

  uint8_t count = getAttackCountByCategory(category);
  for (uint8_t i = 0; i < count; i++) {
    Serial.printf("  [%u] %s\n", i + 1, getAttackName(category, i));
  }
}
