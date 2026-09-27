#include "menu.h"
#include "drivers.h"
#include "config.h"

// ============= GLOBAL MENU INSTANCE =============

Menu gMenu;

// ============= MENU IMPLEMENTATION =============

Menu::Menu() : currentTab(TAB_WIFI), selectedItem(0), needsRedraw(true) {
}

void Menu::begin() {
  // TODO: Initialize menu system
  // Load fonts, setup display regions
}

void Menu::update() {
  // TODO: Update menu state
  // Check for touch/button inputs
  // Update timers, animations
}

void Menu::display() {
  if (!needsRedraw) return;

  // Draw status bar at top
  drawStatusBar();

  // Draw current tab
  switch (currentTab) {
    case TAB_WIFI:
      displayWiFiTab();
      break;
    case TAB_BLE:
      displayBLETab();
      break;
    case TAB_RF_2400:
      displayRF2400Tab();
      break;
    case TAB_IOT:
      displayIoTTab();
      break;
    case TAB_NFC:
      displayNFCTab();
      break;
    case TAB_IR:
      displayIRTab();
      break;
    case TAB_JAMMING:
      displayJammingTab();
      break;
    case TAB_RF_433:
      displayRF433Tab();
      break;
    case TAB_SYSTEM:
      displaySystemTab();
      break;
    case TAB_SETTINGS:
      displaySettingsTab();
      break;
    case TAB_HARDWARE_TEST:
      displayHardwareTestTab();
      break;
    case TAB_DEVICE_INFO:
      displayDeviceInfoTab();
      break;
    case TAB_HELP:
      displayHelpTab();
      break;
  }

  needsRedraw = false;
}

void Menu::handleTouchInput(int x, int y) {
  // TODO: Handle touchscreen input
}

void Menu::handleButtonInput(int button) {
  // TODO: Handle physical button input
}

// ============= TAB DISPLAY METHODS =============

void Menu::displayWiFiTab() {
  // TODO: Display WiFi Tools (16 attacks)
  // - Network Scan
  // - Handshake Capture
  // - PMKID Capture
  // - Deauth Attack
  // - Beacon Flood
  // - WiFi Jamming
  // - Evil Twin
  // - DNS Spoofing
  // - Fake Portal
  // - SSL Stripping
  // - MAC Tracking
  // - Hidden SSID Finder
  Drivers::Display::drawString(10, 50, "WiFi Tools - Coming Soon", 0xFFFF);
}

void Menu::displayBLETab() {
  // TODO: Display BLE Tools (13 attacks)
  Drivers::Display::drawString(10, 50, "BLE Tools - Coming Soon", 0xFFFF);
}

void Menu::displayRF2400Tab() {
  // TODO: Display 2.4GHz Tools
  Drivers::Display::drawString(10, 50, "RF/2.4GHz Tools - Coming Soon", 0xFFFF);
}

void Menu::displayIoTTab() {
  // TODO: Display IoT/Advanced
  Drivers::Display::drawString(10, 50, "IoT/Advanced - Coming Soon", 0xFFFF);
}

void Menu::displayNFCTab() {
  // TODO: Display NFC/RFID
  Drivers::Display::drawString(10, 50, "NFC/RFID - Coming Soon", 0xFFFF);
}

void Menu::displayIRTab() {
  // TODO: Display IR Tools
  Drivers::Display::drawString(10, 50, "IR Tools - Coming Soon", 0xFFFF);
}

void Menu::displayJammingTab() {
  // TODO: Display Jamming/DoS
  Drivers::Display::drawString(10, 50, "Jamming/DoS - Coming Soon", 0xFFFF);
}

void Menu::displayRF433Tab() {
  // TODO: Display 433MHz Tools
  Drivers::Display::drawString(10, 50, "433MHz Tools - Coming Soon", 0xFFFF);
}

void Menu::displaySystemTab() {
  // TODO: Display System info (battery, memory, GPS)
  Drivers::Display::drawString(10, 50, "System - Coming Soon", 0xFFFF);
}

void Menu::displaySettingsTab() {
  // TODO: Display Settings
  Drivers::Display::drawString(10, 50, "Settings - Coming Soon", 0xFFFF);
}

void Menu::displayHardwareTestTab() {
  // TODO: Display Hardware Test menu
  Drivers::Display::drawString(10, 50, "Hardware Test - Coming Soon", 0xFFFF);
}

void Menu::displayDeviceInfoTab() {
  // TODO: Display Device Info
  Drivers::Display::drawString(10, 50, "Device Info - Coming Soon", 0xFFFF);
}

void Menu::displayHelpTab() {
  // TODO: Display Help/Documentation
  Drivers::Display::drawString(10, 50, "Help - Coming Soon", 0xFFFF);
}

// ============= INTERNAL METHODS =============

void Menu::drawStatusBar() {
  // TODO: Draw status bar at top
  // - WiFi indicator
  // - Battery indicator
  // - Time
  Drivers::Display::fillRect(0, 0, DISPLAY_WIDTH, 30, 0x0000);
}

void Menu::drawMenuItems() {
  // TODO: Draw menu items for current tab
}

void Menu::drawSelectedItemHighlight() {
  // TODO: Draw highlight around selected item
}
