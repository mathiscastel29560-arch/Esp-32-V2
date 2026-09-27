#include "performance_profiler.h"
#include "debug_logger.h"
#include <Arduino.h>
#include <algorithm>

void PerformanceProfiler::startSection(const std::string& name) {
  sectionStartTimes[name] = millis();
}

uint32_t PerformanceProfiler::endSection(const std::string& name) {
  if (sectionStartTimes.find(name) == sectionStartTimes.end()) {
    return 0;
  }

  uint32_t executionTime = millis() - sectionStartTimes[name];
  updateMetric(name, executionTime);
  sectionStartTimes.erase(name);

  return executionTime;
}

PerformanceProfiler::Metrics PerformanceProfiler::getMetrics(const std::string& name) {
  if (metricsMap.find(name) == metricsMap.end()) {
    return {0, 0, 0, 0, 0};
  }
  return metricsMap[name];
}

void PerformanceProfiler::printReport() {
  Serial.println("\n╔════════════════════════════════════════╗");
  Serial.println("║     PERFORMANCE PROFILING REPORT       ║");
  Serial.println("╚════════════════════════════════════════╝");

  Serial.printf("\n%-30s | %-8s | %-8s | %-8s | %s\n",
    "Section", "Min", "Max", "Avg", "Calls");
  Serial.println(std::string(80, '-').c_str());

  for (const auto& pair : metricsMap) {
    const Metrics& m = pair.second;
    Serial.printf("%-30s | %6ums | %6ums | %6ums | %u\n",
      pair.first.c_str(), m.minTime, m.maxTime, m.avgTime, m.callCount);
  }

  auto memSnap = getMemorySnapshot();
  Serial.println("\n📊 Memory Snapshot:");
  Serial.printf("  Free Heap: %u KB\n", memSnap.freeHeap / 1024);
  Serial.printf("  Free PSRAM: %u KB\n", memSnap.freeExternalRAM / 1024);
  Serial.printf("  Largest Block: %u KB\n", memSnap.largestFreeBlock / 1024);
  Serial.printf("  Fragmentation: %u%%\n", memSnap.fragmentation);

  if (!isHealthy()) {
    Serial.println("\n⚠️  WARNING: System health degraded!");
  }
}

void PerformanceProfiler::reset() {
  sectionStartTimes.clear();
  metricsMap.clear();
  DebugLogger::println("[Profiler] Metrics reset");
}

float PerformanceProfiler::getAverageFPS() const {
  // Assuming main loop timing
  auto it = metricsMap.find("main_loop");
  if (it != metricsMap.end()) {
    if (it->second.avgTime > 0) {
      return 1000.0f / it->second.avgTime;
    }
  }
  return 0.0f;
}

bool PerformanceProfiler::isHealthy() const {
  auto memSnap = getMemorySnapshot();

  // Healthy if:
  // - Free heap > 50KB
  // - Fragmentation < 50%
  // - No execution times > 1000ms

  if (memSnap.freeHeap < 50 * 1024) return false;
  if (memSnap.fragmentation > 50) return false;

  for (const auto& pair : metricsMap) {
    if (pair.second.maxTime > 1000) return false;
  }

  return true;
}

void PerformanceProfiler::trackMemoryUsage() {
  lastSnapshot.freeHeap = ESP.getFreeHeap();
  lastSnapshot.freeExternalRAM = ESP.getFreePsram();

  // Estimate largest free block
  heap_caps_get_info(&lastSnapshot.largestFreeBlock, MALLOC_CAP_DEFAULT);

  // Calculate fragmentation (simplified)
  uint32_t totalFree = lastSnapshot.freeHeap + lastSnapshot.freeExternalRAM;
  uint32_t totalMem = 8 * 1024 * 1024;  // 8MB total
  lastSnapshot.fragmentation = 100 - (totalFree * 100 / totalMem);
}

PerformanceProfiler::MemorySnapshot PerformanceProfiler::getMemorySnapshot() const {
  // Note: This is approximate, recalculates on each call
  MemorySnapshot snap;
  snap.freeHeap = ESP.getFreeHeap();
  snap.freeExternalRAM = ESP.getFreePsram();
  snap.largestFreeBlock = snap.freeHeap / 2;  // Estimate
  snap.fragmentation = 15;  // Estimate

  return snap;
}

void PerformanceProfiler::updateMetric(const std::string& name, uint32_t executionTime) {
  if (metricsMap.find(name) == metricsMap.end()) {
    metricsMap[name] = {executionTime, executionTime, executionTime, 1, executionTime};
  } else {
    Metrics& m = metricsMap[name];
    m.minTime = std::min(m.minTime, executionTime);
    m.maxTime = std::max(m.maxTime, executionTime);
    m.callCount++;
    m.totalTime += executionTime;
    m.avgTime = m.totalTime / m.callCount;
  }
}
