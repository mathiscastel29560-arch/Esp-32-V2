#include "menu_advanced.h"
#include "drivers.h"
#include "logging_system.h"

// ============= MENU ADVANCED IMPLEMENTATION =============

MenuAdvanced::MenuAdvanced()
  : currentTab(0), selectedItemIndex(0), statusMessageTime(0),
    lastTouchX(0), lastTouchY(0), lastTouchTime(0),
    visibleItemCount(4), scrollOffset(0), lastDrawTime(0) {
  memset(statusMessage, 0, 128);
  statusMessageColor = COLOR_WHITE;
}

void MenuAdvanced::begin() {
  Logger::getInstance().info("UI", "MenuAdvanced initialized");
}

void MenuAdvanced::update() {
  // Update status message visibility
  if (statusMessageTime > 0) {
    if (millis() - statusMessageTime > 2000) {
      statusMessageTime = 0;
      memset(statusMessage, 0, 128);
    }
  }

  // Update display if needed
  uint32_t now = millis();
  if (now - lastDrawTime > 50) {
    display();
    lastDrawTime = now;
  }
}

void MenuAdvanced::display() {
  // Draw main UI components
  drawStatusBar();
  drawTabBar();
  drawMenuItems();
  drawStatusIndicators();

  if (statusMessageTime > 0) {
    showStatusMessage(statusMessage, 0);
  }
}

void MenuAdvanced::handleTouch(uint16_t x, uint16_t y) {
  lastTouchX = x;
  lastTouchY = y;
  lastTouchTime = millis();

  // Check if touch in tab bar area
  if (y < STATUS_BAR_HEIGHT + TAB_HEIGHT) {
    uint16_t touchArea = x / (DISPLAY_WIDTH / 4);
    setCurrentTab(touchArea);
    return;
  }

  // Check if touch in menu items area
  uint16_t menuStartY = STATUS_BAR_HEIGHT + TAB_HEIGHT + PADDING;
  uint16_t itemY = 0;

  for (uint16_t i = scrollOffset; i < menuItems.size() && i < scrollOffset + visibleItemCount; i++) {
    if (y >= menuStartY + itemY && y < menuStartY + itemY + MENU_ITEM_HEIGHT) {
      selectItem(i);
      if (millis() - lastTouchTime < 300) {
        executeSelectedItem();
      }
      return;
    }
    itemY += MENU_ITEM_HEIGHT;
  }
}

void MenuAdvanced::handleButton(uint8_t button) {
  switch (button) {
    case 1: // UP
      selectPrevItem();
      break;
    case 2: // DOWN
      selectNextItem();
      break;
    case 6: // SELECT
      executeSelectedItem();
      break;
    case 42: // BACK
      // Handle back navigation
      break;
  }
}

void MenuAdvanced::selectNextItem() {
  if (selectedItemIndex < menuItems.size() - 1) {
    selectedItemIndex++;
    updateScrollPosition();
  }
}

void MenuAdvanced::selectPrevItem() {
  if (selectedItemIndex > 0) {
    selectedItemIndex--;
    updateScrollPosition();
  }
}

void MenuAdvanced::selectItem(uint16_t index) {
  if (index < menuItems.size()) {
    selectedItemIndex = index;
    updateScrollPosition();
  }
}

void MenuAdvanced::executeSelectedItem() {
  if (selectedItemIndex < menuItems.size()) {
    MenuItem* item = &menuItems[selectedItemIndex];
    if (item->enabled && item->callback) {
      item->callback();
    }
  }
}

void MenuAdvanced::setCurrentTab(uint8_t tabIndex) {
  currentTab = tabIndex;
  selectedItemIndex = 0;
  scrollOffset = 0;
  clearMenuItems();
  Logger::getInstance().info("UI", "Tab changed");
}

void MenuAdvanced::addMenuItem(const MenuItem& item) {
  menuItems.push_back(item);
}

void MenuAdvanced::clearMenuItems() {
  menuItems.clear();
  selectedItemIndex = 0;
  scrollOffset = 0;
}

MenuItem* MenuAdvanced::getMenuItem(uint16_t index) {
  if (index < menuItems.size()) {
    return &menuItems[index];
  }
  return nullptr;
}

void MenuAdvanced::drawStatusBar() {
  // Draw dark background for status bar
  Drivers::Display::fillRect(0, 0, DISPLAY_WIDTH, STATUS_BAR_HEIGHT, COLOR_DARK_GRAY);

  // Draw battery indicator (placeholder)
  Drivers::Display::drawString(10, 8, "🔋 100%", COLOR_GREEN);

  // Draw WiFi indicator (placeholder)
  Drivers::Display::drawString(100, 8, "📡 ON", COLOR_GREEN);

  // Draw time (placeholder)
  Drivers::Display::drawString(200, 8, "14:32", COLOR_WHITE);

  // Draw border
  Drivers::Display::drawRect(0, STATUS_BAR_HEIGHT - 1, DISPLAY_WIDTH, 1, COLOR_LIGHT_GRAY);
}

void MenuAdvanced::drawTabBar() {
  uint16_t tabWidth = DISPLAY_WIDTH / 4;
  const char* tabs[] = {"WiFi", "BLE", "RF", "IoT"};

  for (uint8_t i = 0; i < 4; i++) {
    uint16_t tabX = i * tabWidth;
    uint16_t tabY = STATUS_BAR_HEIGHT;

    // Background
    uint16_t bgColor = (i == currentTab) ? COLOR_CYAN : COLOR_DARK_GRAY;
    Drivers::Display::fillRect(tabX, tabY, tabWidth, TAB_HEIGHT, bgColor);

    // Text
    uint16_t textColor = (i == currentTab) ? COLOR_BLACK : COLOR_WHITE;
    Drivers::Display::drawString(tabX + 5, tabY + 10, tabs[i], textColor);

    // Border
    Drivers::Display::drawRect(tabX, tabY + TAB_HEIGHT - 1, tabWidth, 1, COLOR_LIGHT_GRAY);
  }
}

void MenuAdvanced::drawMenuItems() {
  uint16_t menuStartY = STATUS_BAR_HEIGHT + TAB_HEIGHT + PADDING;
  uint16_t itemY = 0;

  for (uint16_t i = scrollOffset; i < menuItems.size() && i < scrollOffset + visibleItemCount; i++) {
    drawMenuItemWithSelection(i, menuStartY + itemY, i == selectedItemIndex);
    itemY += MENU_ITEM_HEIGHT;
  }
}

void MenuAdvanced::drawSelectedItemHighlight() {
  // Highlight handled in drawMenuItemWithSelection
}

void MenuAdvanced::drawStatusIndicators() {
  // Optional: Draw additional status info at bottom
}

void MenuAdvanced::drawMenuItemWithSelection(uint16_t itemIndex, uint16_t yPos, bool selected) {
  if (itemIndex >= menuItems.size()) return;

  MenuItem* item = &menuItems[itemIndex];
  uint16_t bgColor = selected ? COLOR_CYAN : COLOR_BLACK;
  uint16_t textColor = selected ? COLOR_BLACK : COLOR_WHITE;

  // Draw background
  Drivers::Display::fillRect(PADDING, yPos, DISPLAY_WIDTH - 2 * PADDING, MENU_ITEM_HEIGHT, bgColor);

  // Draw label
  Drivers::Display::drawString(PADDING + 10, yPos + 5, item->label, textColor);

  // Draw description if selected
  if (selected && item->description) {
    Drivers::Display::drawString(PADDING + 10, yPos + 20, item->description, textColor);
  }

  // Draw border
  if (selected) {
    Drivers::Display::drawRect(PADDING, yPos, DISPLAY_WIDTH - 2 * PADDING, MENU_ITEM_HEIGHT, COLOR_GREEN);
  }
}

void MenuAdvanced::drawScanResult(const AttackResult* result, uint16_t yPos) {
  if (!result) return;

  ResultsFormatter& formatter = ResultsFormatter::getInstance();
  const char* formatted = formatter.format(result);

  uint16_t textColor = (result->status == AttackStatus::SUCCESS) ? COLOR_GREEN :
                       (result->status == AttackStatus::ERROR) ? COLOR_RED : COLOR_YELLOW;

  Drivers::Display::drawString(PADDING, yPos, formatted, textColor);
}

void MenuAdvanced::drawProgressBar(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t percent) {
  // Draw background
  Drivers::Display::drawRect(x, y, width, height, COLOR_WHITE);

  // Draw filled portion
  uint16_t filledWidth = (width - 2) * percent / 100;
  Drivers::Display::fillRect(x + 1, y + 1, filledWidth, height - 2, COLOR_GREEN);

  // Draw percentage text
  char percentStr[16];
  snprintf(percentStr, 15, "%u%%", percent);
  Drivers::Display::drawString(x + width + 5, y, percentStr, COLOR_WHITE);
}

void MenuAdvanced::drawRSSIIndicator(int8_t rssi, uint16_t x, uint16_t y) {
  ResultsFormatter& formatter = ResultsFormatter::getInstance();
  const char* rssiStr = formatter.formatRSSI(rssi);

  uint16_t color = COLOR_WHITE;
  if (rssi >= -60) color = COLOR_GREEN;
  else if (rssi >= -80) color = COLOR_YELLOW;
  else color = COLOR_RED;

  Drivers::Display::drawString(x, y, rssiStr, color);
}

void MenuAdvanced::drawBatteryIndicator(uint8_t percent, uint16_t x, uint16_t y) {
  char batteryStr[32];
  const char* icon = (percent > 75) ? "🔋" : (percent > 50) ? "🔋" : (percent > 25) ? "⚠" : "🪫";
  snprintf(batteryStr, 31, "%s %u%%", icon, percent);

  uint16_t color = (percent > 75) ? COLOR_GREEN : (percent > 25) ? COLOR_YELLOW : COLOR_RED;
  Drivers::Display::drawString(x, y, batteryStr, color);
}

bool MenuAdvanced::promptForInput(const char* prompt, char* output, uint16_t maxLen) {
  return InputDialog::show(prompt, output, maxLen);
}

bool MenuAdvanced::promptForNumber(const char* prompt, uint32_t& value, uint32_t minVal, uint32_t maxVal) {
  return InputDialog::showNumeric(prompt, value, minVal, maxVal);
}

bool MenuAdvanced::confirmDialog(const char* message) {
  return InputDialog::showConfirm(message);
}

void MenuAdvanced::showStatusMessage(const char* msg, uint16_t durationMs) {
  strncpy(statusMessage, msg, 127);
  statusMessage[127] = '\0';
  statusMessageTime = millis();

  if (durationMs > 0) {
    // Draw centered message
    uint16_t msgLen = strlen(msg) * 6;
    uint16_t x = (DISPLAY_WIDTH - msgLen) / 2;
    uint16_t y = DISPLAY_HEIGHT - 40;

    Drivers::Display::fillRect(x - 10, y - 10, msgLen + 20, 30, COLOR_DARK_GRAY);
    Drivers::Display::drawString(x, y, msg, COLOR_WHITE);
  }
}

void MenuAdvanced::showErrorMessage(const char* error) {
  statusMessageColor = COLOR_RED;
  showStatusMessage(error, 3000);
  Logger::getInstance().error("UI", error);
}

void MenuAdvanced::showSuccessMessage(const char* msg) {
  statusMessageColor = COLOR_GREEN;
  showStatusMessage(msg, 2000);
  Logger::getInstance().info("UI", msg);
}

void MenuAdvanced::displayScanResults(const std::vector<AttackResult*>& results) {
  uint16_t yPos = STATUS_BAR_HEIGHT + TAB_HEIGHT + PADDING;

  for (const auto* result : results) {
    if (result) {
      drawScanResult(result, yPos);
      yPos += MENU_ITEM_HEIGHT;
    }
  }
}

void MenuAdvanced::displayAttackStats(uint16_t successful, uint16_t failed, uint32_t durationMs) {
  char statsStr[128];
  snprintf(statsStr, 127, "✓ %u  ✗ %u  (%ums)", successful, failed, durationMs);

  Drivers::Display::drawString(PADDING, DISPLAY_HEIGHT - 30, statsStr, COLOR_CYAN);
}

void MenuAdvanced::updateScrollPosition() {
  if (selectedItemIndex < scrollOffset) {
    scrollOffset = selectedItemIndex;
  } else if (selectedItemIndex >= scrollOffset + visibleItemCount) {
    scrollOffset = selectedItemIndex - visibleItemCount + 1;
  }
}

uint16_t MenuAdvanced::getTouchableArea(uint16_t itemIndex) const {
  return STATUS_BAR_HEIGHT + TAB_HEIGHT + PADDING + (itemIndex - scrollOffset) * MENU_ITEM_HEIGHT;
}

bool MenuAdvanced::isTouchInArea(uint16_t x, uint16_t y, uint16_t ax, uint16_t ay, uint16_t aw, uint16_t ah) const {
  return (x >= ax && x < ax + aw && y >= ay && y < ay + ah);
}

void MenuAdvanced::clearDrawArea(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
  Drivers::Display::fillRect(x, y, w, h, color);
}

// ============= TOAST NOTIFICATION IMPLEMENTATION =============

char Toast::message[256] = "";
uint32_t Toast::displayTime = 0;
uint32_t Toast::durationMs = 0;
uint16_t Toast::color = COLOR_WHITE;
bool Toast::visible = false;

void Toast::show(const char* message, uint16_t durationMs, uint16_t color) {
  strncpy(Toast::message, message, 255);
  Toast::message[255] = '\0';
  Toast::displayTime = millis();
  Toast::durationMs = durationMs;
  Toast::color = color;
  Toast::visible = true;
}

void Toast::showError(const char* message) {
  show(message, 2000, COLOR_RED);
}

void Toast::showSuccess(const char* message) {
  show(message, 2000, COLOR_GREEN);
}

void Toast::update() {
  if (visible && millis() - displayTime > durationMs) {
    visible = false;
  }
}

void Toast::draw() {
  if (!visible) return;

  uint16_t msgLen = strlen(message) * 6;
  uint16_t x = (DISPLAY_WIDTH - msgLen) / 2;
  uint16_t y = DISPLAY_HEIGHT - 50;

  Drivers::Display::fillRect(x - 10, y - 10, msgLen + 20, 30, COLOR_DARK_GRAY);
  Drivers::Display::drawString(x, y, message, color);
}

// ============= INPUT DIALOG IMPLEMENTATION =============

bool InputDialog::show(const char* prompt, char* result, uint16_t maxLen) {
  // Draw dialog background
  drawDialog(prompt);

  // TODO: Implement keyboard input handling
  // This is a placeholder - full implementation would handle touch/button input

  return true;
}

bool InputDialog::showNumeric(const char* prompt, uint32_t& result, uint32_t minVal, uint32_t maxVal) {
  // Draw numeric input dialog
  drawDialog(prompt, "Enter number");
  drawKeyboard(true);

  // TODO: Implement numeric input
  return true;
}

bool InputDialog::showConfirm(const char* message, const char* yesLabel, const char* noLabel) {
  // Draw confirmation dialog
  drawDialog(message);

  // Draw buttons
  uint16_t btnY = DISPLAY_HEIGHT - 50;
  Drivers::Display::drawString(50, btnY, yesLabel, COLOR_GREEN);
  Drivers::Display::drawString(250, btnY, noLabel, COLOR_RED);

  // TODO: Wait for button input
  return true;
}

void InputDialog::drawDialog(const char* title, const char* subtitle) {
  uint16_t dialogWidth = 280;
  uint16_t dialogHeight = 150;
  uint16_t x = (DISPLAY_WIDTH - dialogWidth) / 2;
  uint16_t y = (DISPLAY_HEIGHT - dialogHeight) / 2;

  // Draw background
  Drivers::Display::fillRect(x, y, dialogWidth, dialogHeight, COLOR_DARK_GRAY);

  // Draw border
  Drivers::Display::drawRect(x, y, dialogWidth, dialogHeight, COLOR_CYAN);

  // Draw title
  Drivers::Display::drawString(x + 20, y + 20, title, COLOR_WHITE);

  // Draw subtitle if present
  if (subtitle) {
    Drivers::Display::drawString(x + 20, y + 50, subtitle, COLOR_LIGHT_GRAY);
  }
}

void InputDialog::drawKeyboard(bool numeric) {
  // Draw keyboard layout
  // TODO: Full keyboard rendering
}

void InputDialog::handleKeyboardInput(char key) {
  // TODO: Handle keyboard input
}

// ============= UI STATE MANAGER IMPLEMENTATION =============

void UIStateManager::setState(UIState newState) {
  previousState = currentState;
  currentState = newState;
}

void UIStateManager::transitionTo(UIState newState) {
  setState(newState);
}

void UIStateManager::returnToPrevious() {
  currentState = previousState;
}
