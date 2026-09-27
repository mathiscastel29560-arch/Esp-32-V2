#ifndef BATCH_PROCESSOR_H
#define BATCH_PROCESSOR_H

#include <Arduino.h>
#include <vector>
#include <functional>

template<typename T>
class BatchProcessor {
public:
  BatchProcessor(uint16_t batchSize = 100, uint32_t timeoutMs = 1000)
    : maxBatchSize(batchSize), timeout(timeoutMs), lastFlushTime(millis()) {}

  void add(const T& item) {
    if (items.size() >= maxBatchSize) {
      flush();
    }
    items.push_back(item);
  }

  void addMultiple(const std::vector<T>& newItems) {
    for (const auto& item : newItems) {
      add(item);
    }
  }

  void flush() {
    if (!items.empty() && onFlush) {
      onFlush(items);
      items.clear();
      lastFlushTime = millis();
    }
  }

  void setFlushCallback(std::function<void(const std::vector<T>&)> cb) {
    onFlush = cb;
  }

  void updateIfNeeded() {
    if (millis() - lastFlushTime > timeout && !items.empty()) {
      flush();
    }
  }

  uint16_t getItemCount() const { return items.size(); }
  bool isFull() const { return items.size() >= maxBatchSize; }

private:
  std::vector<T> items;
  uint16_t maxBatchSize;
  uint32_t timeout;
  uint32_t lastFlushTime;
  std::function<void(const std::vector<T>&)> onFlush;
};

#endif
