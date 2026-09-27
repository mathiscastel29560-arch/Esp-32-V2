#pragma once

#include <string>
#include <vector>
#include <cstdint>

// Ultimate tools menu for LoRa and Cellular
class FlipperUltimateMenu {
public:
  enum UltimateTab {
    ULTIMATE_LORA,       // LoRa/Wireless
    ULTIMATE_CELLULAR,   // Cellular/LTE/5G
    ULTIMATE_TAB_MAX
  };

  struct UltimateMenuItem {
    std::string title;
    std::string icon;
    std::string description;
    uint8_t id;
  };

  static FlipperUltimateMenu& getInstance() {
    static FlipperUltimateMenu instance;
    return instance;
  }

  // Display & Navigation
  void displayMenu(UltimateTab tab);
  void handleSelect(UltimateTab tab, uint8_t itemIndex);
  std::vector<UltimateMenuItem> getMenuItems(UltimateTab tab);

  // Tab info
  std::string getTabName(UltimateTab tab) const;
  std::string getTabIcon(UltimateTab tab) const;

  // Current state
  UltimateTab getCurrentTab() const { return currentTab; }
  uint8_t getCurrentItem() const { return currentItem; }

private:
  FlipperUltimateMenu() = default;

  UltimateTab currentTab = ULTIMATE_LORA;
  uint8_t currentItem = 0;

  // Display methods
  void displayLoRaMenu();
  void displayCellularMenu();

  // Handle methods
  void handleLoRaSelect(uint8_t item);
  void handleCellularSelect(uint8_t item);
};
