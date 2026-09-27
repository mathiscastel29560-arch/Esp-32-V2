#ifndef METRICS_H
#define METRICS_H

#include <Arduino.h>
#include <map>
#include <vector>

struct MetricData {
  uint32_t value;
  uint32_t minValue;
  uint32_t maxValue;
  uint32_t avgValue;
  uint32_t sampleCount;
  uint32_t timestamp;

  MetricData() : value(0), minValue(UINT32_MAX), maxValue(0),
                 avgValue(0), sampleCount(0), timestamp(millis()) {}

  void addSample(uint32_t sample) {
    value = sample;
    if (sample < minValue) minValue = sample;
    if (sample > maxValue) maxValue = sample;

    uint32_t total = (avgValue * sampleCount) + sample;
    sampleCount++;
    avgValue = total / sampleCount;

    timestamp = millis();
  }
};

class MetricsCollector {
public:
  static MetricsCollector& getInstance() {
    static MetricsCollector instance;
    return instance;
  }

  void recordMetric(const char* name, uint32_t value) {
    auto it = metrics.find(name);
    if (it != metrics.end()) {
      it->second->addSample(value);
    } else {
      MetricData* data = new MetricData();
      data->addSample(value);
      metrics[name] = data;
    }
  }

  MetricData* getMetric(const char* name) {
    auto it = metrics.find(name);
    return (it != metrics.end()) ? it->second : nullptr;
  }

  void printAllMetrics() {
    Serial.println("\n=== PERFORMANCE METRICS ===");
    for (auto& pair : metrics) {
      MetricData* data = pair.second;
      Serial.printf("%s:\n", pair.first);
      Serial.printf("  Current: %lu, Min: %lu, Max: %lu, Avg: %lu (samples: %lu)\n",
        data->value, data->minValue, data->maxValue, data->avgValue, data->sampleCount);
    }
    Serial.println("===========================\n");
  }

  void resetMetrics() {
    for (auto& pair : metrics) {
      delete pair.second;
    }
    metrics.clear();
  }

private:
  MetricsCollector() {}

  std::map<const char*, MetricData*> metrics;
};

// Convenience macros
#define METRIC_RECORD(name, value) \
  MetricsCollector::getInstance().recordMetric(name, value)

#define METRIC_TIME_START(name) \
  uint32_t _metric_start_##name = micros()

#define METRIC_TIME_END(name) \
  METRIC_RECORD(#name, micros() - _metric_start_##name)

#endif
