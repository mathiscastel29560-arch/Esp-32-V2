#ifndef DISTRIBUTED_CACHE_H
#define DISTRIBUTED_CACHE_H

#include <Arduino.h>
#include <map>
#include <vector>
#include <cstring>

template<typename T>
class CacheEntry {
public:
  CacheEntry(const T& v) : value(v), timestamp(millis()), accessCount(0) {}

  T& getValue() {
    accessCount++;
    lastAccessTime = millis();
    return value;
  }

  bool isStale(uint32_t maxAgeMsL) const {
    return (millis() - timestamp) > maxAgeMsL;
  }

  uint32_t getAccessCount() const { return accessCount; }
  uint32_t getAgeMs() const { return millis() - timestamp; }

private:
  T value;
  uint32_t timestamp;
  uint32_t lastAccessTime;
  uint32_t accessCount;
};

template<typename KeyT, typename ValueT>
class DistributedCache {
public:
  static DistributedCache& getInstance() {
    static DistributedCache instance;
    return instance;
  }

  void put(const KeyT& key, const ValueT& value) {
    auto it = cache.find(key);
    if (it != cache.end()) {
      delete it->second;
    }
    cache[key] = new CacheEntry<ValueT>(value);
  }

  bool get(const KeyT& key, ValueT& output) {
    auto it = cache.find(key);
    if (it != cache.end()) {
      output = it->second->getValue();
      return true;
    }
    return false;
  }

  void remove(const KeyT& key) {
    auto it = cache.find(key);
    if (it != cache.end()) {
      delete it->second;
      cache.erase(it);
    }
  }

  void clear() {
    for (auto& pair : cache) {
      delete pair.second;
    }
    cache.clear();
  }

  uint16_t size() const { return cache.size(); }

  void evictStale(uint32_t maxAgeMs) {
    auto it = cache.begin();
    while (it != cache.end()) {
      if (it->second->isStale(maxAgeMs)) {
        delete it->second;
        it = cache.erase(it);
      } else {
        ++it;
      }
    }
  }

  void printStats() {
    Serial.printf("Cache size: %u entries\n", cache.size());
    uint32_t totalAccess = 0;
    for (auto& pair : cache) {
      totalAccess += pair.second->getAccessCount();
    }
    Serial.printf("Total accesses: %lu\n", totalAccess);
  }

private:
  DistributedCache() {}

  std::map<KeyT, CacheEntry<ValueT>*> cache;
};

#endif
