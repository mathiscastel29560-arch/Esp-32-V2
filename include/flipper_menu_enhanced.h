#pragma once

#include <string>
#include <vector>
#include <cstdint>

// Enhanced menu system with beautiful UI, smooth navigation, and visual feedback
class FlipperMenuEnhanced {
public:
  enum MenuTab {
    MENU_MAIN,
    MENU_RF_TOOLS,
    MENU_BLUETOOTH,
    MENU_GPIO,
    MENU_BADUSB,
    MENU_MALWARE,
    MENU_IBUTTON,
    MENU_GAMES,
    MENU_ARCHIVE,
    MENU_SETTINGS,
    MENU_HELP,
    MENU_ABOUT,
  };

  struct MenuOption {
    std::string title;
    std::string icon;
    std::string description;
    std::string subcategory;
    uint16_t actionCode;
  };

  struct MenuState {
    MenuTab currentTab;
    uint8_t selectedIndex;
    uint8_t scrollOffset;
    bool isSubMenu;
    MenuTab previousTab;
  };

  // Singleton
  static FlipperMenuEnhanced& getInstance() {
    static FlipperMenuEnhanced instance;
    return instance;
  }

  // === NAVIGATION ===
  void displayMainMenu();
  void displayTabMenu(MenuTab tab);
  void openSubmenu(MenuTab tab);
  void closeSubmenu();
  void navigateUp();
  void navigateDown();
  void handleSelect(uint16_t actionCode);

  // === MENU RENDERING ===
  void renderMenuFrame(const std::string& title, const std::vector<MenuOption>& options);
  void renderSelectedItem(const MenuOption& item);
  void renderMenuInfo();
  void renderHelpPanel();

  // === ANIMATIONS ===
  void transitionToMenu(MenuTab newTab);
  void highlightMenuItem(uint8_t index);
  void confirmAction(const std::string& action);
  void showAlert(const std::string& message, bool isError = false);

  // === DATA ===
  std::vector<MenuOption> getMenuItems(MenuTab tab);
  MenuState getMenuState() const;
  void setMenuState(const MenuState& state);

private:
  FlipperMenuEnhanced() = default;
  
  MenuState currentState;
  std::vector<MenuOption> menuCache;
};
