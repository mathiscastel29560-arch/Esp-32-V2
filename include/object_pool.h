#ifndef OBJECT_POOL_H
#define OBJECT_POOL_H

#include <Arduino.h>
#include <vector>

template<typename T>
class ObjectPool {
public:
  ObjectPool(uint16_t poolSize) : maxSize(poolSize) {}

  ~ObjectPool() {
    for (auto* obj : available) {
      delete obj;
    }
    available.clear();
  }

  T* acquire() {
    if (available.empty()) {
      if (created < maxSize) {
        T* obj = new T();
        created++;
        return obj;
      }
      return nullptr;
    }

    T* obj = available.back();
    available.pop_back();
    inUse++;
    return obj;
  }

  void release(T* obj) {
    if (obj && inUse > 0) {
      available.push_back(obj);
      inUse--;
    }
  }

  uint16_t getAvailableCount() const { return available.size(); }
  uint16_t getInUseCount() const { return inUse; }
  uint32_t getTotalCreated() const { return created; }

private:
  std::vector<T*> available;
  uint16_t maxSize;
  uint16_t inUse = 0;
  uint32_t created = 0;
};

#endif
