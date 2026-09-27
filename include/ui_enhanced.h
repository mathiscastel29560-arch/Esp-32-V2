#pragma once

#include <string>
#include <vector>
#include <cstdint>

// Enhanced UI system with beautiful graphics, animations, and visual feedback
class UIEnhanced {
public:
  // Color palette - Professional & Modern
  enum Color {
    BLACK = 0x0000,
    WHITE = 0xFFFF,
    DARK_GRAY = 0x2104,
    LIGHT_GRAY = 0xC618,
    TIGER_ORANGE = 0xFCA0,    // Tiger stripe orange (#FFC000)
    TIGER_BLACK = 0x0000,     // Intense black
    ELECTRIC_BLUE = 0x047F,   // Electric blue (#00BFFF)
    LIME_GREEN = 0x07E0,      // Lime green (#00FF00)
    CRIMSON_RED = 0xF800,     // Crimson red (#FF0000)
    GOLD = 0xFEE0,            // Gold (#FFFF00)
  };

  // UI Elements
  struct MenuItem {
    std::string title;
    std::string icon;
    std::string description;
    Color color;
    bool isSelected;
    uint8_t level;  // Submenu nesting level
  };

  struct StatusBar {
    int batteryPercent;
    bool wifiConnected;
    bool bleActive;
    std::string currentTime;
    std::string temperature;
  };

  struct ProgressBar {
    uint8_t percentage;
    std::string label;
    Color color;
  };

  // Singleton
  static UIEnhanced& getInstance() {
    static UIEnhanced instance;
    return instance;
  }

  // === BOOT SCREEN ===
  void displayAnimatedBootScreen();
  void displayTigerAnimation();
  void displaySystemLoadingBar();
  void displaySuccessAnimation();

  // === STATUS BAR ===
  void drawStatusBar(const StatusBar& status);
  void drawBatteryIndicator(int percent);
  void drawSignalStrength(int rssi);
  void drawStatusIcons(bool wifi, bool ble, bool recording);

  // === MENU SYSTEM ===
  void displayMenuHeader(const std::string& title, const std::string& icon);
  void displayMenuItem(const MenuItem& item, bool isHighlighted);
  void displayMenuSeparator();
  void displayMenuFooter(const std::string& navigation);

  // === GRAPHICS ===
  void drawProgressBar(const ProgressBar& progress);
  void drawWaveform(const std::vector<uint8_t>& data, const std::string& label);
  void drawSignalMap(const std::vector<int>& rssiValues);
  void drawBatteryGraph(const std::vector<int>& history);
  void drawTemperatureGauge(float temp);

  // === ANIMATIONS ===
  void animateLoading(uint8_t progress);
  void animatePulse(const std::string& text, Color color);
  void animateSlideIn(const std::string& text, Color color);
  void animateSuccess(const std::string& message);
  void animateWarning(const std::string& message);
  void animateError(const std::string& message);

  // === VISUAL ELEMENTS ===
  void drawBox(uint8_t x, uint8_t y, uint8_t width, uint8_t height, Color color);
  void drawRoundedBox(uint8_t x, uint8_t y, uint8_t width, uint8_t height, Color color);
  void drawGradientBar(uint8_t width, Color startColor, Color endColor);
  void drawAsciiArt(const std::vector<std::string>& art, Color color);

  // === THEMES ===
  enum Theme {
    THEME_DARK,      // Dark background (default)
    THEME_LIGHT,     // Light background
    THEME_HACKER,    // Hacker/terminal green
    THEME_MINIMAL,   // Minimal monochrome
  };

  void setTheme(Theme theme);
  Theme getCurrentTheme() const;

  // === TEXT FORMATTING ===
  std::string formatBold(const std::string& text) const;
  std::string formatDim(const std::string& text) const;
  std::string formatColor(const std::string& text, Color color) const;
  std::string formatCentered(const std::string& text, uint8_t width) const;

private:
  UIEnhanced() = default;
  
  Theme currentTheme = THEME_DARK;
  
  // Animation helpers
  void drawAnimationFrame(uint8_t frame, uint8_t totalFrames);
  Color interpolateColor(Color start, Color end, float progress);
};
