#ifndef TESTING_FRAMEWORK_H
#define TESTING_FRAMEWORK_H

#include <Arduino.h>
#include <vector>
#include "attack_framework.h"

// ============= TEST RESULT STRUCTURE =============

enum class TestStatus {
  PASSED = 0,
  FAILED = 1,
  SKIPPED = 2,
  ERROR = 3
};

struct TestResult {
  const char* testName;
  const char* testModule;
  TestStatus status;
  uint32_t durationMs;
  char errorMessage[256];
  uint32_t timestamp;

  TestResult() : testName(""), testModule(""), status(TestStatus::PASSED),
                 durationMs(0), timestamp(0) {
    memset(errorMessage, 0, 256);
  }
};

// ============= TEST SUITE CLASS =============

class TestSuite {
public:
  TestSuite(const char* name) : suiteName(name), totalTests(0), passedTests(0),
                                failedTests(0), skippedTests(0) {}

  void addTest(const char* testName, bool (*testFunc)(void));
  void runAll();
  void runTest(uint16_t index);

  uint16_t getTotalTests() const { return totalTests; }
  uint16_t getPassedTests() const { return passedTests; }
  uint16_t getFailedTests() const { return failedTests; }
  uint16_t getSkippedTests() const { return skippedTests; }

  TestResult* getResult(uint16_t index) {
    if (index < results.size()) return results[index];
    return nullptr;
  }

  void printSummary();

private:
  const char* suiteName;
  std::vector<const char*> testNames;
  std::vector<bool (*)(void)> testFunctions;
  std::vector<TestResult*> results;

  uint16_t totalTests;
  uint16_t passedTests;
  uint16_t failedTests;
  uint16_t skippedTests;
};

// ============= ASSERTIONS =============

#define ASSERT_TRUE(condition) \
  if (!(condition)) { \
    Serial.printf("ASSERTION FAILED: " #condition " at line %d\n", __LINE__); \
    return false; \
  }

#define ASSERT_FALSE(condition) \
  if (condition) { \
    Serial.printf("ASSERTION FAILED: !" #condition " at line %d\n", __LINE__); \
    return false; \
  }

#define ASSERT_EQUAL(a, b) \
  if ((a) != (b)) { \
    Serial.printf("ASSERTION FAILED: " #a " != " #b " at line %d\n", __LINE__); \
    return false; \
  }

#define ASSERT_NOT_NULL(ptr) \
  if ((ptr) == nullptr) { \
    Serial.printf("ASSERTION FAILED: " #ptr " is NULL at line %d\n", __LINE__); \
    return false; \
  }

// ============= MOCK DRIVERS =============

namespace MockDrivers {

// Mock GPIO Driver
class MockGPIO {
public:
  static void init() { initialized = true; }
  static bool isInitialized() { return initialized; }
  static void simulateButtonPress(uint8_t button) { lastButton = button; }
  static uint8_t getLastButton() { return lastButton; }
  static void setBatteryPercent(uint8_t percent) { batteryPercent = percent; }
  static uint8_t getBatteryPercent() { return batteryPercent; }

private:
  static bool initialized;
  static uint8_t lastButton;
  static uint8_t batteryPercent;
};

// Mock Display Driver
class MockDisplay {
public:
  static void init() { initialized = true; }
  static bool isInitialized() { return initialized; }
  static void recordDrawCall(const char* operation) {
    if (drawCalls.size() < MAX_DRAW_CALLS) {
      drawCalls.push_back(operation);
    }
  }
  static uint16_t getDrawCallCount() { return drawCalls.size(); }
  static const char* getDrawCall(uint16_t index) {
    if (index < drawCalls.size()) return drawCalls[index];
    return nullptr;
  }
  static void clearDrawCalls() { drawCalls.clear(); }

private:
  static bool initialized;
  static std::vector<const char*> drawCalls;
  static const uint16_t MAX_DRAW_CALLS = 100;
};

// Mock I2C Driver
class MockI2C {
public:
  static void init() { initialized = true; }
  static bool isInitialized() { return initialized; }
  static bool read(uint8_t addr, uint8_t* data, uint16_t len) {
    // Simulate successful I2C read
    return true;
  }
  static bool write(uint8_t addr, const uint8_t* data, uint16_t len) {
    // Simulate successful I2C write
    return true;
  }
  static uint16_t getReadCount() { return readCount; }
  static uint16_t getWriteCount() { return writeCount; }

private:
  static bool initialized;
  static uint16_t readCount;
  static uint16_t writeCount;
};

// Mock RF Driver (CC1101, NRF24, etc.)
class MockRF {
public:
  static void init() { initialized = true; }
  static bool isInitialized() { return initialized; }
  static void simulateRSSI(int8_t rssi) { lastRSSI = rssi; }
  static int8_t getRSSI() { return lastRSSI; }
  static void simulateData(const uint8_t* data, uint16_t len) {
    if (len <= MAX_PACKET_SIZE) {
      memcpy(mockData, data, len);
      mockDataLen = len;
      hasData = true;
    }
  }
  static bool hasReceivedData() { return hasData; }
  static uint16_t getDataLength() { return mockDataLen; }
  static const uint8_t* getData() { return mockData; }
  static void clearData() {
    hasData = false;
    mockDataLen = 0;
  }

private:
  static bool initialized;
  static int8_t lastRSSI;
  static bool hasData;
  static uint16_t mockDataLen;
  static uint8_t mockData[255];
  static const uint16_t MAX_PACKET_SIZE = 255;
};

// Mock NFC Driver
class MockNFC {
public:
  static void init() { initialized = true; }
  static bool isInitialized() { return initialized; }
  static void simulateCardDetection(const char* cardUID) {
    strncpy(detectedCard, cardUID, 31);
    detectedCard[31] = '\0';
    cardDetected = true;
  }
  static bool cardReady() { return cardDetected; }
  static const char* getDetectedCard() { return detectedCard; }
  static void clearCardDetection() {
    cardDetected = false;
    memset(detectedCard, 0, 32);
  }

private:
  static bool initialized;
  static bool cardDetected;
  static char detectedCard[32];
};

} // namespace MockDrivers

// ============= MOCK ATTACKS =============

class MockAttack : public Attack {
public:
  MockAttack(const char* name) : Attack(name) {}

  bool begin() override { return true; }
  bool start() override {
    Attack::start();
    startTime = millis();
    return true;
  }
  bool stop() override {
    Attack::stop();
    return true;
  }

  void update() override {
    if (isRunning && millis() - startTime > 1000) {
      AttackResult* result = ResultBuilder::createScan("Mock Target", -65);
      addResult(result);
      setStatus(AttackStatus::SUCCESS);
    }
  }

  void generateTestResults(uint16_t count) {
    for (uint16_t i = 0; i < count; i++) {
      char desc[64];
      snprintf(desc, 63, "Test Result %u", i);
      AttackResult* result = ResultBuilder::createScan(desc, -50 - i);
      addResult(result);
    }
  }
};

// ============= HARDWARE TEST SUITE =============

class HardwareTestSuite : public TestSuite {
public:
  HardwareTestSuite() : TestSuite("Hardware Tests") {
    addTest("GPIO Initialization", testGPIOInit);
    addTest("Display Initialization", testDisplayInit);
    addTest("I2C Communication", testI2CComm);
    addTest("RF Module Init", testRFInit);
    addTest("NFC Detection", testNFCDetection);
  }

private:
  static bool testGPIOInit();
  static bool testDisplayInit();
  static bool testI2CComm();
  static bool testRFInit();
  static bool testNFCDetection();
};

// ============= ATTACK TEST SUITE =============

class AttackTestSuite : public TestSuite {
public:
  AttackTestSuite() : TestSuite("Attack Framework Tests") {
    addTest("Attack Creation", testAttackCreation);
    addTest("Result Builder", testResultBuilder);
    addTest("Result Collection", testResultCollection);
    addTest("Status Management", testStatusManagement);
  }

private:
  static bool testAttackCreation();
  static bool testResultBuilder();
  static bool testResultCollection();
  static bool testStatusManagement();
};

// ============= PERFORMANCE BENCHMARK =============

struct BenchmarkResult {
  const char* name;
  uint32_t minTimeMs;
  uint32_t maxTimeMs;
  uint32_t avgTimeMs;
  uint32_t runCount;
};

class PerformanceBenchmark {
public:
  static PerformanceBenchmark& getInstance() {
    static PerformanceBenchmark instance;
    return instance;
  }

  void startMeasure(const char* benchName) {
    strncpy(currentBench, benchName, 63);
    currentBench[63] = '\0';
    startTime = millis();
  }

  void stopMeasure() {
    uint32_t elapsed = millis() - startTime;
    recordMeasurement(currentBench, elapsed);
  }

  BenchmarkResult* getResult(const char* benchName);
  void printAllResults();

private:
  PerformanceBenchmark() {}

  void recordMeasurement(const char* benchName, uint32_t timeMs);

  char currentBench[64];
  uint32_t startTime;
  std::vector<BenchmarkResult*> results;
};

// ============= MEMORY PROFILER =============

struct MemoryStats {
  uint32_t heapFree;
  uint32_t heapUsed;
  uint32_t psramFree;
  uint32_t psramUsed;
  uint32_t timestamp;
};

class MemoryProfiler {
public:
  static MemoryProfiler& getInstance() {
    static MemoryProfiler instance;
    return instance;
  }

  void captureSnapshot(const char* label) {
    MemoryStats* stats = new MemoryStats();
    stats->heapFree = ESP.getFreeHeap();
    stats->heapUsed = ESP.getHeapSize() - stats->heapFree;
    stats->psramFree = ESP.getFreePsram();
    stats->psramUsed = ESP.getPsramSize() - stats->psramFree;
    stats->timestamp = millis();

    snapshots.push_back(stats);
  }

  void printReport() {
    Serial.println("\n=== Memory Profile Report ===");
    for (auto* snap : snapshots) {
      Serial.printf("Heap: %u KB free, PSRAM: %u KB free\n",
        snap->heapFree / 1024, snap->psramFree / 1024);
    }
  }

private:
  MemoryProfiler() {}
  std::vector<MemoryStats*> snapshots;
};

#endif // TESTING_FRAMEWORK_H
