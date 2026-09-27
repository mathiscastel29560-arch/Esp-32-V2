#pragma once

#include <string>
#include <vector>
#include <cstdint>

// Flipper Zero Menu System Integration
class FlipperMenu {
public:
  enum FlipperTab {
    FLIPPER_RF_TOOLS,      // Sub-Ghz, NFC, IR (existing)
    FLIPPER_BLUETOOTH,     // BLE Scanner
    FLIPPER_GPIO,          // GPIO & UART
    FLIPPER_BADUSB,        // USB HID Attacks
    FLIPPER_MALWARE,       // Malware DB
    FLIPPER_IBUTTON,       // iButton Emulation
    FLIPPER_GAMES,         // Games & Utilities
    FLIPPER_ARCHIVE,       // File Manager
    FLIPPER_MAX
  };

  struct MenuItem {
    std::string title;
    std::string icon;
    std::string description;
    uint8_t tabIndex;
  };

  static FlipperMenu& getInstance() {
    static FlipperMenu instance;
    return instance;
  }

  // Display menu
  void displayMenu(FlipperTab tab);
  void handleSelect(FlipperTab tab, uint8_t itemIndex);
  void handleBack();

  // Get menu items for tab
  std::vector<MenuItem> getMenuItems(FlipperTab tab);

  // Tab names
  std::string getTabName(FlipperTab tab) const;
  std::string getTabIcon(FlipperTab tab) const;

  // Navigation
  FlipperTab getNextTab(FlipperTab current) const;
  FlipperTab getPreviousTab(FlipperTab current) const;

  // Current state
  FlipperTab getCurrentTab() const { return currentTab; }
  uint8_t getCurrentItem() const { return currentItem; }

private:
  FlipperMenu() = default;

  FlipperTab currentTab = FLIPPER_RF_TOOLS;
  uint8_t currentItem = 0;

  // Helper methods
  void displayRFToolsMenu();
  void displayBluetoothMenu();
  void displayGPIOMenu();
  void displayBadUSBMenu();
  void displayMalwareMenu();
  void displayIButtonMenu();
  void displayGamesMenu();
  void displayArchiveMenu();

  void handleRFTools(uint8_t item);
  void handleBluetooth(uint8_t item);
  void handleGPIO(uint8_t item);
  void handleBadUSB(uint8_t item);
  void handleMalware(uint8_t item);
  void handleIButton(uint8_t item);
  void handleGames(uint8_t item);
  void handleArchive(uint8_t item);
};
