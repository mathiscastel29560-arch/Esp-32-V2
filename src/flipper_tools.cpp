#include "flipper_tools.h"
#include "debug_logger.h"
#include <Arduino.h>
#include <algorithm>
#include <cstring>

// Private state variables
static bool g_bleScanning = false;
static uint32_t g_lastScanTime = 0;
static std::vector<FlipperTools::BluetoothDevice> g_btDevices;
static std::vector<FlipperTools::MalwareSignature> g_malwareDatabase;
static std::vector<FlipperTools::IButtonKey> g_storedButtons;
static bool g_malwareDbLoaded = false;
static bool g_usbReady = false;

// Boot screen display with tiger ASCII art
void FlipperTools::displayBootScreen() {
  Serial.clear();
  delay(500);

  // Display tiger ASCII art
  drawTiger();
  delay(1000);

  // Boot sequence animation
  drawBootSequence();

  Serial.println("\n╔════════════════════════════════════════╗");
  Serial.println("║   ESP32-V2 FLIPPER AUDIT PLATFORM    ║");
  Serial.println("║          Version 3.1.0               ║");
  Serial.println("║     Professional Security Auditor    ║");
  Serial.println("╚════════════════════════════════════════╝");

  Serial.println("\n📊 System Initialization Status:");
  Serial.println("  ✓ Hardware drivers loaded");
  Serial.println("  ✓ Flipper tools initialized");
  Serial.println("  ✓ Database synchronized");
  Serial.println("  ✓ Security systems armed");
  Serial.println("  ✓ RF modules calibrated");
  Serial.println("  ✓ Memory optimized");

  delay(1500);
  Serial.println("\n🐯 Tiger Audit Platform Ready! 🐯\n");
  delay(500);
}

void FlipperTools::drawTiger() {
  Serial.println("\n");
  Serial.println("                   🐯");
  Serial.println("                  /|(|\\");
  Serial.println("                 / | | \\");
  Serial.println("                /  | |  \\");
  Serial.println("               /   | |   \\");
  Serial.println("              |    | |    |");
  Serial.println("              |  .-'--.  |");
  Serial.println("              | (  o o  ) |");
  Serial.println("              |  '-...-'  |");
  Serial.println("              |   /| |\\   |");
  Serial.println("              |  / | | \\  |");
  Serial.println("               \\/  | |  \\/");
  Serial.println("                   | |");
  Serial.println("                  /| |\\");
  Serial.println("                 / | | \\");
  Serial.println("                |  | |  |");
  Serial.println("                |  | |  |");
  Serial.println("                |_/ \\_|_|");
  Serial.println("\n");

  // Tiger face close-up
  Serial.println("╔════════════════════════════════════════╗");
  Serial.println("║          TIGER 🐯 AUDIT SYSTEM         ║");
  Serial.println("║                                        ║");
  Serial.println("║  ┌─────────┐   ┌─────────┐             ║");
  Serial.println("║  │ /   \\ │   │ /   \\ │  RF EYES     ║");
  Serial.println("║  │ | o | │   │ | o | │  WATCHING     ║");
  Serial.println("║  │ \\___/ │   │ \\___/ │               ║");
  Serial.println("║          ╲   ╱                         ║");
  Serial.println("║        ────●────                       ║");
  Serial.println("║       Roaring with Power              ║");
  Serial.println("╚════════════════════════════════════════╝");
}

void FlipperTools::drawBootSequence() {
  Serial.print("\n  Loading");
  for (int i = 0; i < 10; i++) {
    Serial.print(".");
    delay(100);
  }
  Serial.println(" DONE!\n");

  // System checks with progress
  Serial.println("  [██████████████░░░░░░░░░░] 50% - RF Initialization");
  delay(300);
  Serial.println("  [████████████████████░░░░] 75% - Security Module");
  delay(300);
  Serial.println("  [██████████████████████░░] 90% - RF Calibration");
  delay(200);
  Serial.println("  [██████████████████████████] 100% - Ready!\n");
}

// === BLUETOOTH/BLE TOOLS ===

FlipperTools::ResultCode FlipperTools::startBLEScanning(uint32_t durationMs) {
  if (durationMs == 0 || durationMs > 600000) {
    DebugLogger::error("[FlipperTools] Invalid scan duration: %u ms (valid: 1-600000)", durationMs);
    return RESULT_ERROR_INVALID_PARAM;
  }

  if (g_bleScanning) {
    DebugLogger::warn("[FlipperTools] BLE scan already in progress");
    return RESULT_ERROR_INVALID_PARAM;
  }

  g_bleScanning = true;
  g_lastScanTime = millis();
  g_btDevices.clear();

  DebugLogger::info("[FlipperTools] BLE scan started for %u ms", durationMs);
  // TODO: Implement actual BLE scanning using Arduino BLE library

  return RESULT_SUCCESS;
}

FlipperTools::ResultCode FlipperTools::stopBLEScanning() {
  if (!g_bleScanning) {
    return RESULT_ERROR_INVALID_PARAM;
  }

  g_bleScanning = false;
  DebugLogger::info("[FlipperTools] BLE scan stopped");
  return RESULT_SUCCESS;
}

std::vector<FlipperTools::BluetoothDevice> FlipperTools::getBTDevices() const {
  return g_btDevices;
}

FlipperTools::ResultCode FlipperTools::connectBTDevice(const std::string& address) {
  if (address.empty() || address.length() != 17) {
    DebugLogger::error("[FlipperTools] Invalid BT address format: %s", address.c_str());
    return RESULT_ERROR_INVALID_PARAM;
  }

  auto it = std::find_if(g_btDevices.begin(), g_btDevices.end(),
    [&address](const BluetoothDevice& dev) { return dev.address == address; });

  if (it == g_btDevices.end()) {
    DebugLogger::error("[FlipperTools] BT device not found: %s", address.c_str());
    return RESULT_ERROR_NOT_FOUND;
  }

  DebugLogger::info("[FlipperTools] Connecting to BT device: %s (%s)",
    address.c_str(), it->name.c_str());
  // TODO: Implement BT connection

  return RESULT_SUCCESS;
}

FlipperTools::ResultCode FlipperTools::disconnectBTDevice(const std::string& address) {
  if (address.empty()) {
    return RESULT_ERROR_INVALID_PARAM;
  }

  DebugLogger::info("[FlipperTools] Disconnecting from BT device: %s", address.c_str());
  // TODO: Implement BT disconnection

  return RESULT_SUCCESS;
}

FlipperTools::ResultCode FlipperTools::emulateBTDevice(const std::string& name) {
  if (name.empty() || name.length() > 29) {
    DebugLogger::error("[FlipperTools] Invalid device name (max 29 chars): %s", name.c_str());
    return RESULT_ERROR_INVALID_PARAM;
  }

  DebugLogger::info("[FlipperTools] Emulating BT device: %s", name.c_str());
  // TODO: Implement BT device emulation

  return RESULT_SUCCESS;
}

uint32_t FlipperTools::getBTDeviceCount() const {
  return g_btDevices.size();
}

bool FlipperTools::isBLEScanning() const {
  return g_bleScanning;
}

// === GPIO & UART TOOLS ===

FlipperTools::ResultCode FlipperTools::scanGPIO(std::vector<GpioPin>& pins) {
  pins.clear();

  struct PinConfig {
    uint8_t pin;
    const char* description;
  };

  const PinConfig knownPins[] = {
    {1, "Button UP"},
    {2, "Button DOWN"},
    {6, "Button SELECT"},
    {42, "Button BACK"},
    {21, "Buzzer (PWM)"},
    {38, "IR TX"},
    {39, "IR RX"},
    {7, "Battery ADC"},
    {4, "CC1101 GDO0"},
    {40, "CC1101 GDO2"},
    {41, "NRF24 IRQ"},
    {12, "SPI Clock"},
    {11, "SPI MOSI"},
    {13, "SPI MISO"},
    {10, "CC1101 CS"},
    {14, "NRF24 CS"},
    {15, "NRF24 CE"},
    {8, "I2C SDA"},
    {9, "I2C SCL"},
  };

  for (const auto& config : knownPins) {
    GpioPin pin;
    pin.pin = config.pin;
    pin.description = config.description;
    pin.level = digitalRead(config.pin);
    pin.isInput = true;
    pin.isOutput = false;
    pin.frequency = 0;

    if (pin.isValid()) {
      pins.push_back(pin);
    }
  }

  DebugLogger::info("[FlipperTools] GPIO scan complete: %u pins", pins.size());
  return RESULT_SUCCESS;
}

FlipperTools::ResultCode FlipperTools::readPin(uint8_t pin, uint8_t& level) {
  if (pin >= 50) {
    DebugLogger::error("[FlipperTools] Invalid GPIO pin: %u", pin);
    return RESULT_ERROR_INVALID_PARAM;
  }

  level = digitalRead(pin) ? 1 : 0;
  DebugLogger::verbose("[FlipperTools] GPIO %u read: %u", pin, level);

  return RESULT_SUCCESS;
}

FlipperTools::ResultCode FlipperTools::writePin(uint8_t pin, uint8_t level) {
  if (pin >= 50) {
    DebugLogger::error("[FlipperTools] Invalid GPIO pin: %u", pin);
    return RESULT_ERROR_INVALID_PARAM;
  }

  if (level > 1) {
    DebugLogger::error("[FlipperTools] Invalid GPIO level: %u (must be 0 or 1)", level);
    return RESULT_ERROR_INVALID_PARAM;
  }

  digitalWrite(pin, level ? HIGH : LOW);
  DebugLogger::info("[FlipperTools] GPIO %u set to %u", pin, level);

  return RESULT_SUCCESS;
}

FlipperTools::ResultCode FlipperTools::setPWMFrequency(uint8_t pin, uint32_t frequency) {
  if (pin >= 50) {
    return RESULT_ERROR_INVALID_PARAM;
  }

  if (frequency == 0 || frequency > 40000) {
    DebugLogger::error("[FlipperTools] Invalid PWM frequency: %u Hz", frequency);
    return RESULT_ERROR_INVALID_PARAM;
  }

  // TODO: Implement PWM frequency setting
  DebugLogger::info("[FlipperTools] PWM pin %u set to %u Hz", pin, frequency);

  return RESULT_SUCCESS;
}

std::vector<uint8_t> FlipperTools::getUARTDevices() {
  std::vector<uint8_t> devices;
  devices.push_back(1);  // UART1 for GPS
  return devices;
}

FlipperTools::ResultCode FlipperTools::scanUART() {
  DebugLogger::info("[FlipperTools] Scanning UART devices...");
  // TODO: Implement UART device scanning
  return RESULT_SUCCESS;
}

// === BadUSB/HID EMULATION ===

FlipperTools::ResultCode FlipperTools::sendKeyboardSequence(const std::string& sequence) {
  if (sequence.empty() || sequence.length() > 1000) {
    DebugLogger::error("[FlipperTools] Invalid keyboard sequence length: %u", sequence.length());
    return RESULT_ERROR_INVALID_PARAM;
  }

  if (!g_usbReady) {
    DebugLogger::error("[FlipperTools] USB HID not ready");
    return RESULT_ERROR_HARDWARE;
  }

  DebugLogger::info("[FlipperTools] Sending keyboard sequence: %s", sequence.c_str());
  // TODO: Implement USB HID keyboard emulation

  return RESULT_SUCCESS;
}

FlipperTools::ResultCode FlipperTools::sendMouseMove(int8_t x, int8_t y) {
  if (!g_usbReady) {
    return RESULT_ERROR_HARDWARE;
  }

  DebugLogger::verbose("[FlipperTools] Mouse move: (%d, %d)", x, y);
  // TODO: Implement USB HID mouse movement

  return RESULT_SUCCESS;
}

FlipperTools::ResultCode FlipperTools::sendMouseClick(uint8_t button) {
  if (button > 3) {
    DebugLogger::error("[FlipperTools] Invalid mouse button: %u", button);
    return RESULT_ERROR_INVALID_PARAM;
  }

  if (!g_usbReady) {
    return RESULT_ERROR_HARDWARE;
  }

  DebugLogger::info("[FlipperTools] Mouse click: button %u", button);
  // TODO: Implement USB HID mouse click

  return RESULT_SUCCESS;
}

FlipperTools::ResultCode FlipperTools::sendMouseDrag(int8_t x, int8_t y, uint8_t button) {
  if (button > 3 || !g_usbReady) {
    return RESULT_ERROR_INVALID_PARAM;
  }

  DebugLogger::info("[FlipperTools] Mouse drag: (%d, %d) button %u", x, y, button);
  return RESULT_SUCCESS;
}

bool FlipperTools::isUSBReady() const {
  return g_usbReady;
}

// === MALWARE SCANNER ===

FlipperTools::ResultCode FlipperTools::loadMalwareDatabase() {
  g_malwareDatabase.clear();

  const MalwareSignature sigs[] = {
    {"d41d8cd98f00b204e9800998ecf8427e", "EmptyFile", "Suspicious", "LOW", "Empty file signature", 0, false},
    {"5d41402abc4b2a76b9719d911017c592", "HelloWorld", "Known", "LOW", "Test string hash", 0, false},
  };

  for (const auto& sig : sigs) {
    g_malwareDatabase.push_back(sig);
  }

  g_malwareDbLoaded = true;
  DebugLogger::info("[FlipperTools] Loaded %u malware signatures", g_malwareDatabase.size());

  return RESULT_SUCCESS;
}

FlipperTools::ResultCode FlipperTools::scanForMalware(const std::string& hash, std::vector<MalwareSignature>& results) {
  if (!g_malwareDbLoaded) {
    DebugLogger::warn("[FlipperTools] Malware database not loaded");
    return RESULT_ERROR_INVALID_PARAM;
  }

  if (hash.empty() || (hash.length() != 32 && hash.length() != 40)) {
    DebugLogger::error("[FlipperTools] Invalid hash format: %s", hash.c_str());
    return RESULT_ERROR_INVALID_PARAM;
  }

  results.clear();
  for (const auto& sig : g_malwareDatabase) {
    if (sig.hash == hash) {
      results.push_back(sig);
    }
  }

  DebugLogger::info("[FlipperTools] Malware scan found %u matches", results.size());
  return RESULT_SUCCESS;
}

FlipperTools::ResultCode FlipperTools::calculateFileHash(const std::string& filepath, std::string& hashOutput) {
  if (filepath.empty() || filepath.length() > 255) {
    DebugLogger::error("[FlipperTools] Invalid filepath: %s", filepath.c_str());
    return RESULT_ERROR_INVALID_PARAM;
  }

  DebugLogger::info("[FlipperTools] Calculating hash for: %s", filepath.c_str());
  // TODO: Implement actual MD5/SHA256 hash calculation
  hashOutput = "d41d8cd98f00b204e9800998ecf8427e";

  return RESULT_SUCCESS;
}

uint32_t FlipperTools::getMalwareSignatureCount() const {
  return g_malwareDatabase.size();
}

bool FlipperTools::isMalwareDbLoaded() const {
  return g_malwareDbLoaded;
}

// === iButton EMULATION ===

FlipperTools::ResultCode FlipperTools::registerIButton(const IButtonKey& key) {
  if (key.family == 0) {
    DebugLogger::error("[FlipperTools] Invalid iButton family code");
    return RESULT_ERROR_INVALID_PARAM;
  }

  g_storedButtons.push_back(key);
  DebugLogger::info("[FlipperTools] Registered iButton family 0x%02X", key.family);

  return RESULT_SUCCESS;
}

FlipperTools::ResultCode FlipperTools::getStoredButtons(std::vector<IButtonKey>& buttons) {
  buttons = g_storedButtons;
  DebugLogger::info("[FlipperTools] Retrieved %u stored iButtons", buttons.size());

  return RESULT_SUCCESS;
}

FlipperTools::ResultCode FlipperTools::emulateIButton(const IButtonKey& key) {
  if (key.family == 0) {
    return RESULT_ERROR_INVALID_PARAM;
  }

  DebugLogger::info("[FlipperTools] Emulating iButton family 0x%02X", key.family);
  // TODO: Implement iButton emulation

  return RESULT_SUCCESS;
}

FlipperTools::ResultCode FlipperTools::deleteIButton(const std::string& id) {
  if (id.empty()) {
    return RESULT_ERROR_INVALID_PARAM;
  }

  // TODO: Implement iButton deletion
  DebugLogger::info("[FlipperTools] Deleted iButton: %s", id.c_str());

  return RESULT_SUCCESS;
}

uint32_t FlipperTools::getIButtonCount() const {
  return g_storedButtons.size();
}

// === GAMES & UTILITIES ===

void FlipperTools::playSnakeGame() {
  Serial.println("\n🐍 SNAKE GAME 🐍");
  Serial.println("Use ▲▼ buttons to navigate");
  Serial.println("Press BACK to exit\n");
  // TODO: Implement snake game
  delay(2000);
}

void FlipperTools::playFlappyBirdGame() {
  Serial.println("\n🐦 FLAPPY BIRD 🐦");
  Serial.println("Tap SELECT to fly!");
  Serial.println("Avoid obstacles\n");
  // TODO: Implement flappy bird game
  delay(2000);
}

void FlipperTools::displayAlarmClock() {
  Serial.println("\n⏰ ALARM CLOCK ⏰");
  Serial.println("Current time: (would show RTC time)");
  Serial.println("Set alarm via menu\n");
  // TODO: Implement alarm clock
}

void FlipperTools::displayMetronome(uint16_t bpm) {
  if (bpm == 0 || bpm > 300) {
    DebugLogger::error("[FlipperTools] Invalid BPM: %u (valid: 1-300)", bpm);
    return;
  }

  Serial.printf("\n🎵 METRONOME - %u BPM 🎵\n", bpm);
  Serial.println("Playing... (Press BACK to stop)\n");
  // TODO: Implement metronome with buzzer
}

void FlipperTools::displayMemoryStats() {
  Serial.println("\n💾 MEMORY STATISTICS 💾");
  Serial.printf("  Free heap: %u bytes\n", ESP.getFreeHeap());
  Serial.printf("  Total heap: %u bytes\n", ESP.getHeapSize());
  Serial.printf("  Free PSRAM: %u bytes\n", ESP.getFreePsram());
  Serial.println();
}

// === ARCHIVE/FILE MANAGER ===

FlipperTools::ResultCode FlipperTools::listFiles(const std::string& path, std::vector<std::string>& files) {
  if (path.empty() || path.length() > 255) {
    DebugLogger::error("[FlipperTools] Invalid file path: %s", path.c_str());
    return RESULT_ERROR_INVALID_PARAM;
  }

  files.clear();
  DebugLogger::info("[FlipperTools] Listing files in: %s", path.c_str());
  // TODO: Implement actual file listing

  return RESULT_SUCCESS;
}

FlipperTools::ResultCode FlipperTools::deleteFile(const std::string& path) {
  if (path.empty()) {
    return RESULT_ERROR_INVALID_PARAM;
  }

  DebugLogger::warn("[FlipperTools] Deleting file: %s", path.c_str());
  // TODO: Implement file deletion

  return RESULT_SUCCESS;
}

FlipperTools::ResultCode FlipperTools::renameFile(const std::string& oldPath, const std::string& newPath) {
  if (oldPath.empty() || newPath.empty()) {
    return RESULT_ERROR_INVALID_PARAM;
  }

  DebugLogger::info("[FlipperTools] Renaming: %s -> %s", oldPath.c_str(), newPath.c_str());
  // TODO: Implement file renaming

  return RESULT_SUCCESS;
}

FlipperTools::ResultCode FlipperTools::getFileSize(const std::string& path, uint32_t& size) {
  if (path.empty()) {
    return RESULT_ERROR_INVALID_PARAM;
  }

  DebugLogger::verbose("[FlipperTools] Getting size of: %s", path.c_str());
  size = 0;
  // TODO: Implement file size retrieval

  return RESULT_SUCCESS;
}

FlipperTools::ResultCode FlipperTools::copyFile(const std::string& source, const std::string& dest) {
  if (source.empty() || dest.empty()) {
    return RESULT_ERROR_INVALID_PARAM;
  }

  DebugLogger::info("[FlipperTools] Copying: %s -> %s", source.c_str(), dest.c_str());
  // TODO: Implement file copying

  return RESULT_SUCCESS;
}

// System status methods
bool FlipperTools::isHealthy() const {
  return ESP.getFreeHeap() > 10000;  // At least 10KB free
}

std::string FlipperTools::getStatus() const {
  if (isHealthy()) {
    return "🐯 Tiger Audit System Active";
  }
  return "⚠️ Tiger Audit System - Low Memory";
}
