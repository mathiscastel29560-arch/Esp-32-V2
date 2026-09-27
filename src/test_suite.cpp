#include "test_suite.h"
#include "debug_logger.h"
#include <Arduino.h>
#include <map>

void TestSuite::addTest(const std::string& name, TestFunction test) {
  tests[name] = test;
  DebugLogger::printf("[TestSuite] Registered test: %s\n", name.c_str());
}

void TestSuite::runAll() {
  results.clear();

  Serial.println("\n╔════════════════════════════════════════╗");
  Serial.println("║        RUNNING TEST SUITE              ║");
  Serial.println("╚════════════════════════════════════════╝");

  uint32_t passCount = 0;
  uint32_t failCount = 0;
  uint32_t totalTime = 0;

  for (const auto& pair : tests) {
    uint32_t startTime = millis();
    bool passed = pair.second();
    uint32_t executionTime = millis() - startTime;

    TestResult result;
    result.name = pair.first;
    result.passed = passed;
    result.executionTime = executionTime;
    result.message = passed ? "PASS" : "FAIL";

    results.push_back(result);
    logTestResult(result);

    if (passed) {
      passCount++;
    } else {
      failCount++;
    }

    totalTime += executionTime;
  }

  printSummary();

  Serial.printf("\n✅ Passed: %u\n", passCount);
  Serial.printf("❌ Failed: %u\n", failCount);
  Serial.printf("⏱️  Total Time: %u ms\n", totalTime);
}

bool TestSuite::runTest(const std::string& name) {
  if (tests.find(name) == tests.end()) {
    DebugLogger::printf("[TestSuite] Test not found: %s\n", name.c_str());
    return false;
  }

  uint32_t startTime = millis();
  bool passed = tests[name]();
  uint32_t executionTime = millis() - startTime;

  TestResult result;
  result.name = name;
  result.passed = passed;
  result.executionTime = executionTime;
  result.message = passed ? "PASS" : "FAIL";

  results.push_back(result);
  logTestResult(result);

  return passed;
}

void TestSuite::printSummary() const {
  Serial.println("\n📋 Test Results:");
  Serial.printf("%-40s | %-8s | %s\n", "Test Name", "Status", "Time");
  Serial.println(std::string(70, '-').c_str());

  for (const auto& result : results) {
    const char* status = result.passed ? "✅ PASS" : "❌ FAIL";
    Serial.printf("%-40s | %8s | %u ms\n",
      result.name.c_str(), status, result.executionTime);
  }
}

void TestSuite::runStressTest(uint32_t durationSeconds) {
  Serial.println("\n╔════════════════════════════════════════╗");
  Serial.println("║         RUNNING STRESS TEST            ║");
  Serial.println("╚════════════════════════════════════════╝");

  uint32_t startTime = millis();
  uint32_t iterations = 0;
  uint32_t errors = 0;
  uint32_t maxTime = 0;
  uint32_t minTime = UINT32_MAX;

  while (millis() - startTime < durationSeconds * 1000) {
    for (const auto& pair : tests) {
      uint32_t testStart = millis();
      bool passed = pair.second();
      uint32_t testTime = millis() - testStart;

      if (!passed) errors++;
      maxTime = std::max(maxTime, testTime);
      minTime = std::min(minTime, testTime);

      iterations++;
    }
  }

  Serial.println("\n📊 Stress Test Results:");
  Serial.printf("  Duration: %u seconds\n", durationSeconds);
  Serial.printf("  Iterations: %u\n", iterations);
  Serial.printf("  Errors: %u\n", errors);
  Serial.printf("  Success Rate: %.1f%%\n", (100.0f * (iterations - errors)) / iterations);
  Serial.printf("  Min Time: %u ms\n", minTime);
  Serial.printf("  Max Time: %u ms\n", maxTime);
}

void TestSuite::logTestResult(const TestResult& result) {
  const char* icon = result.passed ? "✅" : "❌";
  DebugLogger::printf("[TestSuite] %s %s (%u ms)\n",
    icon, result.name.c_str(), result.executionTime);
}
