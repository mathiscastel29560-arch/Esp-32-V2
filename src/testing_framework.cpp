#include "testing_framework.h"
#include "logging_system.h"

// ============= TEST SUITE IMPLEMENTATION =============

void TestSuite::addTest(const char* testName, bool (*testFunc)(void)) {
  testNames.push_back(testName);
  testFunctions.push_back(testFunc);
  totalTests++;
}

void TestSuite::runAll() {
  Serial.printf("\n========== Running Test Suite: %s ==========\n", suiteName);

  for (uint16_t i = 0; i < testNames.size(); i++) {
    runTest(i);
  }

  printSummary();
}

void TestSuite::runTest(uint16_t index) {
  if (index >= testNames.size()) return;

  const char* testName = testNames[index];
  bool (*testFunc)(void) = testFunctions[index];

  Serial.printf("[%u/%u] Running: %s ... ", index + 1, testNames.size(), testName);

  uint32_t startTime = millis();
  bool result = testFunc();
  uint32_t duration = millis() - startTime;

  TestResult* testResult = new TestResult();
  testResult->testName = testName;
  testResult->testModule = suiteName;
  testResult->timestamp = millis();
  testResult->durationMs = duration;

  if (result) {
    Serial.printf("✓ PASSED (%u ms)\n", duration);
    testResult->status = TestStatus::PASSED;
    passedTests++;
  } else {
    Serial.printf("✗ FAILED (%u ms)\n", duration);
    testResult->status = TestStatus::FAILED;
    failedTests++;
  }

  results.push_back(testResult);
}

void TestSuite::printSummary() {
  Serial.printf("\n========== Test Summary: %s ==========\n", suiteName);
  Serial.printf("Total:  %u\n", totalTests);
  Serial.printf("Passed: %u ✓\n", passedTests);
  Serial.printf("Failed: %u ✗\n", failedTests);
  Serial.printf("Skipped: %u ⊘\n", skippedTests);
  Serial.printf("Pass Rate: %.1f%%\n", (float)passedTests * 100.0f / totalTests);
  Serial.println("=========================================\n");
}

// ============= MOCK DRIVERS IMPLEMENTATION =============

namespace MockDrivers {

bool MockGPIO::initialized = false;
uint8_t MockGPIO::lastButton = 0;
uint8_t MockGPIO::batteryPercent = 100;

bool MockDisplay::initialized = false;
std::vector<const char*> MockDisplay::drawCalls;

bool MockI2C::initialized = false;
uint16_t MockI2C::readCount = 0;
uint16_t MockI2C::writeCount = 0;

bool MockRF::initialized = false;
int8_t MockRF::lastRSSI = -70;
bool MockRF::hasData = false;
uint16_t MockRF::mockDataLen = 0;
uint8_t MockRF::mockData[255] = {0};

bool MockNFC::initialized = false;
bool MockNFC::cardDetected = false;
char MockNFC::detectedCard[32] = {0};

} // namespace MockDrivers

// ============= HARDWARE TEST SUITE IMPLEMENTATION =============

bool HardwareTestSuite::testGPIOInit() {
  MockDrivers::MockGPIO::init();
  ASSERT_TRUE(MockDrivers::MockGPIO::isInitialized());
  return true;
}

bool HardwareTestSuite::testDisplayInit() {
  MockDrivers::MockDisplay::init();
  ASSERT_TRUE(MockDrivers::MockDisplay::isInitialized());
  return true;
}

bool HardwareTestSuite::testI2CComm() {
  MockDrivers::MockI2C::init();
  ASSERT_TRUE(MockDrivers::MockI2C::isInitialized());

  uint8_t testData[8] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
  bool readResult = MockDrivers::MockI2C::read(0x68, testData, 8);
  ASSERT_TRUE(readResult);

  bool writeResult = MockDrivers::MockI2C::write(0x68, testData, 8);
  ASSERT_TRUE(writeResult);

  return true;
}

bool HardwareTestSuite::testRFInit() {
  MockDrivers::MockRF::init();
  ASSERT_TRUE(MockDrivers::MockRF::isInitialized());

  MockDrivers::MockRF::simulateRSSI(-60);
  ASSERT_EQUAL(MockDrivers::MockRF::getRSSI(), -60);

  return true;
}

bool HardwareTestSuite::testNFCDetection() {
  MockDrivers::MockNFC::init();
  ASSERT_TRUE(MockDrivers::MockNFC::isInitialized());

  MockDrivers::MockNFC::simulateCardDetection("04:AB:CD:EF");
  ASSERT_TRUE(MockDrivers::MockNFC::cardReady());

  const char* card = MockDrivers::MockNFC::getDetectedCard();
  ASSERT_NOT_NULL(card);

  return true;
}

// ============= ATTACK TEST SUITE IMPLEMENTATION =============

bool AttackTestSuite::testAttackCreation() {
  MockAttack attack("Test Attack");
  ASSERT_NOT_NULL(attack.getName());
  ASSERT_TRUE(attack.begin());
  ASSERT_TRUE(attack.start());
  ASSERT_TRUE(attack.isActive());
  ASSERT_EQUAL(attack.getStatus(), AttackStatus::SCANNING);
  ASSERT_TRUE(attack.stop());
  ASSERT_FALSE(attack.isActive());
  return true;
}

bool AttackTestSuite::testResultBuilder() {
  // Test SCAN result creation
  AttackResult* scanResult = ResultBuilder::createScan("Test Network", -75);
  ASSERT_NOT_NULL(scanResult);
  ASSERT_EQUAL((int)scanResult->type, (int)ResultType::SCAN);
  ASSERT_EQUAL((int)scanResult->status, (int)AttackStatus::SUCCESS);
  ASSERT_EQUAL(scanResult->rssi, -75);
  delete scanResult;

  // Test PACKET result creation
  uint8_t testData[] = {0x01, 0x02, 0x03, 0x04};
  AttackResult* packetResult = ResultBuilder::createPacket(testData, 4, "Test Packet");
  ASSERT_NOT_NULL(packetResult);
  ASSERT_EQUAL((int)packetResult->type, (int)ResultType::PACKET);
  ASSERT_EQUAL(packetResult->dataLen, 4);
  delete packetResult;

  // Test STATUS result creation
  AttackResult* statusResult = ResultBuilder::createStatus("Scanning...", AttackStatus::SCANNING);
  ASSERT_NOT_NULL(statusResult);
  ASSERT_EQUAL((int)statusResult->type, (int)ResultType::STATUS);
  ASSERT_EQUAL((int)statusResult->status, (int)AttackStatus::SCANNING);
  delete statusResult;

  // Test ERROR result creation
  AttackResult* errorResult = ResultBuilder::createError("Test Error");
  ASSERT_NOT_NULL(errorResult);
  ASSERT_EQUAL((int)errorResult->type, (int)ResultType::ERROR);
  ASSERT_EQUAL((int)errorResult->status, (int)AttackStatus::ERROR);
  delete errorResult;

  return true;
}

bool AttackTestSuite::testResultCollection() {
  MockAttack attack("Result Collection Test");

  attack.generateTestResults(10);
  ASSERT_EQUAL(attack.getResultCount(), 10);

  AttackResult* result = attack.getResult(0);
  ASSERT_NOT_NULL(result);
  ASSERT_EQUAL((int)result->type, (int)ResultType::SCAN);

  attack.clearResults();
  ASSERT_EQUAL(attack.getResultCount(), 0);

  return true;
}

bool AttackTestSuite::testStatusManagement() {
  MockAttack attack("Status Test");

  ASSERT_EQUAL((int)attack.getStatus(), (int)AttackStatus::IDLE);

  attack.start();
  ASSERT_EQUAL((int)attack.getStatus(), (int)AttackStatus::SCANNING);

  attack.stop();
  ASSERT_EQUAL((int)attack.getStatus(), (int)AttackStatus::IDLE);

  return true;
}

// ============= PERFORMANCE BENCHMARK IMPLEMENTATION =============

BenchmarkResult* PerformanceBenchmark::getResult(const char* benchName) {
  for (auto* result : results) {
    if (strcmp(result->name, benchName) == 0) {
      return result;
    }
  }
  return nullptr;
}

void PerformanceBenchmark::recordMeasurement(const char* benchName, uint32_t timeMs) {
  BenchmarkResult* existing = getResult(benchName);

  if (existing) {
    existing->runCount++;
    existing->avgTimeMs = (existing->avgTimeMs + timeMs) / 2;
    if (timeMs < existing->minTimeMs) existing->minTimeMs = timeMs;
    if (timeMs > existing->maxTimeMs) existing->maxTimeMs = timeMs;
  } else {
    BenchmarkResult* newResult = new BenchmarkResult();
    newResult->name = benchName;
    newResult->minTimeMs = timeMs;
    newResult->maxTimeMs = timeMs;
    newResult->avgTimeMs = timeMs;
    newResult->runCount = 1;
    results.push_back(newResult);
  }
}

void PerformanceBenchmark::printAllResults() {
  Serial.println("\n========== Performance Benchmark Results ==========");
  for (auto* result : results) {
    Serial.printf("%-30s | Min: %u ms | Avg: %u ms | Max: %u ms | Runs: %u\n",
      result->name, result->minTimeMs, result->avgTimeMs, result->maxTimeMs, result->runCount);
  }
  Serial.println("===================================================\n");
}

// ============= MEMORY PROFILER IMPLEMENTATION =============

// Implementation already in header as template class

// ============= TEST RUNNER UTILITY =============

class TestRunner {
public:
  static void runAllTests() {
    Serial.println("\n\n");
    Serial.println("╔════════════════════════════════════════════════════╗");
    Serial.println("║        ESP32-S3 Security Platform - Test Suite     ║");
    Serial.println("╚════════════════════════════════════════════════════╝");

    // Run hardware tests
    HardwareTestSuite hwTests;
    hwTests.runAll();

    // Run attack framework tests
    AttackTestSuite attackTests;
    attackTests.runAll();

    // Print memory report
    MemoryProfiler::getInstance().printReport();

    Serial.println("\n✓ All test suites completed!");
  }

  static void runQuickTests() {
    AttackTestSuite tests;
    tests.runAll();
  }

  static void benchmarkAttackExecution() {
    Serial.println("\n========== Benchmarking Attack Execution ==========");

    PerformanceBenchmark& bench = PerformanceBenchmark::getInstance();

    for (int i = 0; i < 5; i++) {
      MockAttack attack("Benchmark Test");
      attack.begin();

      bench.startMeasure("Attack Start");
      attack.start();
      bench.stopMeasure();

      delay(100);

      bench.startMeasure("Attack Update");
      attack.update();
      bench.stopMeasure();

      bench.startMeasure("Attack Stop");
      attack.stop();
      bench.stopMeasure();

      attack.cleanup();
    }

    bench.printAllResults();
  }
};

#endif // TestRunner
