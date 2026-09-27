#pragma once

#include <string>
#include <map>
#include <cstdint>

// Performance profiling and monitoring
class PerformanceProfiler {
public:
  struct Metrics {
    uint32_t minTime;      // Minimum execution time (ms)
    uint32_t maxTime;      // Maximum execution time (ms)
    uint32_t avgTime;      // Average execution time (ms)
    uint32_t callCount;    // Total calls
    uint64_t totalTime;    // Total time spent (ms)
  };

  static PerformanceProfiler& getInstance() {
    static PerformanceProfiler instance;
    return instance;
  }

  // Start timing a section
  void startSection(const std::string& name);

  // End timing a section
  uint32_t endSection(const std::string& name);

  // Get metrics for a section
  Metrics getMetrics(const std::string& name);

  // Print all metrics
  void printReport();

  // Reset all metrics
  void reset();

  // Get average FPS
  float getAverageFPS() const;

  // Check system health
  bool isHealthy() const;

  // Memory usage tracking
  void trackMemoryUsage();

  // Get memory snapshot
  struct MemorySnapshot {
    uint32_t freeHeap;
    uint32_t freeExternalRAM;
    uint32_t largestFreeBlock;
    uint8_t fragmentation;
  };

  MemorySnapshot getMemorySnapshot() const;

private:
  PerformanceProfiler() = default;

  std::map<std::string, uint32_t> sectionStartTimes;
  std::map<std::string, Metrics> metricsMap;
  MemorySnapshot lastSnapshot;

  // Helper
  void updateMetric(const std::string& name, uint32_t executionTime);
};

#endif // PERFORMANCE_PROFILER_H
