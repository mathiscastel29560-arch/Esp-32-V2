#ifndef CIRCUIT_BREAKER_H
#define CIRCUIT_BREAKER_H

#include <Arduino.h>

enum class CircuitState {
  CLOSED = 0,    // Normal operation
  OPEN = 1,      // Failing, reject requests
  HALF_OPEN = 2  // Testing if recovered
};

class CircuitBreaker {
public:
  CircuitBreaker(uint16_t failureThreshold = 5, uint32_t resetTimeoutMs = 30000)
    : threshold(failureThreshold), resetTimeout(resetTimeoutMs),
      failureCount(0), state(CircuitState::CLOSED), lastFailureTime(0) {}

  bool canExecute() {
    uint32_t now = millis();

    if (state == CircuitState::OPEN) {
      if (now - lastFailureTime > resetTimeout) {
        state = CircuitState::HALF_OPEN;
        failureCount = 0;
        return true;
      }
      return false;
    }

    return true;
  }

  void recordSuccess() {
    failureCount = 0;
    if (state == CircuitState::HALF_OPEN) {
      state = CircuitState::CLOSED;
    }
  }

  void recordFailure() {
    lastFailureTime = millis();
    failureCount++;

    if (failureCount >= threshold) {
      state = CircuitState::OPEN;
    }
  }

  CircuitState getState() const { return state; }
  uint16_t getFailureCount() const { return failureCount; }

  void printStatus() {
    Serial.println("=== CIRCUIT BREAKER ===");
    Serial.printf("State: ");
    switch (state) {
      case CircuitState::CLOSED: Serial.println("CLOSED"); break;
      case CircuitState::OPEN: Serial.println("OPEN"); break;
      case CircuitState::HALF_OPEN: Serial.println("HALF_OPEN"); break;
    }
    Serial.printf("Failures: %u / %u\n", failureCount, threshold);
    Serial.println("=======================\n");
  }

private:
  uint16_t threshold;
  uint32_t resetTimeout;
  uint16_t failureCount;
  CircuitState state;
  uint32_t lastFailureTime;
};

#endif
