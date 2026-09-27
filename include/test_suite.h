#pragma once

#include <string>
#include <vector>
#include <functional>

// Unit test framework for ESP32
class TestSuite {
public:
  struct TestResult {
    std::string name;
    bool passed;
    std::string message;
    uint32_t executionTime;
  };

  using TestFunction = std::function<bool()>;

  static TestSuite& getInstance() {
    static TestSuite instance;
    return instance;
  }

  // Register test
  void addTest(const std::string& name, TestFunction test);

  // Run all tests
  void runAll();

  // Run specific test
  bool runTest(const std::string& name);

  // Get results
  std::vector<TestResult> getResults() const { return results; }

  // Get pass/fail summary
  void printSummary() const;

  // Run stress test
  void runStressTest(uint32_t durationSeconds);

private:
  TestSuite() = default;

  std::map<std::string, TestFunction> tests;
  std::vector<TestResult> results;

  // Helper
  void logTestResult(const TestResult& result);
};

// Assertion macros
#define ASSERT_TRUE(condition, msg) \
  if (!(condition)) { return false; }

#define ASSERT_FALSE(condition, msg) \
  if ((condition)) { return false; }

#define ASSERT_EQUAL(a, b, msg) \
  if ((a) != (b)) { return false; }

#define ASSERT_NOT_EQUAL(a, b, msg) \
  if ((a) == (b)) { return false; }

#endif // TEST_SUITE_H
