#ifndef SYSTEM_DIAGNOSTICS_H
#define SYSTEM_DIAGNOSTICS_H

#include <Arduino.h>
#include <vector>

// ============= HARDWARE STATUS =============

enum class HardwareStatus {
  OK = 0,
  WARNING = 1,
  ERROR = 2,
  NOT_DETECTED = 3,
  UNTESTED = 4
};

struct HardwareInfo {
  const char* name;
  HardwareStatus status;
  const char* description;
  uint32_t lastTestTime;
  uint32_t testDuration;

  HardwareInfo() : name(""), status(HardwareStatus::UNTESTED),
                   description(""), lastTestTime(0), testDuration(0) {}
};

struct SystemDiagnostics {
  uint16_t cpuFreq;              // MHz
  uint32_t heapFree;             // Bytes
  uint32_t psramFree;            // Bytes
  float cpuTemp;                 // Celsius (if sensor available)
  uint16_t watchdogTimeouts;     // Count
  uint16_t rebootCount;
  uint32_t uptime;               // Seconds
  const char* buildVersion;
  const char* buildDate;
};

// ============= HARDWARE DETECTION =============

class HardwareDetector {
public:
  static HardwareDetector& getInstance() {
    static HardwareDetector instance;
    return instance;
  }

  void scanAll();
  void scanGPIO();
  void scanI2C();
  void scanSPI();
  void scanUART();
  void scanRF();
  void scanDisplay();
  void scanSensors();

  HardwareInfo* getHardwareInfo(const char* name);
  uint16_t getAllHardwareInfo(HardwareInfo* output, uint16_t maxCount);

  HardwareStatus getStatus(const char* hardwareName);
  uint16_t getDetectedCount() const;
  uint16_t getWorkingCount() const;
  uint16_t getErrorCount() const;

  void printDetectionReport();

private:
  HardwareDetector();

  std::vector<HardwareInfo*> detected;
  uint32_t lastScanTime;

  static const uint16_t MAX_HARDWARE = 30;

  void addDetected(const char* name, HardwareStatus status, const char* desc);
  bool testI2CDevice(uint8_t addr);
  bool testSPIDevice(uint8_t cs);
};

// ============= SELF-TEST MODE =============

enum class TestLevel {
  QUICK = 0,      // 5 seconds - essential only
  STANDARD = 1,   // 30 seconds - all modules
  FULL = 2,       // 120+ seconds - stress tests
  EXTENDED = 3    // 300+ seconds - deep diagnostics
};

struct TestResult {
  const char* testName;
  bool passed;
  uint32_t duration;
  const char* errorMessage;

  TestResult() : testName(""), passed(false), duration(0), errorMessage("") {}
};

class SystemSelfTest {
public:
  static SystemSelfTest& getInstance() {
    static SystemSelfTest instance;
    return instance;
  }

  void begin();
  void runTest(TestLevel level);
  bool isRunning() const { return testing; }
  uint8_t getProgress() const { return testProgress; }

  TestResult* getResult(uint16_t index);
  uint16_t getResultCount() const { return results.size(); }
  uint16_t getPassedCount() const;
  uint16_t getFailedCount() const;

  void printResults();
  void generateReport(char* output, uint16_t maxLen);

private:
  SystemSelfTest();

  std::vector<TestResult*> results;
  bool testing;
  uint8_t testProgress;
  uint32_t testStartTime;

  static const uint16_t MAX_RESULTS = 50;

  // Test methods
  bool testGPIO();
  bool testI2C();
  bool testSPI();
  bool testUART();
  bool testRF();
  bool testDisplay();
  bool testMemory();
  bool testWiFi();
  bool testBLE();

  void addResult(const char* name, bool passed, uint32_t duration, const char* error = "");
};

// ============= SYSTEM INFO =============

class SystemInfo {
public:
  static SystemInfo& getInstance() {
    static SystemInfo instance;
    return instance;
  }

  void update();

  SystemDiagnostics* getDiagnostics() { return &diags; }

  uint16_t getCPUFreq() const { return diags.cpuFreq; }
  uint32_t getHeapFree() const { return diags.heapFree; }
  uint32_t getPSRAMFree() const { return diags.psramFree; }
  uint32_t getUptime() const { return diags.uptime; }

  const char* getDeviceMAC();
  const char* getChipModel();
  uint32_t getChipID();
  uint32_t getFlashSize();

  float getHeapFragmentation();
  bool isHeapFragmented() const { return getHeapFragmentation() > 0.5f; }

  void printSystemInfo();

private:
  SystemInfo();

  SystemDiagnostics diags;
  uint32_t lastUpdateTime;
  char deviceMAC[18];
};

// ============= MODE MANAGER =============

enum class OperatingMode {
  NORMAL = 0,        // Regular operation
  DIAGNOSTIC = 1,    // Self-test mode
  LOW_POWER = 2,     // Battery conservation
  RECOVERY = 3,      // Error recovery
  MAINTENANCE = 4    // Debug/maintenance
};

class ModeManager {
public:
  static ModeManager& getInstance() {
    static ModeManager instance;
    return instance;
  }

  void setMode(OperatingMode newMode);
  OperatingMode getMode() const { return currentMode; }

  bool isInDiagnosticMode() const { return currentMode == OperatingMode::DIAGNOSTIC; }
  bool isInRecoveryMode() const { return currentMode == OperatingMode::RECOVERY; }

  void handleDiagnosticMode();
  void handleRecoveryMode();
  void handleMaintenanceMode();

private:
  ModeManager() : currentMode(OperatingMode::NORMAL) {}

  OperatingMode currentMode;
  uint32_t modeStartTime;
};

#endif // SYSTEM_DIAGNOSTICS_H
