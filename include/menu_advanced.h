#ifndef MENU_ADVANCED_H
#define MENU_ADVANCED_H

#include <Arduino.h>
#include <vector>
#include "attack_framework.h"

// ============= UI CONSTANTS =============

#define DISPLAY_WIDTH 320
#define DISPLAY_HEIGHT 240
#define STATUS_BAR_HEIGHT 30
#define TAB_HEIGHT 40
#define MENU_ITEM_HEIGHT 40
#define PADDING 10

// ============= COLOR DEFINITIONS =============

#define COLOR_BLACK 0x0000
#define COLOR_WHITE 0xFFFF
#define COLOR_RED 0xF800
#define COLOR_GREEN 0x07E0
#define COLOR_YELLOW 0xFFE0
#define COLOR_CYAN 0x07FF
#define COLOR_MAGENTA 0xF81F
#define COLOR_BLUE 0x001F
#define COLOR_DARK_GRAY 0x2104
#define COLOR_LIGHT_GRAY 0xC618

// ============= MENU ITEM STRUCTURE =============

struct MenuItem {
  const char* label;           // Display text
  const char* description;     // Detail text
  uint16_t icon;               // Icon emoji/char
  bool enabled;                // Clickable
  void (*callback)(void);      // Action callback

  MenuItem() : label(""), description(""), icon(0), enabled(true), callback(nullptr) {}

  MenuItem(const char* lbl, const char* desc, bool en = true, void(*cb)(void) = nullptr)
    : label(lbl), description(desc), icon(0), enabled(en), callback(cb) {}
};

// ============= ADVANCED MENU CLASS =============

class MenuAdvanced {
public:
  MenuAdvanced();

  // Initialization
  void begin();
  void update();
  void display();

  // Input handling
  void handleTouch(uint16_t x, uint16_t y);
  void handleButton(uint8_t button);

  // Menu navigation
  void selectNextItem();
  void selectPrevItem();
  void selectItem(uint16_t index);
  void executeSelectedItem();

  // Tab management
  void setCurrentTab(uint8_t tabIndex);
  uint8_t getCurrentTab() const { return currentTab; }

  // Item management
  void addMenuItem(const MenuItem& item);
  void clearMenuItems();
  uint16_t getMenuItemCount() const { return menuItems.size(); }
  MenuItem* getMenuItem(uint16_t index);

  // Display helpers
  void drawStatusBar();
  void drawTabBar();
  void drawMenuItems();
  void drawSelectedItemHighlight();
  void drawStatusIndicators();

  // Item rendering
  void drawMenuItemWithSelection(uint16_t itemIndex, uint16_t yPos, bool selected);
  void drawScanResult(const AttackResult* result, uint16_t yPos);
  void drawProgressBar(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t percent);
  void drawRSSIIndicator(int8_t rssi, uint16_t x, uint16_t y);
  void drawBatteryIndicator(uint8_t percent, uint16_t x, uint16_t y);

  // Parameters & Input
  bool promptForInput(const char* prompt, char* output, uint16_t maxLen);
  bool promptForNumber(const char* prompt, uint32_t& value, uint32_t minVal, uint32_t maxVal);
  bool confirmDialog(const char* message);

  // Status messages
  void showStatusMessage(const char* msg, uint16_t durationMs = 2000);
  void showErrorMessage(const char* error);
  void showSuccessMessage(const char* msg);

  // Results visualization
  void displayScanResults(const std::vector<AttackResult*>& results);
  void displayAttackStats(uint16_t successful, uint16_t failed, uint32_t durationMs);

private:
  uint8_t currentTab;
  uint16_t selectedItemIndex;
  std::vector<MenuItem> menuItems;

  uint32_t statusMessageTime;
  char statusMessage[128];
  uint16_t statusMessageColor;

  // Touch state
  uint16_t lastTouchX;
  uint16_t lastTouchY;
  uint32_t lastTouchTime;

  // Drawing state
  uint16_t visibleItemCount;
  uint16_t scrollOffset;
  uint32_t lastDrawTime;

  // Helper methods
  void updateScrollPosition();
  uint16_t getTouchableArea(uint16_t itemIndex) const;
  bool isTouchInArea(uint16_t x, uint16_t y, uint16_t ax, uint16_t ay, uint16_t aw, uint16_t ah) const;
  void clearDrawArea(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color = COLOR_BLACK);
};

// ============= TOAST NOTIFICATION SYSTEM =============

class Toast {
public:
  static void show(const char* message, uint16_t durationMs = 2000, uint16_t color = COLOR_WHITE);
  static void showError(const char* message);
  static void showSuccess(const char* message);
  static void update();
  static void draw();

private:
  static char message[256];
  static uint32_t displayTime;
  static uint32_t durationMs;
  static uint16_t color;
  static bool visible;
};

// ============= PARAMETER INPUT DIALOG =============

class InputDialog {
public:
  static bool show(const char* prompt, char* result, uint16_t maxLen);
  static bool showNumeric(const char* prompt, uint32_t& result, uint32_t minVal, uint32_t maxVal);
  static bool showConfirm(const char* message, const char* yesLabel = "Yes", const char* noLabel = "No");

private:
  static void drawDialog(const char* title, const char* subtitle = "");
  static void drawKeyboard(bool numeric = false);
  static void handleKeyboardInput(char key);
};

// ============= UI STATE MACHINE =============

enum class UIState {
  MENU = 0,           // Main menu display
  EXECUTING = 1,      // Attack running
  RESULTS = 2,        // Showing results
  INPUT = 3,          // Input dialog
  STATUS = 4,         // Status message
  SETTINGS = 5        // Settings submenu
};

class UIStateManager {
public:
  static UIStateManager& getInstance() {
    static UIStateManager instance;
    return instance;
  }

  void setState(UIState newState);
  UIState getState() const { return currentState; }

  void transitionTo(UIState newState);
  void returnToPrevious();

private:
  UIStateManager() : currentState(UIState::MENU) {}
  UIState currentState;
  UIState previousState;
};

#endif // MENU_ADVANCED_H
