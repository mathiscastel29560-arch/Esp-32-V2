#include "flipper_menu_enhanced.h"
#include "ui_enhanced.h"
#include <Arduino.h>
#include <algorithm>

void FlipperMenuEnhanced::displayMainMenu() {
  UIEnhanced& ui = UIEnhanced::getInstance();
  
  Serial.clear();
  delay(100);
  
  // Status bar
  UIEnhanced::StatusBar status{
    .batteryPercent = 85,
    .wifiConnected = true,
    .bleActive = false,
    .currentTime = "14:32",
    .temperature = "32°C"
  };
  ui.drawStatusBar(status);
  
  // Main menu header
  Serial.println("\n");
  Serial.println("╔════════════════════════════════════════════╗");
  Serial.println("║                                            ║");
  Serial.println("║         🐯 TIGER AUDIT PLATFORM 🐯         ║");
  Serial.println("║                                            ║");
  Serial.println("║     Professional Security Toolkit v3.1     ║");
  Serial.println("║                                            ║");
  Serial.println("╠════════════════════════════════════════════╣");
  Serial.println("║  SELECT A CATEGORY:                        ║");
  Serial.println("╠════════════════════════════════════════════╣");
  
  // Main menu items
  std::vector<std::string> items = {
    "📡  RF Tools          - Sub-Ghz, NFC, Infrared",
    "🔵  Bluetooth/BLE     - Device scanning & attacks",
    "🔌  GPIO & UART       - Hardware interface testing",
    "⌨️   BadUSB/HID       - USB emulation & injection",
    "🦠  Malware Scanner   - Threat detection",
    "🔑  iButton           - Physical key cloning",
    "🎮  Games & Tools     - Entertainment & utilities",
    "📁  Archive/Files     - Storage management",
    "⚙️   Settings         - Configuration",
    "❓  Help              - Documentation",
    "ℹ️   About             - Version info"
  };
  
  for (size_t i = 0; i < items.size(); i++) {
    if (i == currentState.selectedIndex) {
      Serial.printf("║ ▶ %s\n", items[i].c_str());
    } else {
      Serial.printf("║   %s\n", items[i].c_str());
    }
  }
  
  Serial.println("╚════════════════════════════════════════════╝");
  Serial.println("\n  ▲▼ Navigate  ● Select  ◄ Back  ? Help\n");
}

void FlipperMenuEnhanced::displayTabMenu(MenuTab tab) {
  UIEnhanced& ui = UIEnhanced::getInstance();
  
  Serial.clear();
  delay(100);
  
  // Determine tab name and icon
  std::string tabName;
  std::string icon;
  
  switch (tab) {
    case MENU_RF_TOOLS:
      tabName = "RF Tools";
      icon = "📡";
      break;
    case MENU_BLUETOOTH:
      tabName = "Bluetooth/BLE";
      icon = "🔵";
      break;
    case MENU_GPIO:
      tabName = "GPIO & UART";
      icon = "🔌";
      break;
    case MENU_BADUSB:
      tabName = "BadUSB/HID";
      icon = "⌨️";
      break;
    case MENU_MALWARE:
      tabName = "Malware Scanner";
      icon = "🦠";
      break;
    default:
      tabName = "Tools";
      icon = "🔧";
  }
  
  ui.displayMenuHeader(tabName, icon);
  
  auto items = getMenuItems(tab);
  for (size_t i = 0; i < items.size(); i++) {
    UIEnhanced::MenuItem item{
      .title = items[i].title,
      .icon = items[i].icon,
      .description = items[i].description,
      .color = UIEnhanced::ELECTRIC_BLUE,
      .isSelected = (i == currentState.selectedIndex),
      .level = 0
    };
    ui.displayMenuItem(item, i == currentState.selectedIndex);
  }
  
  ui.displayMenuFooter("▲▼ Navigate  ● Select  ◄ Back  ? Info");
}

void FlipperMenuEnhanced::openSubmenu(MenuTab tab) {
  currentState.previousTab = currentState.currentTab;
  currentState.currentTab = tab;
  currentState.selectedIndex = 0;
  currentState.isSubMenu = true;
  
  transitionToMenu(tab);
}

void FlipperMenuEnhanced::closeSubmenu() {
  if (currentState.isSubMenu) {
    currentState.currentTab = currentState.previousTab;
    currentState.selectedIndex = 0;
    currentState.isSubMenu = false;
  }
}

void FlipperMenuEnhanced::navigateUp() {
  if (currentState.selectedIndex > 0) {
    currentState.selectedIndex--;
  }
}

void FlipperMenuEnhanced::navigateDown() {
  auto items = getMenuItems(currentState.currentTab);
  if (currentState.selectedIndex < items.size() - 1) {
    currentState.selectedIndex++;
  }
}

void FlipperMenuEnhanced::handleSelect(uint16_t actionCode) {
  UIEnhanced& ui = UIEnhanced::getInstance();
  
  switch (actionCode) {
    case 1:
      ui.animateSuccess("Opening RF Tools");
      openSubmenu(MENU_RF_TOOLS);
      break;
    case 2:
      ui.animateSuccess("Opening Bluetooth/BLE");
      openSubmenu(MENU_BLUETOOTH);
      break;
    default:
      ui.animateWarning("Tool initializing...");
  }
}

void FlipperMenuEnhanced::renderMenuFrame(const std::string& title, const std::vector<MenuOption>& options) {
  Serial.println("\n╔════════════════════════════════════════════╗");
  Serial.printf("║ %s%-39s║\n", title.c_str(), "");
  Serial.println("╠════════════════════════════════════════════╣");
  
  for (size_t i = 0; i < options.size(); i++) {
    if (i == currentState.selectedIndex) {
      Serial.printf("║ ▶ %s %s\n", options[i].icon.c_str(), options[i].title.c_str());
    } else {
      Serial.printf("║   %s %s\n", options[i].icon.c_str(), options[i].title.c_str());
    }
  }
  
  Serial.println("╚════════════════════════════════════════════╝");
}

void FlipperMenuEnhanced::renderSelectedItem(const MenuOption& item) {
  Serial.println("\n╭─────────────────────────────────────────╮");
  Serial.printf("│ %s %s\n", item.icon.c_str(), item.title.c_str());
  Serial.println("├─────────────────────────────────────────┤");
  Serial.printf("│ %s\n", item.description.c_str());
  Serial.println("╰─────────────────────────────────────────╯\n");
}

void FlipperMenuEnhanced::transitionToMenu(MenuTab newTab) {
  UIEnhanced& ui = UIEnhanced::getInstance();
  
  // Smooth transition
  delay(150);
  displayTabMenu(newTab);
}

void FlipperMenuEnhanced::highlightMenuItem(uint8_t index) {
  currentState.selectedIndex = index;
}

void FlipperMenuEnhanced::confirmAction(const std::string& action) {
  UIEnhanced& ui = UIEnhanced::getInstance();
  ui.animateSuccess(action);
}

void FlipperMenuEnhanced::showAlert(const std::string& message, bool isError) {
  UIEnhanced& ui = UIEnhanced::getInstance();
  
  if (isError) {
    ui.animateError(message);
  } else {
    ui.animateWarning(message);
  }
}

std::vector<FlipperMenuEnhanced::MenuOption> FlipperMenuEnhanced::getMenuItems(MenuTab tab) {
  std::vector<MenuOption> items;
  
  switch (tab) {
    case MENU_RF_TOOLS:
      items = {
        {"Sub-Ghz Scanner", "📡", "Scan 433/868/915 MHz frequencies", "RF", 101},
        {"NFC/RFID Reader", "📱", "Read and emulate cards", "RF", 102},
        {"Infrared Control", "🔴", "Learn and replay IR codes", "RF", 103},
        {"RF Analyzer", "📊", "Analyze signal strength & patterns", "RF", 104},
      };
      break;
      
    case MENU_BLUETOOTH:
      items = {
        {"BLE Scanner", "🔍", "Discover Bluetooth devices", "BLE", 201},
        {"Device Connection", "🔗", "Connect to BT device", "BLE", 202},
        {"Device Emulation", "🎭", "Emulate BLE peripheral", "BLE", 203},
        {"BLE Sniffer", "👀", "Capture BLE packets", "BLE", 204},
        {"Spam Attack", "💥", "BLE Advertisement flooding", "BLE", 205},
      };
      break;
      
    case MENU_GPIO:
      items = {
        {"GPIO Scanner", "🔌", "Scan all GPIO pins", "GPIO", 301},
        {"Pin Reader", "📖", "Read individual pin states", "GPIO", 302},
        {"Pin Writer", "✍️", "Write GPIO pin values", "GPIO", 303},
        {"PWM Control", "〰️", "Configure PWM frequencies", "GPIO", 304},
      };
      break;
      
    default:
      items = {
        {"Not Implemented", "⚠️", "This tool is under development", "", 999},
      };
  }
  
  return items;
}

MenuState FlipperMenuEnhanced::getMenuState() const {
  return currentState;
}

void FlipperMenuEnhanced::setMenuState(const MenuState& state) {
  currentState = state;
}
