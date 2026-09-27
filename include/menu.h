#ifndef MENU_H
#define MENU_H

#include <Arduino.h>

// ============= MENU STRUCTURE =============

enum MenuTab {
  TAB_WIFI = 0,        // 📡 WiFi Tools (16 attacks)
  TAB_BLE = 1,         // 🔵 BLE Tools (13 attacks)
  TAB_RF_2400 = 2,     // 📶 RF/2.4GHz (14 attacks)
  TAB_IOT = 3,         // 🛰️ IoT/Advanced (8 attacks)
  TAB_NFC = 4,         // 🔴 NFC/RFID (11 attacks)
  TAB_IR = 5,          // 🔴 IR Tools (11 attacks)
  TAB_JAMMING = 6,     // 💥 Jamming/DoS (12 attacks)
  TAB_RF_433 = 7,      // 📡 433MHz Tools (10 attacks)
  TAB_SYSTEM = 8,      // ⚙️ System
  TAB_SETTINGS = 9,    // ⚙️ Settings
  TAB_HARDWARE_TEST = 10, // 🔧 Hardware Test
  TAB_DEVICE_INFO = 11,   // ℹ️ Device Info
  TAB_HELP = 12        // ❓ Help
};

// ============= MENU CLASS =============

class Menu {
public:
  Menu();
  void begin();
  void update();
  void display();
  void handleTouchInput(int x, int y);
  void handleButtonInput(int button);

private:
  MenuTab currentTab;
  int selectedItem;
  bool needsRedraw;

  // Tab handlers
  void displayWiFiTab();
  void displayBLETab();
  void displayRF2400Tab();
  void displayIoTTab();
  void displayNFCTab();
  void displayIRTab();
  void displayJammingTab();
  void displayRF433Tab();
  void displaySystemTab();
  void displaySettingsTab();
  void displayHardwareTestTab();
  void displayDeviceInfoTab();
  void displayHelpTab();

  // Status bar
  void drawStatusBar();
  void drawMenuItems();
  void drawSelectedItemHighlight();
};

extern Menu gMenu;

#endif // MENU_H
