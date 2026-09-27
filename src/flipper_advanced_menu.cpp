#include "flipper_advanced_menu.h"
#include "flipper_advanced.h"
#include "debug_logger.h"
#include <Arduino.h>

void FlipperAdvancedMenu::displayMenu(AdvancedTab tab) {
  currentTab = tab;

  Serial.println("\n╔════════════════════════════════════════╗");
  Serial.printf("║  %s %-34s║\n", getTabIcon(tab).c_str(), getTabName(tab).c_str());
  Serial.println("╚════════════════════════════════════════╝");

  switch (tab) {
    case ADV_CAN_BUS:
      displayCANMenu();
      break;
    case ADV_JAMMING:
      displayJammingMenu();
      break;
    case ADV_JTAG:
      displayJTAGMenu();
      break;
    default:
      break;
  }
}

void FlipperAdvancedMenu::handleSelect(AdvancedTab tab, uint8_t itemIndex) {
  currentItem = itemIndex;

  switch (tab) {
    case ADV_CAN_BUS:
      handleCANSelect(itemIndex);
      break;
    case ADV_JAMMING:
      handleJammingSelect(itemIndex);
      break;
    case ADV_JTAG:
      handleJTAGSelect(itemIndex);
      break;
    default:
      break;
  }
}

std::vector<FlipperAdvancedMenu::MenuOption> FlipperAdvancedMenu::getMenuItems(AdvancedTab tab) {
  std::vector<MenuOption> items;

  switch (tab) {
    case ADV_CAN_BUS:
      items = {
        {"Initialize CAN Bus", "🚗", "Setup CAN interface at 500kbps", 0},
        {"Scan Network", "🔍", "Enumerate CAN devices/ECUs", 1},
        {"Capture Messages", "📊", "Log CAN traffic (30s)", 2},
        {"Send Message", "📤", "Transmit custom CAN frame", 3},
        {"Flood CAN Bus", "🌊", "DoS: Send 1000 random frames", 4},
        {"Fuzz Messages", "🎲", "Send malformed CAN packets", 5},
        {"Analyze Traffic", "📈", "Show statistics & patterns", 6},
      };
      break;

    case ADV_JAMMING:
      items = {
        {"WiFi Jammer", "📡", "Disrupt 2.4GHz WiFi (50% power)", 0},
        {"BLE Jammer", "🔵", "Jam Bluetooth Low Energy", 1},
        {"RF Jammer", "📻", "Jam custom frequency", 2},
        {"Noise Pattern", "🎵", "Generate interference pattern", 3},
        {"Jammed Devices", "📋", "Show affected devices", 4},
        {"Effectiveness", "📊", "Calculate jam success rate", 5},
        {"Stop Jamming", "🛑", "Disable all jamming", 6},
      };
      break;

    case ADV_JTAG:
      items = {
        {"Init JTAG", "🔌", "Configure JTAG pins (TCK,TMS,TDI,TDO)", 0},
        {"Init SWD", "🔌", "Configure SWD pins (CLK,DATA)", 1},
        {"Scan Chain", "🔍", "Detect connected debug devices", 2},
        {"Memory Map", "📍", "Read chip memory layout", 3},
        {"Read Memory", "📖", "Dump chip memory (Flash/RAM)", 4},
        {"Write Memory", "✏️ ", "Patch firmware in memory", 5},
        {"Dump Firmware", "💾", "Extract full firmware to file", 6},
        {"Identify Chip", "🎯", "Get chip ID & architecture", 7},
        {"Breakpoint", "🔴", "Set debug breakpoint", 8},
        {"Step Debug", "⏭️ ", "Step through execution", 9},
      };
      break;

    default:
      break;
  }

  return items;
}

std::string FlipperAdvancedMenu::getTabName(AdvancedTab tab) const {
  switch (tab) {
    case ADV_CAN_BUS: return "CAN Bus (Automotive)";
    case ADV_JAMMING: return "Jamming Tools";
    case ADV_JTAG: return "JTAG/SWD Debug";
    default: return "Unknown";
  }
}

std::string FlipperAdvancedMenu::getTabIcon(AdvancedTab tab) const {
  switch (tab) {
    case ADV_CAN_BUS: return "🚗";
    case ADV_JAMMING: return "📡";
    case ADV_JTAG: return "🔧";
    default: return "❓";
  }
}

// Display methods
void FlipperAdvancedMenu::displayCANMenu() {
  auto items = getMenuItems(ADV_CAN_BUS);
  Serial.println("\n  🚗 Automotive CAN Bus Tools:");
  for (size_t i = 0; i < items.size(); i++) {
    Serial.printf("  %s [%u] %s %s\n",
      (i == currentItem) ? "▶" : " ",
      i + 1, items[i].icon.c_str(), items[i].title.c_str());
  }
  Serial.println("\n  ▲▼ Navigate | ● Select | ◄ Back");
}

void FlipperAdvancedMenu::displayJammingMenu() {
  auto items = getMenuItems(ADV_JAMMING);
  Serial.println("\n  📡 Jamming & Interference Tools:");
  Serial.println("  ⚠️  WARNING: Illegal in most jurisdictions!");
  for (size_t i = 0; i < items.size(); i++) {
    Serial.printf("  %s [%u] %s %s\n",
      (i == currentItem) ? "▶" : " ",
      i + 1, items[i].icon.c_str(), items[i].title.c_str());
  }
  Serial.println("\n  ▲▼ Navigate | ● Select | ◄ Back");
}

void FlipperAdvancedMenu::displayJTAGMenu() {
  auto items = getMenuItems(ADV_JTAG);
  Serial.println("\n  🔧 JTAG/SWD Hardware Debugging:");
  for (size_t i = 0; i < std::min((size_t)5, items.size()); i++) {
    Serial.printf("  %s [%u] %s %s\n",
      (i == currentItem) ? "▶" : " ",
      i + 1, items[i].icon.c_str(), items[i].title.c_str());
  }
  Serial.println("  ... (more options available)");
  Serial.println("\n  ▲▼ Navigate | ● Select | ◄ Back");
}

// Handle methods
void FlipperAdvancedMenu::handleCANSelect(uint8_t item) {
  auto& adv = FlipperAdvanced::getInstance();

  switch (item) {
    case 0:
      Serial.println("\n⚙️  Initializing CAN Bus at 500kbps...");
      adv.initCANBus(500000);
      delay(1000);
      Serial.println("✓ CAN Bus ready!\n");
      break;

    case 1:
      Serial.println("\n🔍 Scanning CAN network...");
      adv.scanCANNetwork();
      break;

    case 2:
      Serial.println("\n📊 Capturing CAN messages for 30 seconds...");
      auto messages = adv.captureCANMessages(30000);
      Serial.printf("Captured %u messages\n", messages.size());
      break;

    case 3:
      Serial.println("\n📤 Sending custom CAN frame ID: 0x123");
      FlipperAdvanced::CANMessage msg;
      msg.id = 0x123;
      msg.dlc = 8;
      msg.data[0] = 0xAA;
      msg.data[1] = 0xBB;
      adv.sendCANMessage(msg);
      Serial.println("✓ Message sent\n");
      break;

    case 4:
      Serial.println("\n🌊 Flooding CAN bus with 1000 messages...");
      adv.floodCANBus(0x100, 8, 1000);
      Serial.println("✓ Flood complete\n");
      break;

    case 5:
      Serial.println("\n🎲 Fuzzing CAN bus for 10 seconds...");
      adv.fuzzyCANMessages(10000);
      Serial.println("✓ Fuzzing complete\n");
      break;

    case 6:
      adv.analyzeCANTraffic();
      break;
  }
}

void FlipperAdvancedMenu::handleJammingSelect(uint8_t item) {
  auto& adv = FlipperAdvanced::getInstance();

  switch (item) {
    case 0:
      if (!adv.isJammingActive()) {
        Serial.println("\n⚠️  Starting WiFi Jamming at 50% power...");
        Serial.println("🚨 WARNING: Illegal in most countries!");
        adv.startWiFiJamming(50);
      } else {
        Serial.println("\n⚠️  Jamming already active!");
      }
      break;

    case 1:
      if (!adv.isJammingActive()) {
        Serial.println("\n⚠️  Starting BLE Jamming...");
        adv.startBLEJamming(50);
      } else {
        Serial.println("\n⚠️  Jamming already active!");
      }
      break;

    case 2:
      if (!adv.isJammingActive()) {
        Serial.println("\n📻 Starting RF Jamming at 868 MHz...");
        adv.startRFJamming(868000000, 50);
      } else {
        Serial.println("\n⚠️  Jamming already active!");
      }
      break;

    case 3:
      Serial.println("\n🎵 Generating noise patterns:");
      adv.generateNoisePattern("burst");
      delay(500);
      adv.generateNoisePattern("sweep");
      delay(500);
      adv.generateNoisePattern("random");
      break;

    case 4:
      {
        auto devices = adv.getJammedDevices();
        Serial.printf("\nJammed Devices: %u\n", devices.size());
        for (const auto& dev : devices) {
          Serial.printf("  - %s (%s, -%.1f dB)\n",
            dev.address.c_str(), dev.type.c_str(), (float)dev.rssiLoss);
        }
      }
      break;

    case 5:
      {
        float eff = adv.getJamEffectiveness();
        Serial.printf("\n📊 Jamming Effectiveness: %.1f%%\n", eff);
      }
      break;

    case 6:
      if (adv.isJammingActive()) {
        Serial.println("\n🛑 Stopping all jamming...");
        adv.stopJamming();
      } else {
        Serial.println("\n✓ No active jamming\n");
      }
      break;
  }
}

void FlipperAdvancedMenu::handleJTAGSelect(uint8_t item) {
  auto& adv = FlipperAdvanced::getInstance();

  switch (item) {
    case 0:
      Serial.println("\n🔌 Configuring JTAG pins...");
      adv.initJTAG(14, 15, 11, 12);  // Example pins
      break;

    case 1:
      Serial.println("\n🔌 Configuring SWD pins...");
      adv.initSWD(14, 15);
      break;

    case 2:
      Serial.println("\n🔍 Scanning JTAG chain...");
      adv.scanJTAGDevices();
      break;

    case 3:
      Serial.println("\n📍 Memory Map:");
      auto regions = adv.readMemoryMap();
      for (const auto& reg : regions) {
        Serial.printf("  0x%08X - 0x%08X: %s (%s) [%s]\n",
          reg.startAddress,
          reg.startAddress + reg.size,
          reg.type.c_str(),
          reg.permissions.c_str(),
          reg.isAccessible ? "✓" : "✗");
      }
      Serial.println();
      break;

    case 4:
      Serial.println("\n📖 Reading 1KB from Flash (0x08000000)...");
      auto data = adv.readMemory(0x08000000, 1024);
      Serial.printf("Read %u bytes\n", data.size());
      Serial.printf("First 16 bytes: ");
      for (int i = 0; i < 16; i++) {
        Serial.printf("%02X ", data[i]);
      }
      Serial.println("\n");
      break;

    case 5:
      Serial.println("\n✏️  Writing firmware patch...");
      std::vector<uint8_t> patch = {0x90, 0x90, 0x90, 0x90};
      adv.writeMemory(0x08000100, patch);
      break;

    case 6:
      Serial.println("\n💾 Dumping firmware...");
      adv.dumpFirmware(0x08000000, 0x100000, "/spiffs/firmware.bin");
      break;

    case 7: {
      Serial.println("\n🎯 Chip Identification:");
      std::string chip = adv.identifyChip();
      Serial.printf("  Chip: %s\n\n", chip.c_str());
      break;
    }

    case 8:
      Serial.println("\n🔴 Setting breakpoint at 0x08000200...");
      adv.setBreakpoint(0x08000200);
      break;

    case 9:
      Serial.println("\n⏭️  Stepping debugger...");
      adv.stepDebugger();
      break;
  }
}
