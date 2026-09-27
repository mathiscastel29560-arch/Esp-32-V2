#include "flipper_menu.h"
#include "flipper_tools.h"
#include "debug_logger.h"
#include <Arduino.h>

void FlipperMenu::displayMenu(FlipperTab tab) {
  currentTab = tab;

  // Status bar
  Serial.println("\n╔════════════════════════════════════════╗");
  Serial.printf("║  %s%-36s║\n", getTabIcon(tab).c_str(), getTabName(tab).c_str());
  Serial.println("╚════════════════════════════════════════╝");

  switch (tab) {
    case FLIPPER_RF_TOOLS:
      displayRFToolsMenu();
      break;
    case FLIPPER_BLUETOOTH:
      displayBluetoothMenu();
      break;
    case FLIPPER_GPIO:
      displayGPIOMenu();
      break;
    case FLIPPER_BADUSB:
      displayBadUSBMenu();
      break;
    case FLIPPER_MALWARE:
      displayMalwareMenu();
      break;
    case FLIPPER_IBUTTON:
      displayIButtonMenu();
      break;
    case FLIPPER_GAMES:
      displayGamesMenu();
      break;
    case FLIPPER_ARCHIVE:
      displayArchiveMenu();
      break;
    default:
      break;
  }
}

void FlipperMenu::handleSelect(FlipperTab tab, uint8_t itemIndex) {
  currentItem = itemIndex;

  switch (tab) {
    case FLIPPER_RF_TOOLS:
      handleRFTools(itemIndex);
      break;
    case FLIPPER_BLUETOOTH:
      handleBluetooth(itemIndex);
      break;
    case FLIPPER_GPIO:
      handleGPIO(itemIndex);
      break;
    case FLIPPER_BADUSB:
      handleBadUSB(itemIndex);
      break;
    case FLIPPER_MALWARE:
      handleMalware(itemIndex);
      break;
    case FLIPPER_IBUTTON:
      handleIButton(itemIndex);
      break;
    case FLIPPER_GAMES:
      handleGames(itemIndex);
      break;
    case FLIPPER_ARCHIVE:
      handleArchive(itemIndex);
      break;
    default:
      break;
  }
}

std::vector<FlipperMenu::MenuItem> FlipperMenu::getMenuItems(FlipperTab tab) {
  std::vector<MenuItem> items;

  switch (tab) {
    case FLIPPER_RF_TOOLS:
      items = {
        {"Sub-Ghz Scanner", "📡", "Scan 433/868/915 MHz", 0},
        {"NFC/RFID Reader", "📱", "Read and emulate cards", 1},
        {"Infrared Control", "🔴", "Learn and replay IR codes", 2},
        {"RF Analyzer", "📊", "Analyze signal strength", 3},
      };
      break;
    case FLIPPER_BLUETOOTH:
      items = {
        {"BLE Scanner", "🔵", "Scan Bluetooth devices", 0},
        {"Connect Device", "🔗", "Connect to BT device", 1},
        {"Emulate Device", "🎭", "Pretend to be a BT device", 2},
        {"BLE Sniffer", "👀", "Capture BLE packets", 3},
      };
      break;
    case FLIPPER_GPIO:
      items = {
        {"GPIO Scanner", "🔌", "Scan all GPIO pins", 0},
        {"Read Pin", "📖", "Read pin state", 1},
        {"Write Pin", "✏️ ", "Set pin state", 2},
        {"UART Monitor", "📺", "Monitor serial ports", 3},
      };
      break;
    case FLIPPER_BADUSB:
      items = {
        {"Keyboard Script", "⌨️ ", "Execute keyboard sequence", 0},
        {"Mouse Control", "🖱️ ", "Control mouse pointer", 1},
        {"USB Scanner", "🔍", "Detect USB devices", 2},
        {"HID Devices", "🎮", "List connected devices", 3},
      };
      break;
    case FLIPPER_MALWARE:
      items = {
        {"Load Database", "💾", "Load malware signatures", 0},
        {"Scan Files", "🔎", "Scan for malware", 1},
        {"Hash Calculator", "🔐", "Calculate file hash", 2},
        {"Threat Report", "📋", "View threat database", 3},
      };
      break;
    case FLIPPER_IBUTTON:
      items = {
        {"Read iButton", "🔑", "Read iButton key", 0},
        {"Add Key", "➕", "Register new iButton", 1},
        {"Emulate Key", "🎭", "Emulate stored key", 2},
        {"Stored Keys", "📚", "List saved keys", 3},
      };
      break;
    case FLIPPER_GAMES:
      items = {
        {"Snake Game", "🐍", "Classic snake game", 0},
        {"Flappy Bird", "🐦", "Bird obstacle game", 1},
        {"Alarm Clock", "⏰", "Set alarms and timers", 2},
        {"Metronome", "🎵", "Music tempo keeper", 3},
      };
      break;
    case FLIPPER_ARCHIVE:
      items = {
        {"File Browser", "📁", "Browse file system", 0},
        {"Delete File", "🗑️ ", "Remove files", 1},
        {"Rename File", "✏️ ", "Rename files", 2},
        {"Disk Usage", "💾", "Show storage info", 3},
      };
      break;
    default:
      break;
  }

  return items;
}

std::string FlipperMenu::getTabName(FlipperTab tab) const {
  switch (tab) {
    case FLIPPER_RF_TOOLS: return "RF Tools (Sub-Ghz, NFC, IR)";
    case FLIPPER_BLUETOOTH: return "Bluetooth / BLE";
    case FLIPPER_GPIO: return "GPIO & UART";
    case FLIPPER_BADUSB: return "BadUSB / HID";
    case FLIPPER_MALWARE: return "Malware Scanner";
    case FLIPPER_IBUTTON: return "iButton Emulation";
    case FLIPPER_GAMES: return "Games & Utilities";
    case FLIPPER_ARCHIVE: return "Archive / Files";
    default: return "Unknown";
  }
}

std::string FlipperMenu::getTabIcon(FlipperTab tab) const {
  switch (tab) {
    case FLIPPER_RF_TOOLS: return "📡";
    case FLIPPER_BLUETOOTH: return "🔵";
    case FLIPPER_GPIO: return "🔌";
    case FLIPPER_BADUSB: return "⌨️ ";
    case FLIPPER_MALWARE: return "🦠";
    case FLIPPER_IBUTTON: return "🔑";
    case FLIPPER_GAMES: return "🎮";
    case FLIPPER_ARCHIVE: return "📁";
    default: return "❓";
  }
}

FlipperMenu::FlipperTab FlipperMenu::getNextTab(FlipperTab current) const {
  uint8_t next = (uint8_t)current + 1;
  if (next >= FLIPPER_MAX) return FLIPPER_RF_TOOLS;
  return (FlipperTab)next;
}

FlipperMenu::FlipperTab FlipperMenu::getPreviousTab(FlipperTab current) const {
  if (current == 0) return (FlipperTab)(FLIPPER_MAX - 1);
  return (FlipperTab)((uint8_t)current - 1);
}

// Menu displays
void FlipperMenu::displayRFToolsMenu() {
  auto items = getMenuItems(FLIPPER_RF_TOOLS);
  Serial.println("\n  RF & Radio Tools:");
  for (size_t i = 0; i < items.size(); i++) {
    Serial.printf("  %s [%u] %s - %s\n",
      (i == currentItem) ? "▶" : " ",
      i + 1, items[i].icon.c_str(), items[i].title.c_str());
    Serial.printf("       %s\n", items[i].description.c_str());
  }
  Serial.println("\n  ▲▼ Navigate | ● Select | ◄ Back");
}

void FlipperMenu::displayBluetoothMenu() {
  auto items = getMenuItems(FLIPPER_BLUETOOTH);
  Serial.println("\n  Bluetooth/BLE Tools:");
  for (size_t i = 0; i < items.size(); i++) {
    Serial.printf("  %s [%u] %s %s\n",
      (i == currentItem) ? "▶" : " ",
      i + 1, items[i].icon.c_str(), items[i].title.c_str());
  }
  Serial.println("\n  ▲▼ Navigate | ● Select | ◄ Back");
}

void FlipperMenu::displayGPIOMenu() {
  auto items = getMenuItems(FLIPPER_GPIO);
  Serial.println("\n  GPIO & UART Tools:");
  for (size_t i = 0; i < items.size(); i++) {
    Serial.printf("  %s [%u] %s %s\n",
      (i == currentItem) ? "▶" : " ",
      i + 1, items[i].icon.c_str(), items[i].title.c_str());
  }
  Serial.println("\n  ▲▼ Navigate | ● Select | ◄ Back");
}

void FlipperMenu::displayBadUSBMenu() {
  auto items = getMenuItems(FLIPPER_BADUSB);
  Serial.println("\n  BadUSB / HID Tools:");
  for (size_t i = 0; i < items.size(); i++) {
    Serial.printf("  %s [%u] %s %s\n",
      (i == currentItem) ? "▶" : " ",
      i + 1, items[i].icon.c_str(), items[i].title.c_str());
  }
  Serial.println("\n  ▲▼ Navigate | ● Select | ◄ Back");
}

void FlipperMenu::displayMalwareMenu() {
  auto items = getMenuItems(FLIPPER_MALWARE);
  Serial.println("\n  Malware Scanner:");
  for (size_t i = 0; i < items.size(); i++) {
    Serial.printf("  %s [%u] %s %s\n",
      (i == currentItem) ? "▶" : " ",
      i + 1, items[i].icon.c_str(), items[i].title.c_str());
  }
  Serial.println("\n  ▲▼ Navigate | ● Select | ◄ Back");
}

void FlipperMenu::displayIButtonMenu() {
  auto items = getMenuItems(FLIPPER_IBUTTON);
  Serial.println("\n  iButton Emulation:");
  for (size_t i = 0; i < items.size(); i++) {
    Serial.printf("  %s [%u] %s %s\n",
      (i == currentItem) ? "▶" : " ",
      i + 1, items[i].icon.c_str(), items[i].title.c_str());
  }
  Serial.println("\n  ▲▼ Navigate | ● Select | ◄ Back");
}

void FlipperMenu::displayGamesMenu() {
  auto items = getMenuItems(FLIPPER_GAMES);
  Serial.println("\n  Games & Utilities:");
  for (size_t i = 0; i < items.size(); i++) {
    Serial.printf("  %s [%u] %s %s\n",
      (i == currentItem) ? "▶" : " ",
      i + 1, items[i].icon.c_str(), items[i].title.c_str());
  }
  Serial.println("\n  ▲▼ Navigate | ● Select | ◄ Back");
}

void FlipperMenu::displayArchiveMenu() {
  auto items = getMenuItems(FLIPPER_ARCHIVE);
  Serial.println("\n  Archive / File Manager:");
  for (size_t i = 0; i < items.size(); i++) {
    Serial.printf("  %s [%u] %s %s\n",
      (i == currentItem) ? "▶" : " ",
      i + 1, items[i].icon.c_str(), items[i].title.c_str());
  }
  Serial.println("\n  ▲▼ Navigate | ● Select | ◄ Back");
}

// Handle selections
void FlipperMenu::handleRFTools(uint8_t item) {
  auto& flipper = FlipperTools::getInstance();
  DebugLogger::printf("[FlipperMenu] RF Tools item: %u\n", item);
  // RF tools already handled by existing systems
}

void FlipperMenu::handleBluetooth(uint8_t item) {
  auto& flipper = FlipperTools::getInstance();
  switch (item) {
    case 0:
      Serial.println("\n📊 Starting BLE scan...");
      flipper.startBLEScanning(10000);
      break;
    case 1:
      Serial.println("\n🔗 Connect to device");
      flipper.connectBTDevice("00:11:22:33:44:55");
      break;
    case 2:
      Serial.println("\n🎭 Emulating BT device...");
      flipper.emulateBTDevice("ESP32-Flipper");
      break;
  }
}

void FlipperMenu::handleGPIO(uint8_t item) {
  auto& flipper = FlipperTools::getInstance();
  switch (item) {
    case 0: {
      Serial.println("\n🔌 GPIO Scan:");
      auto pins = flipper.scanGPIO();
      for (const auto& pin : pins) {
        Serial.printf("  GPIO %u: %s [%s]\n",
          pin.pin, pin.description.c_str(),
          pin.level ? "HIGH" : "LOW");
      }
      break;
    }
    case 1:
      Serial.println("\n📖 Reading pin...");
      break;
    case 2:
      Serial.println("\n✏️  Writing pin...");
      break;
  }
}

void FlipperMenu::handleBadUSB(uint8_t item) {
  auto& flipper = FlipperTools::getInstance();
  switch (item) {
    case 0:
      Serial.println("\n⌨️  BadUSB Keyboard Attack");
      flipper.sendKeyboardSequence("Hello World");
      break;
    case 1:
      Serial.println("\n🖱️  Mouse Control");
      flipper.sendMouseMove(10, 10);
      break;
  }
}

void FlipperMenu::handleMalware(uint8_t item) {
  auto& flipper = FlipperTools::getInstance();
  switch (item) {
    case 0:
      Serial.println("\n💾 Loading malware database...");
      flipper.loadMalwareDatabase();
      Serial.println("✓ Database loaded!");
      break;
    case 1:
      Serial.println("\n🔎 Scanning for malware...");
      // TODO: Scan files in system
      break;
  }
}

void FlipperMenu::handleIButton(uint8_t item) {
  auto& flipper = FlipperTools::getInstance();
  switch (item) {
    case 0:
      Serial.println("\n🔑 Reading iButton...");
      break;
    case 1:
      Serial.println("\n➕ Adding new iButton key...");
      break;
  }
}

void FlipperMenu::handleGames(uint8_t item) {
  auto& flipper = FlipperTools::getInstance();
  switch (item) {
    case 0:
      flipper.playSnakeGame();
      break;
    case 1:
      flipper.playFlappyBirdGame();
      break;
    case 2:
      flipper.displayAlarmClock();
      break;
    case 3:
      flipper.displayMetronome(120);
      break;
  }
}

void FlipperMenu::handleArchive(uint8_t item) {
  auto& flipper = FlipperTools::getInstance();
  switch (item) {
    case 0:
      Serial.println("\n📁 File Browser");
      flipper.listFiles("/spiffs");
      break;
    case 1:
      Serial.println("\n🗑️  Delete File");
      break;
  }
}

void FlipperMenu::handleBack() {
  DebugLogger::println("[FlipperMenu] Back pressed");
}
