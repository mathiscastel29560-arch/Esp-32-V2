#ifndef VALIDATION_H
#define VALIDATION_H

#include <Arduino.h>

// Precondition checks
#define REQUIRE(condition) \
  if (!(condition)) { \
    Serial.printf("[REQUIRE FAILED] %s at %s:%d\n", #condition, __FILE__, __LINE__); \
    return false; \
  }

#define REQUIRE_NULL_CHECK(ptr) \
  if ((ptr) == nullptr) { \
    Serial.printf("[NULL CHECK FAILED] %s at %s:%d\n", #ptr, __FILE__, __LINE__); \
    return false; \
  }

// Postcondition checks
#define ENSURE(condition) \
  if (!(condition)) { \
    Serial.printf("[ENSURE FAILED] %s at %s:%d\n", #condition, __FILE__, __LINE__); \
  }

// Invariant checks
#define INVARIANT(condition) \
  if (!(condition)) { \
    Serial.printf("[INVARIANT BROKEN] %s at %s:%d\n", #condition, __FILE__, __LINE__); \
  }

// Range validation
#define VALIDATE_RANGE(value, min, max) \
  if ((value) < (min) || (value) > (max)) { \
    Serial.printf("[RANGE FAILED] %d not in [%d, %d] at %s:%d\n", \
      (int)(value), (int)(min), (int)(max), __FILE__, __LINE__); \
    return false; \
  }

// Size validation
#define VALIDATE_SIZE(size, max) \
  if ((size) > (max)) { \
    Serial.printf("[SIZE EXCEEDED] %u > %u at %s:%d\n", \
      (unsigned)(size), (unsigned)(max), __FILE__, __LINE__); \
    return false; \
  }

// Pointer dereference safety
class SafePointer {
public:
  template<typename T>
  static T* checked(T* ptr, const char* name = "pointer") {
    if (!ptr) {
      Serial.printf("[DEREF NULL] %s at\n", name);
      return nullptr;
    }
    return ptr;
  }

  template<typename T>
  static bool isValid(T* ptr) {
    return ptr != nullptr;
  }
};

#endif
