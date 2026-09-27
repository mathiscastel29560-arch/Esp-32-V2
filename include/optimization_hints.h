#ifndef OPTIMIZATION_HINTS_H
#define OPTIMIZATION_HINTS_H

#include <Arduino.h>

// Force inline for hot paths
#define FORCE_INLINE __attribute__((always_inline)) inline

// Restrict pointers for better optimization
#define RESTRICT __restrict__

// Likely branch prediction hints
#define LIKELY(x) __builtin_expect(!!(x), 1)
#define UNLIKELY(x) __builtin_expect(!!(x), 0)

// Memory ordering
#define VOLATILE_READ(ptr) *(volatile typeof(ptr)*)(ptr)
#define VOLATILE_WRITE(ptr, val) *(volatile typeof(&val)*)(ptr) = (val)

// Function attributes
#define PURE_FUNCTION __attribute__((pure))
#define CONST_FUNCTION __attribute__((const))
#define SECTION(name) __attribute__((section(#name)))

// For PSRAM allocation (when available)
#define PSRAM_ATTR IRAM_ATTR

// Performance profiling markers
class PerformanceMarker {
public:
  PerformanceMarker(const char* name) : startTime(micros()), name(name) {}

  ~PerformanceMarker() {
    uint32_t elapsed = micros() - startTime;
    if (elapsed > 1000) {
      Serial.printf("[PERF] %s: %lu us\n", name, elapsed);
    }
  }

private:
  uint32_t startTime;
  const char* name;
};

#define MEASURE_PERF(name) PerformanceMarker _perf_marker(name)

// Compile-time assertions
#define STATIC_ASSERT(expr) static_assert(expr, #expr)

#endif
