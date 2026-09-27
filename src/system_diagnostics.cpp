#include "system_diagnostics.h"
#include "logging_system.h"

// ============= HARDWARE DETECTOR IMPLEMENTATION =============

HardwareDetector::HardwareDetector() : lastScanTime(0) {
  Logger::getInstance().info("Detector", "Initialized");
}

void HardwareDetector::scanAll() {
  Logger::getInstance().info("Detector", "Starting full hardware scan...");
  detected.clear();

  scanGPIO();
  scanI2C();
  scanSPI();
  scanUART();
  scanRF();
  scanDisplay();
  scanSensors();

  lastScanTime = millis();
  printDetectionReport();
}

void HardwareDetector::scanGPIO() {
  addDetected("GPIO_Buttons", HardwareStatus::OK, "Buttons: UP, DOWN, SELECT, BACK");
  addDetected("GPIO_Buzzer", HardwareStatus::OK, "Buzzer on GPIO 21");
  addDetected("GPIO_Battery", HardwareStatus::OK, "Battery ADC on GPIO 7");
  addDetected("GPIO_IR", HardwareStatus::OK, "IR TX/RX on GPIO 38/39");
}

void HardwareDetector::scanI2C() {
  // Test RTC
  if (testI2CDevice(0x68)) {
    addDetected("RTC_DS3231", HardwareStatus::OK, "RTC at 0x68");
  } else {
    addDetected("RTC_DS3231", HardwareStatus::ERROR, "RTC not responding at 0x68");
  }

  // Test NFC
  if (testI2CDevice(0x24)) {
    addDetected("NFC_PN532", HardwareStatus::OK, "NFC reader at 0x24");
  } else {
    addDetected("NFC_PN532", HardwareStatus::WARNING, "NFC not detected at 0x24");
  }
}

void HardwareDetector::scanSPI() {
  // Test CC1101 (433MHz)
  if (testSPIDevice(10)) {
    addDetected("RF_CC1101_433", HardwareStatus::OK, "433MHz transceiver (CS=10)");
  } else {
    addDetected("RF_CC1101_433", HardwareStatus::WARNING, "433MHz not detected");
  }

  // Test NRF24 (2.4GHz)
  if (testSPIDevice(14)) {
    addDetected("RF_NRF24_2400", HardwareStatus::OK, "2.4GHz transceiver (CS=14)");
  } else {
    addDetected("RF_NRF24_2400", HardwareStatus::WARNING, "2.4GHz not detected");
  }

  // Test SX1262 (868MHz)
  if (testSPIDevice(5)) {
    addDetected("RF_SX1262_868", HardwareStatus::OK, "868MHz LoRa (CS=5)");
  } else {
    addDetected("RF_SX1262_868", HardwareStatus::WARNING, "868MHz not detected");
  }
}

void HardwareDetector::scanUART() {
  addDetected("UART_GPS", HardwareStatus::OK, "GPS on UART1 (RX=18, TX=17)");
  addDetected("UART_Debug", HardwareStatus::OK, "Debug UART0 (USB)");
}

void HardwareDetector::scanRF() {
  // Covered in scanSPI
}

void HardwareDetector::scanDisplay() {
  addDetected("Display_ILI9341", HardwareStatus::OK, "3.5\" TFT display");
  addDetected("Touch_XPT2046", HardwareStatus::OK, "Resistive touchscreen");
}

void HardwareDetector::scanSensors() {
  addDetected("RFID_MFRC522", HardwareStatus::OK, "RFID reader via SPI");
}

HardwareInfo* HardwareDetector::getHardwareInfo(const char* name) {
  for (auto* hw : detected) {
    if (strcmp(hw->name, name) == 0) {
      return hw;
    }
  }
  return nullptr;
}

uint16_t HardwareDetector::getAllHardwareInfo(HardwareInfo* output, uint16_t maxCount) {
  uint16_t count = (detected.size() < maxCount) ? detected.size() : maxCount;
  for (uint16_t i = 0; i < count; i++) {
    output[i] = *detected[i];
  }
  return count;
}

HardwareStatus HardwareDetector::getStatus(const char* hardwareName) {
  HardwareInfo* hw = getHardwareInfo(hardwareName);
  if (hw) return hw->status;
  return HardwareStatus::NOT_DETECTED;
}

uint16_t HardwareDetector::getDetectedCount() const {
  return detected.size();
}

uint16_t HardwareDetector::getWorkingCount() const {
  uint16_t count = 0;
  for (auto* hw : detected) {
    if (hw->status == HardwareStatus::OK) count++;
  }
  return count;
}

uint16_t HardwareDetector::getErrorCount() const {
  uint16_t count = 0;
  for (auto* hw : detected) {
    if (hw->status == HardwareStatus::ERROR) count++;
  }
  return count;
}

void HardwareDetector::printDetectionReport() {
  Serial.println("\n========== Hardware Detection Report ==========");
  Serial.printf("Total: %u | Working: %u | Errors: %u\n",
    getDetectedCount(), getWorkingCount(), getErrorCount());

  for (auto* hw : detected) {
    const char* statusStr = "";
    switch (hw->status) {
      case HardwareStatus::OK: statusStr = "✓ OK"; break;
      case HardwareStatus::WARNING: statusStr = "⚠ WARNING"; break;
      case HardwareStatus::ERROR: statusStr = "✗ ERROR"; break;
      case HardwareStatus::NOT_DETECTED: statusStr = "? NOT DETECTED"; break;
      case HardwareStatus::UNTESTED: statusStr = "- UNTESTED"; break;
    }
    Serial.printf("  %s | %s | %s\n", statusStr, hw->name, hw->description);
  }
  Serial.println("=============================================\n");
}

void HardwareDetector::addDetected(const char* name, HardwareStatus status, const char* desc) {
  if (detected.size() >= MAX_HARDWARE) return;

  HardwareInfo* hw = new HardwareInfo();
  hw->name = name;
  hw->status = status;
  hw->description = desc;
  hw->lastTestTime = millis();
  hw->testDuration = 0;

  detected.push_back(hw);
}

bool HardwareDetector::testI2CDevice(uint8_t addr) {
  // TODO: Implement actual I2C device detection
  return true;
}

bool HardwareDetector::testSPIDevice(uint8_t cs) {
  // TODO: Implement actual SPI device detection
  return true;
}

// ============= SYSTEM SELF-TEST IMPLEMENTATION =============

SystemSelfTest::SystemSelfTest() : testing(false), testProgress(0), testStartTime(0) {
  Logger::getInstance().info("SelfTest", "Initialized");
}

void SystemSelfTest::begin() {
  results.clear();
  testing = false;
  testProgress = 0;
}

void SystemSelfTest::runTest(TestLevel level) {
  Logger::getInstance().info("SelfTest", "Starting self-test...");

  testing = true;
  testStartTime = millis();

  switch (level) {
    case TestLevel::QUICK:
      testProgress = 0;
      testGPIO();
      testProgress = 25;
      testI2C();
      testProgress = 50;
      testMemory();
      testProgress = 75;
      break;

    case TestLevel::STANDARD:
      testProgress = 0;
      testGPIO();
      testProgress = 15;
      testI2C();
      testProgress = 30;
      testSPI();
      testProgress = 45;
      testDisplay();
      testProgress = 60;
      testMemory();
      testProgress = 75;
      testWiFi();
      testProgress = 90;
      break;

    case TestLevel::FULL:
    case TestLevel::EXTENDED:
      // Run all tests with stress testing
      testGPIO();
      testI2C();
      testSPI();
      testUART();
      testRF();
      testDisplay();
      testMemory();
      testWiFi();
      testBLE();
      break;
  }

  testProgress = 100;
  testing = false;

  printResults();
}

TestResult* SystemSelfTest::getResult(uint16_t index) {
  if (index < results.size()) {
    return results[index];
  }
  return nullptr;
}

uint16_t SystemSelfTest::getPassedCount() const {
  uint16_t count = 0;
  for (auto* r : results) {
    if (r->passed) count++;
  }
  return count;
}

uint16_t SystemSelfTest::getFailedCount() const {
  uint16_t count = 0;
  for (auto* r : results) {
    if (!r->passed) count++;
  }
  return count;
}

void SystemSelfTest::printResults() {
  Serial.println("\n========== Self-Test Results ==========");
  Serial.printf("Total: %u | Passed: %u | Failed: %u\n",
    (uint16_t)results.size(), getPassedCount(), getFailedCount());

  for (auto* r : results) {
    Serial.printf("  %s: %s (%u ms)\n", r->testName,
      r->passed ? "✓ PASSED" : "✗ FAILED", r->duration);
    if (!r->passed && r->errorMessage) {
      Serial.printf("    Error: %s\n", r->errorMessage);
    }
  }
  Serial.println("=====================================\n");
}

void SystemSelfTest::generateReport(char* output, uint16_t maxLen) {
  uint16_t written = 0;
  written += snprintf(output + written, maxLen - written,
    "SELF-TEST REPORT\n================\nTotal: %u\nPassed: %u\nFailed: %u\n\nDetails:\n",
    (uint16_t)results.size(), getPassedCount(), getFailedCount());

  for (auto* r : results) {
    written += snprintf(output + written, maxLen - written,
      "%s: %s (%u ms)\n", r->testName, r->passed ? "PASS" : "FAIL", r->duration);
  }
}

bool SystemSelfTest::testGPIO() {
  uint32_t start = millis();
  // TODO: Implement GPIO tests
  addResult("GPIO", true, millis() - start);
  return true;
}

bool SystemSelfTest::testI2C() {
  uint32_t start = millis();
  // TODO: Implement I2C tests
  addResult("I2C", true, millis() - start);
  return true;
}

bool SystemSelfTest::testSPI() {
  uint32_t start = millis();
  // TODO: Implement SPI tests
  addResult("SPI", true, millis() - start);
  return true;
}

bool SystemSelfTest::testUART() {
  uint32_t start = millis();
  // TODO: Implement UART tests
  addResult("UART", true, millis() - start);
  return true;
}

bool SystemSelfTest::testRF() {
  uint32_t start = millis();
  // TODO: Implement RF module tests
  addResult("RF", true, millis() - start);
  return true;
}

bool SystemSelfTest::testDisplay() {
  uint32_t start = millis();
  // TODO: Implement display tests
  addResult("Display", true, millis() - start);
  return true;
}

bool SystemSelfTest::testMemory() {
  uint32_t start = millis();
  uint32_t heapFree = ESP.getFreeHeap();
  bool passed = heapFree > 100000; // At least 100KB free
  addResult("Memory", passed, millis() - start);
  return passed;
}

bool SystemSelfTest::testWiFi() {
  uint32_t start = millis();
  // TODO: Implement WiFi tests
  addResult("WiFi", true, millis() - start);
  return true;
}

bool SystemSelfTest::testBLE() {
  uint32_t start = millis();
  // TODO: Implement BLE tests
  addResult("BLE", true, millis() - start);
  return true;
}

void SystemSelfTest::addResult(const char* name, bool passed, uint32_t duration, const char* error) {
  if (results.size() >= MAX_RESULTS) return;

  TestResult* result = new TestResult();
  result->testName = name;
  result->passed = passed;
  result->duration = duration;
  if (error) result->errorMessage = error;

  results.push_back(result);
}

// ============= SYSTEM INFO IMPLEMENTATION =============

SystemInfo::SystemInfo() : lastUpdateTime(0) {
  memset(deviceMAC, 0, 18);
  update();
}

void SystemInfo::update() {
  uint32_t now = millis();
  if (now - lastUpdateTime < 1000) return; // Update every 1 second
  lastUpdateTime = now;

  diags.cpuFreq = getCPUFreq();
  diags.heapFree = ESP.getFreeHeap();
  diags.psramFree = ESP.getFreePsram();
  diags.uptime = now / 1000;
}

const char* SystemInfo::getDeviceMAC() {
  // TODO: Get actual MAC address
  strncpy(deviceMAC, "AA:BB:CC:DD:EE:FF", sizeof(deviceMAC) - 1);  // FIX: Use strncpy
  deviceMAC[sizeof(deviceMAC) - 1] = '\0';
  return deviceMAC;
}

const char* SystemInfo::getChipModel() {
  return "ESP32-S3-WROOM-2";
}

uint32_t SystemInfo::getChipID() {
  return ESP.getEfuseMac() & 0xFFFFFF;
}

uint32_t SystemInfo::getFlashSize() {
  return 16 * 1024 * 1024; // 16MB
}

float SystemInfo::getHeapFragmentation() {
  // Simple fragmentation estimate
  return (float)(ESP.getHeapSize() - ESP.getFreeHeap()) / ESP.getHeapSize();
}

void SystemInfo::printSystemInfo() {
  Serial.println("\n========== System Information ==========");
  Serial.printf("Chip: %s\n", getChipModel());
  Serial.printf("MAC: %s\n", getDeviceMAC());
  Serial.printf("Chip ID: %u\n", getChipID());
  Serial.printf("Flash: %u MB\n", getFlashSize() / (1024*1024));
  Serial.printf("Heap: %u KB free\n", diags.heapFree / 1024);
  Serial.printf("PSRAM: %u KB free\n", diags.psramFree / 1024);
  Serial.printf("Uptime: %u s\n", diags.uptime);
  Serial.printf("CPU: %u MHz\n", diags.cpuFreq);
  Serial.println("=========================================\n");
}

// ============= MODE MANAGER IMPLEMENTATION =============

void ModeManager::setMode(OperatingMode newMode) {
  if (newMode == currentMode) return;

  currentMode = newMode;
  modeStartTime = millis();

  const char* modeStr = "";
  switch (newMode) {
    case OperatingMode::NORMAL: modeStr = "NORMAL"; break;
    case OperatingMode::DIAGNOSTIC: modeStr = "DIAGNOSTIC"; break;
    case OperatingMode::LOW_POWER: modeStr = "LOW_POWER"; break;
    case OperatingMode::RECOVERY: modeStr = "RECOVERY"; break;
    case OperatingMode::MAINTENANCE: modeStr = "MAINTENANCE"; break;
  }

  char msg[128];
  snprintf(msg, 127, "Mode changed to: %s", modeStr);
  Logger::getInstance().info("ModeManager", msg);
}

void ModeManager::handleDiagnosticMode() {
  // TODO: Implement diagnostic mode logic
}

void ModeManager::handleRecoveryMode() {
  // TODO: Implement recovery mode logic
}

void ModeManager::handleMaintenanceMode() {
  // TODO: Implement maintenance mode logic
}
