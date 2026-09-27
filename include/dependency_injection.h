#ifndef DEPENDENCY_INJECTION_H
#define DEPENDENCY_INJECTION_H

#include <Arduino.h>
#include <map>
#include <functional>

class ServiceContainer {
public:
  static ServiceContainer& getInstance() {
    static ServiceContainer instance;
    return instance;
  }

  template<typename T>
  void registerService(const char* name, std::function<T*()> factory) {
    services[name] = [factory]() -> void* {
      return (void*)factory();
    };
  }

  template<typename T>
  T* getService(const char* name) {
    auto it = services.find(name);
    if (it != services.end()) {
      return (T*)it->second();
    }
    return nullptr;
  }

  void printRegistry() {
    Serial.println("\n=== SERVICE REGISTRY ===");
    Serial.printf("Total services: %u\n", services.size());
    for (auto& pair : services) {
      Serial.printf("  - %s\n", pair.first);
    }
    Serial.println("========================\n");
  }

private:
  ServiceContainer() {}

  std::map<const char*, std::function<void*()>> services;
};

#endif
