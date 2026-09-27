#include "ui_enhanced.h"
#include <Arduino.h>
#include <cstdio>

// ============ BOOT SCREEN ============

void UIEnhanced::displayAnimatedBootScreen() {
  // Clear screen and set dark theme
  setTheme(THEME_DARK);
  Serial.clear();
  delay(300);

  // Display opening sequence
  Serial.println("\n\n");
  
  // Big tiger title with animation
  displayTigerAnimation();
  
  delay(800);
  
  // System loading
  displaySystemLoadingBar();
  
  delay(1000);
  
  // Success animation
  displaySuccessAnimation();
}

void UIEnhanced::displayTigerAnimation() {
  // Terrifying Tiger Animation - Progressive frames with RF/BLE elements

  // Frame 1: Antennae and RF waves appearing
  Serial.println("\n");
  Serial.println("  ╔════════════════════════════════════════╗");
  Serial.println("  ║  |    |    RF HUNTING MODE    |    |  ║");
  Serial.println("  ║  ≈≈≈≈≈≈≈≈≈≈≈≈≈≈≈≈≈≈≈≈≈≈≈≈≈≈≈≈≈  ║");
  Serial.println("  ║                                        ║");
  Serial.println("  ║    🐯 TIGER PROWLING NETWORKS 🐯      ║");
  Serial.println("  ║                                        ║");
  delay(250);

  // Frame 2: Tiger eyes glow with RF waves
  Serial.clear();
  Serial.println("\n");
  Serial.println("  ╔════════════════════════════════════════╗");
  Serial.println("  ║  | ≈≈≈ |  RF SCANNING ACTIVE  | ≈≈≈ |  ║");
  Serial.println("  ║  ≈  ~~~  ≈  NETWORK TARGETING  ≈  ~~~ ≈  ║");
  Serial.println("  ║                                        ║");
  Serial.println("  ║       ╭─────────────────────╮          ║");
  Serial.println("  ║       │  ●  ●  EYES GLOW   │          ║");
  Serial.println("  ║       │ Hunting WiFi/BLE   │          ║");
  Serial.println("  ║       ╰─────────────────────╯          ║");
  Serial.println("  ║                                        ║");
  delay(250);

  // Frame 3: Terrifying face with RF surround
  Serial.clear();
  Serial.println("\n");
  Serial.println("  ╔════════════════════════════════════════╗");
  Serial.println("  ║≈≈≈| THREAT DETECTED - RF LOCKED |≈≈≈║");
  Serial.println("  ║~~~  ~  BLE/WiFi INTERCEPTING  ~  ~~~║");
  Serial.println("  ║                                        ║");
  Serial.println("  ║     ╭────────●●────────╮              ║");
  Serial.println("  ║     │  ●═══  vv  ═══● │  ROARING!    ║");
  Serial.println("  ║     │ |█ FANGS READY █| │              ║");
  Serial.println("  ║     ╰────────────────────╯              ║");
  Serial.println("  ║                                        ║");
  delay(250);

  // Frame 4: Full attack mode with antenna arrays
  Serial.clear();
  Serial.println("\n");
  Serial.println("  ╔════════════════════════════════════════╗");
  Serial.println("  ║  | ≈≈≈ | | SIGNAL JAMMED | | ≈≈≈ |   ║");
  Serial.println("  ║  ≈~~~≈~~~≈  RF DOMINANCE  ≈~~~≈~~~≈   ║");
  Serial.println("  ║  ⚡ ⚡ ⚡ ATTACK ACTIVATED ⚡ ⚡ ⚡   ║");
  Serial.println("  ║                                        ║");
  Serial.println("  ║  ╭──⚡───●●────⚡──╮   433/915/2400   ║");
  Serial.println("  ║  │ ●╱════^^^════╲● │    MHz ACTIVE    ║");
  Serial.println("  ║  │ │STRIPES│SCAN│ │    BLE LOCK-ON   ║");
  Serial.println("  ║  ╰──⚡────────────⚡──╯                ║");
  Serial.println("  ║                                        ║");
  delay(250);

  // Frame 5: Maximum power - RF network infiltration
  Serial.clear();
  Serial.println("\n");
  Serial.println("  ╔════════════════════════════════════════╗");
  Serial.println("  ║ | |||  ≈≈≈≈≈ TIGER IN CONTROL ≈≈≈≈≈ | ║");
  Serial.println("  ║ ≈~~~≈~~~≈~~~  ALL NETWORKS OWNED  ~~~≈ ║");
  Serial.println("  ║ ⚡⚡⚡⚡⚡ HUNT COMMENCING ⚡⚡⚡⚡⚡ ║");
  Serial.println("  ║                                        ║");
  Serial.println("  ║   🐯  ╭─⚡⚡⚡●●●⚡⚡⚡─╮  🐯            ║");
  Serial.println("  ║       │ ╱╲MERCILESS╱╲ │               ║");
  Serial.println("  ║       │├──█████████──┤│ RF PREDATOR    ║");
  Serial.println("  ║       │└─ HUNTING ─┘│ BLE/WiFi TRAPPED║");
  Serial.println("  ║       ╰─────────────────╯               ║");
  Serial.println("  ║                                        ║");
  delay(250);

  // Frame 6: Final - System Online and Dangerous
  Serial.clear();
  Serial.println("\n");
  Serial.println("  ╔════════════════════════════════════════╗");
  Serial.println("  ║ |||  ≈≈≈≈ TIGER AUDIT ACTIVE ≈≈≈≈ ||| ║");
  Serial.println("  ║ ⚡⚡⚡ RF DOMINANCE LOCKED IN ⚡⚡⚡ ║");
  Serial.println("  ║ ●●● ALL THREATS NEUTRALIZED ●●● ║");
  Serial.println("  ║                                        ║");
  Serial.println("  ║  NETWORKS SCANNING + DEVICES MAPPING  ║");
  Serial.println("  ║  RF MONITORING + SECURITY TESTING     ║");
  Serial.println("  ║  THREAT DETECTION + PROTOCOLS ARMED   ║");
  Serial.println("  ║                                        ║");
  Serial.println("  ║  🐯  MERCILESS. UNSTOPPABLE. READY.  🐯 ║");
  Serial.println("  ║                                        ║");
  delay(300);
}

void UIEnhanced::displaySystemLoadingBar() {
  Serial.println("\n  ⚡ Initializing Security Systems:\n");

  const char* stages[] = {
    "⚡ GPIO & Button Driver",
    "⚡ I2C: RTC & NFC Reader",
    "⚡ SPI: RF Modules (CC1101 + NRF24)",
    "⚡ Wireless: WiFi & BLE Stack",
    "⚡ Security Database & Signatures",
    "⚡ Menu System & Display"
  };

  for (int i = 0; i < 6; i++) {
    delay(150);
    Serial.printf("    ✓ %s\n", stages[i]);
  }

  Serial.println("");
}

void UIEnhanced::displaySuccessAnimation() {
  delay(400);
  Serial.clear();
  Serial.println("\n");

  // Loading bar fills
  Serial.println("  ┌──────────────────────────────────────┐");
  Serial.println("  │ ████████░░░░░░░░░░░░░░░░░░░░░░░░░░ │  25%");
  delay(100);
  Serial.println("  │ ████████████████░░░░░░░░░░░░░░░░░░░ │  50%");
  delay(100);
  Serial.println("  │ ████████████████████████░░░░░░░░░░░ │  75%");
  delay(100);
  Serial.println("  │ ██████████████████████████████████░░ │  95%");
  delay(100);
  Serial.println("  │ ████████████████████████████████████ │ 100%");
  Serial.println("  └──────────────────────────────────────┘");

  delay(500);

  // Success screen
  Serial.println("\n");
  Serial.println("  ╔════════════════════════════════════════╗");
  Serial.println("  ║                                        ║");
  Serial.println("  ║          ✨ SYSTEM READY ✨            ║");
  Serial.println("  ║                                        ║");
  Serial.println("  ║  🐯 Tiger Roaring and Ready to Hunt 🐯 ║");
  Serial.println("  ║                                        ║");
  Serial.println("  ║  All Systems Online • Hardware Ready   ║");
  Serial.println("  ║  RF Modules • Security Tools • Loaded  ║");
  Serial.println("  ║                                        ║");
  Serial.println("  ║    ⚡ Press START to begin auditing ⚡  ║");
  Serial.println("  ║                                        ║");
  Serial.println("  ╚════════════════════════════════════════╝\n");

  delay(600);
}

// ============ STATUS BAR ============

void UIEnhanced::drawStatusBar(const StatusBar& status) {
  Serial.print("╔");
  for (int i = 0; i < 40; i++) Serial.print("═");
  Serial.println("╗");
  
  // Battery icon
  drawBatteryIndicator(status.batteryPercent);
  Serial.print("  ");
  
  // WiFi status
  if (status.wifiConnected) {
    Serial.print("📡 WiFi");
  } else {
    Serial.print("❌ WiFi");
  }
  Serial.print("  ");
  
  // BLE status
  if (status.bleActive) {
    Serial.print("🔵 BLE");
  } else {
    Serial.print("⭕ BLE");
  }
  
  // Time
  Serial.printf("  🕐 %s\n", status.currentTime.c_str());
  
  Serial.print("╚");
  for (int i = 0; i < 40; i++) Serial.print("═");
  Serial.println("╝");
}

void UIEnhanced::drawBatteryIndicator(int percent) {
  Serial.print("🔋 [");
  
  int filled = percent / 10;
  for (int i = 0; i < 10; i++) {
    if (i < filled) {
      if (percent >= 50) Serial.print("█");        // Green
      else if (percent >= 20) Serial.print("▓");   // Yellow
      else Serial.print("░");                       // Red
    } else {
      Serial.print("░");
    }
  }
  
  Serial.printf("] %d%%", percent);
}

void UIEnhanced::drawSignalStrength(int rssi) {
  Serial.print("📶 Signal: [");
  
  // Convert RSSI to bars (0-4)
  int bars = 0;
  if (rssi > -50) bars = 4;
  else if (rssi > -60) bars = 3;
  else if (rssi > -70) bars = 2;
  else if (rssi > -80) bars = 1;
  
  for (int i = 0; i < 4; i++) {
    if (i < bars) Serial.print("▂");
    else Serial.print("▁");
  }
  
  Serial.printf("] %d dBm", rssi);
}

// ============ MENU SYSTEM ============

void UIEnhanced::displayMenuHeader(const std::string& title, const std::string& icon) {
  Serial.println("\n");
  Serial.println("╔════════════════════════════════════════╗");
  Serial.printf("║  %s  %-30s║\n", icon.c_str(), title.c_str());
  Serial.println("╠════════════════════════════════════════╣");
}

void UIEnhanced::displayMenuItem(const MenuItem& item, bool isHighlighted) {
  if (isHighlighted) {
    Serial.print("║ ▶ ");  // Highlighted
  } else {
    Serial.print("║   ");  // Not highlighted
  }
  
  Serial.printf("%s %s\n", item.icon.c_str(), item.title.c_str());
}

void UIEnhanced::displayMenuSeparator() {
  Serial.println("╠════════════════════════════════════════╣");
}

void UIEnhanced::displayMenuFooter(const std::string& navigation) {
  Serial.println("╚════════════════════════════════════════╝");
  Serial.printf("\n  %s\n\n", navigation.c_str());
}

// ============ GRAPHICS ============

void UIEnhanced::drawProgressBar(const ProgressBar& progress) {
  Serial.printf("  %s: [", progress.label.c_str());
  
  int filled = progress.percentage / 5;
  for (int i = 0; i < 20; i++) {
    if (i < filled) Serial.print("█");
    else Serial.print("░");
  }
  
  Serial.printf("] %u%%\n", progress.percentage);
}

void UIEnhanced::drawWaveform(const std::vector<uint8_t>& data, const std::string& label) {
  Serial.printf("  📊 %s:\n  ", label.c_str());
  
  for (const auto& value : data) {
    // Simple ASCII waveform
    uint8_t height = value / 25;  // Scale to 0-10
    if (height == 0) Serial.print("▁");
    else if (height <= 2) Serial.print("▂");
    else if (height <= 4) Serial.print("▃");
    else if (height <= 6) Serial.print("▄");
    else if (height <= 8) Serial.print("▅");
    else Serial.print("█");
  }
  Serial.println();
}

void UIEnhanced::drawSignalMap(const std::vector<int>& rssiValues) {
  Serial.println("  📡 Signal Map:");
  
  for (size_t i = 0; i < rssiValues.size(); i++) {
    int rssi = rssiValues[i];
    int bars = (rssi + 100) / 10;  // Scale to 0-10
    if (bars < 0) bars = 0;
    if (bars > 10) bars = 10;
    
    Serial.printf("    Channel %d: [", i + 1);
    for (int j = 0; j < 10; j++) {
      if (j < bars) Serial.print("▆");
      else Serial.print("░");
    }
    Serial.printf("] %d dBm\n", rssi);
  }
}

void UIEnhanced::drawBatteryGraph(const std::vector<int>& history) {
  Serial.println("  🔋 Battery History:");
  
  for (int i = 0; i < history.size(); i++) {
    int percent = history[i];
    Serial.printf("    ");
    
    for (int j = 0; j < percent / 5; j++) {
      Serial.print("█");
    }
    
    Serial.printf(" %d%%\n", percent);
  }
}

void UIEnhanced::drawTemperatureGauge(float temp) {
  Serial.printf("  🌡️  Temperature: ");
  
  if (temp < 20) {
    Serial.print("🔵 COLD   ");
  } else if (temp < 35) {
    Serial.print("🟢 NORMAL ");
  } else if (temp < 50) {
    Serial.print("🟡 WARM   ");
  } else {
    Serial.print("🔴 HOT    ");
  }
  
  Serial.printf("%.1f°C\n", temp);
}

// ============ ANIMATIONS ============

void UIEnhanced::animateLoading(uint8_t progress) {
  static const char* spinner[] = {"⠋", "⠙", "⠹", "⠸", "⠼", "⠴", "⠦", "⠧", "⠇", "⠏"};
  Serial.printf("\r  %s Loading... %u%%", spinner[progress % 10], progress);
  Serial.flush();
}

void UIEnhanced::animatePulse(const std::string& text, Color color) {
  for (int i = 0; i < 3; i++) {
    Serial.printf("\r  ✨ %s ✨      ", text.c_str());
    delay(200);
    Serial.printf("\r  %s           ", text.c_str());
    delay(200);
  }
  Serial.println();
}

void UIEnhanced::animateSlideIn(const std::string& text, Color color) {
  for (size_t i = 0; i <= text.length(); i++) {
    Serial.printf("\r  > %s", text.substr(0, i).c_str());
    delay(30);
  }
  Serial.println();
}

void UIEnhanced::animateSuccess(const std::string& message) {
  Serial.println("\n  ╭─────────────────────────────────────────╮");
  Serial.printf("  │  ✅ %s\n", message.c_str());
  Serial.println("  ╰─────────────────────────────────────────╯\n");
  delay(800);
}

void UIEnhanced::animateWarning(const std::string& message) {
  Serial.println("\n  ╭─────────────────────────────────────────╮");
  Serial.printf("  │  ⚠️  %s\n", message.c_str());
  Serial.println("  ╰─────────────────────────────────────────╯\n");
  delay(800);
}

void UIEnhanced::animateError(const std::string& message) {
  Serial.println("\n  ╭─────────────────────────────────────────╮");
  Serial.printf("  │  ❌ %s\n", message.c_str());
  Serial.println("  ╰─────────────────────────────────────────╯\n");
  delay(1000);
}

// ============ VISUAL ELEMENTS ============

void UIEnhanced::drawBox(uint8_t width, uint8_t height, Color color) {
  // Draw simple bordered box
  Serial.print("  ╭");
  for (int i = 0; i < width; i++) Serial.print("─");
  Serial.println("╮");
  
  for (int i = 0; i < height; i++) {
    Serial.print("  │");
    for (int j = 0; j < width; j++) Serial.print(" ");
    Serial.println("│");
  }
  
  Serial.print("  ╰");
  for (int i = 0; i < width; i++) Serial.print("─");
  Serial.println("╯");
}

void UIEnhanced::drawRoundedBox(uint8_t width, uint8_t height, Color color) {
  drawBox(width, height, color);
}

void UIEnhanced::setTheme(Theme theme) {
  currentTheme = theme;
}

Theme UIEnhanced::getCurrentTheme() const {
  return currentTheme;
}

std::string UIEnhanced::formatBold(const std::string& text) const {
  return "\033[1m" + text + "\033[0m";
}

std::string UIEnhanced::formatDim(const std::string& text) const {
  return "\033[2m" + text + "\033[0m";
}

std::string UIEnhanced::formatColor(const std::string& text, Color color) const {
  // ANSI color codes for terminal
  return text;  // Placeholder
}

std::string UIEnhanced::formatCentered(const std::string& text, uint8_t width) const {
  int padding = (width - text.length()) / 2;
  std::string result;
  for (int i = 0; i < padding; i++) result += " ";
  result += text;
  return result;
}
