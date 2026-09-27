#pragma once

#include <string>
#include <vector>
#include <cstdint>

// Advanced tools menu for CAN, Jamming, JTAG
class FlipperAdvancedMenu {
public:
  enum AdvancedTab {
    ADV_CAN_BUS,      // Automotive CAN bus tools
    ADV_JAMMING,      // WiFi/BLE/RF jamming
    ADV_JTAG,         // Hardware debugging
    ADV_TAB_MAX
  };

  struct MenuOption {
    std::string title;
    std::string icon;
    std::string description;
    uint8_t id;
  };

  static FlipperAdvancedMenu& getInstance() {
    static FlipperAdvancedMenu instance;
    return instance;
  }

  // Display & Navigation
  void displayMenu(AdvancedTab tab);
  void handleSelect(AdvancedTab tab, uint8_t itemIndex);
  std::vector<MenuOption> getMenuItems(AdvancedTab tab);

  // Tab info
  std::string getTabName(AdvancedTab tab) const;
  std::string getTabIcon(AdvancedTab tab) const;

  // Current state
  AdvancedTab getCurrentTab() const { return currentTab; }
  uint8_t getCurrentItem() const { return currentItem; }

private:
  FlipperAdvancedMenu() = default;

  AdvancedTab currentTab = ADV_CAN_BUS;
  uint8_t currentItem = 0;

  // Display methods
  void displayCANMenu();
  void displayJammingMenu();
  void displayJTAGMenu();

  // Handle methods
  void handleCANSelect(uint8_t item);
  void handleJammingSelect(uint8_t item);
  void handleJTAGSelect(uint8_t item);
};
