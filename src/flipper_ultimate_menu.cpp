#include "flipper_ultimate_menu.h"
#include "flipper_ultimate.h"
#include "debug_logger.h"
#include <Arduino.h>

void FlipperUltimateMenu::displayMenu(UltimateTab tab) {
  currentTab = tab;

  Serial.println("\n╔════════════════════════════════════════╗");
  Serial.printf("║  %s %-34s║\n", getTabIcon(tab).c_str(), getTabName(tab).c_str());
  Serial.println("╚════════════════════════════════════════╝");

  switch (tab) {
    case ULTIMATE_LORA:
      displayLoRaMenu();
      break;
    case ULTIMATE_CELLULAR:
      displayCellularMenu();
      break;
    default:
      break;
  }
}

void FlipperUltimateMenu::handleSelect(UltimateTab tab, uint8_t itemIndex) {
  currentItem = itemIndex;

  switch (tab) {
    case ULTIMATE_LORA:
      handleLoRaSelect(itemIndex);
      break;
    case ULTIMATE_CELLULAR:
      handleCellularSelect(itemIndex);
      break;
    default:
      break;
  }
}

std::vector<FlipperUltimateMenu::UltimateMenuItem> FlipperUltimateMenu::getMenuItems(UltimateTab tab) {
  std::vector<UltimateMenuItem> items;

  switch (tab) {
    case ULTIMATE_LORA:
      items = {
        {"Initialize LoRa", "📡", "Setup 868 MHz LoRaWAN", 0},
        {"Scan Devices", "🔍", "Find LoRa devices (30s)", 1},
        {"Capture Packets", "📊", "Log LoRa traffic", 2},
        {"Send Packet", "📤", "Transmit to device", 3},
        {"Jam LoRa", "🌊", "Disrupt LoRa network", 4},
        {"Analyze Traffic", "📈", "Statistics & patterns", 5},
        {"Decrypt Payload", "🔓", "Break encryption", 6},
      };
      break;

    case ULTIMATE_CELLULAR:
      items = {
        {"Init Modem", "📱", "Start cellular scanning", 0},
        {"Scan Networks", "📡", "Find 4G/5G networks", 1},
        {"Connect", "🔗", "Join cellular network", 2},
        {"Disconnect", "🔌", "Leave network", 3},
        {"Fake Base Station", "📶", "Spoof tower", 4},
        {"IMSI Grab", "🎯", "Capture subscriber ID", 5},
        {"SIM Swap", "🔄", "SIM hijacking attack", 6},
        {"SSL Strip", "🔓", "Downgrade HTTPS", 7},
        {"DNS Hijack", "🌐", "Redirect DNS queries", 8},
        {"Location Tracking", "📍", "Triangulate position", 9},
        {"Signal Strength", "📊", "Analyze RSRP/SINR", 10},
        {"Fake BS Detect", "🔍", "Find rogue towers", 11},
        {"5G Scan", "⚡", "Detect 5G networks", 12},
        {"5G Vulnerabilities", "⚠️ ", "Analyze 5G gaps", 14},
      };
      break;

    default:
      break;
  }

  return items;
}

std::string FlipperUltimateMenu::getTabName(UltimateTab tab) const {
  switch (tab) {
    case ULTIMATE_LORA: return "LoRa / Wireless";
    case ULTIMATE_CELLULAR: return "Cellular / LTE / 5G";
    default: return "Unknown";
  }
}

std::string FlipperUltimateMenu::getTabIcon(UltimateTab tab) const {
  switch (tab) {
    case ULTIMATE_LORA: return "📡";
    case ULTIMATE_CELLULAR: return "📱";
    default: return "❓";
  }
}

// Display methods
void FlipperUltimateMenu::displayLoRaMenu() {
  auto items = getMenuItems(ULTIMATE_LORA);
  Serial.println("\n  📡 LoRa / LoRaWAN Tools:");
  for (size_t i = 0; i < items.size(); i++) {
    Serial.printf("  %s [%u] %s %s\n",
      (i == currentItem) ? "▶" : " ",
      i + 1, items[i].icon.c_str(), items[i].title.c_str());
  }
  Serial.println("\n  ▲▼ Navigate | ● Select | ◄ Back");
}

void FlipperUltimateMenu::displayCellularMenu() {
  auto items = getMenuItems(ULTIMATE_CELLULAR);
  Serial.println("\n  📱 Cellular / LTE / 5G Tools:");
  Serial.println("  ⚠️  WARNING: Many attacks are ILLEGAL!");
  for (size_t i = 0; i < std::min((size_t)7, items.size()); i++) {
    Serial.printf("  %s [%u] %s %s\n",
      (i == currentItem) ? "▶" : " ",
      i + 1, items[i].icon.c_str(), items[i].title.c_str());
  }
  Serial.println("  ... (more options available)");
  Serial.println("\n  ▲▼ Navigate | ● Select | ◄ Back");
}

// Handle methods
void FlipperUltimateMenu::handleLoRaSelect(uint8_t item) {
  auto& ultimate = FlipperUltimate::getInstance();

  switch (item) {
    case 0:
      Serial.println("\n📡 Initializing LoRa at 868 MHz...");
      ultimate.initLoRa(868000000, 7);
      break;

    case 1:
      Serial.println("\n🔍 Scanning for LoRa devices...");
      ultimate.scanLoRaDevices(30000);
      break;

    case 2:
      Serial.println("\n📊 Capturing LoRa packets for 60 seconds...");
      ultimate.captureLoRaPackets(60000);
      break;

    case 3:
      Serial.println("\n📤 Sending LoRa packet...");
      std::vector<uint8_t> payload = {0x48, 0x65, 0x6C, 0x6C, 0x6F};  // "Hello"
      ultimate.sendLoRaPacket("GATEWAY_EU", payload);
      break;

    case 4:
      Serial.println("\n🌊 Jamming LoRa network...");
      ultimate.jamLoRaNetwork(75);
      break;

    case 5:
      ultimate.analyzeLoRaTraffic();
      break;

    case 6:
      Serial.println("\n🔓 Attempting decryption...");
      FlipperUltimate::LoRaPacket packet;
      ultimate.decryptLoRaPayload(packet, "DemoKey123");
      break;
  }
}

void FlipperUltimateMenu::handleCellularSelect(uint8_t item) {
  auto& ultimate = FlipperUltimate::getInstance();

  switch (item) {
    case 0:
      Serial.println("\n📱 Initializing cellular modem...");
      ultimate.initCellularModem();
      delay(1000);
      Serial.println("✓ Modem ready\n");
      break;

    case 1:
      Serial.println("\n📡 Scanning for cellular networks...");
      ultimate.scanCellularNetworks(60000);
      break;

    case 2: {
      Serial.println("\n🔗 Connecting to network...");
      auto networks = ultimate.getDiscoveredNetworks();
      if (!networks.empty()) {
        ultimate.connectToNetwork(networks[0]);
      }
      break;
    }

    case 3:
      Serial.println("\n🔌 Disconnecting...");
      ultimate.disconnectNetwork();
      Serial.println("✓ Disconnected\n");
      break;

    case 4:
      Serial.println("\n⚠️  WARNING: This is ILLEGAL!");
      Serial.println("     Requires explicit authorization.");
      Serial.println("     Simulating fake base station...\n");
      ultimate.performFakeBSAttack("Verizon");
      break;

    case 5:
      Serial.println("\n⚠️  WARNING: CRIMINAL OFFENSE!");
      Serial.println("     Simulating IMSI capture...\n");
      ultimate.performIMSIGrab();
      break;

    case 6:
      Serial.println("\n⚠️  WARNING: FEDERAL CRIME!");
      Serial.println("     SIM swap is identity theft.\n");
      ultimate.performSimSwap("+1-555-0123");
      break;

    case 7:
      Serial.println("\n⚠️  SSL Strip Attack:");
      ultimate.performSSLStrip();
      break;

    case 8:
      Serial.println("\n⚠️  DNS Hijack Attack:");
      ultimate.performDNSHijack();
      break;

    case 9:
      ultimate.captureLocationData();
      break;

    case 10:
      ultimate.analyzeSignalStrength();
      break;

    case 11:
      ultimate.detectFakeBaseStations();
      break;

    case 12:
      ultimate.scan5G();
      break;

    case 13:
      ultimate.analyze5GSecurityGaps();
      break;
  }
}
