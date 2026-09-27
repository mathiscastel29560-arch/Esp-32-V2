#ifndef RATE_LIMITER_H
#define RATE_LIMITER_H

#include <Arduino.h>

class RateLimiter {
public:
  RateLimiter(uint16_t maxRequests = 100, uint32_t windowMs = 1000)
    : maxReqs(maxRequests), window(windowMs), reqCount(0), windowStart(millis()) {}

  bool tryRequest() {
    uint32_t now = millis();

    if (now - windowStart > window) {
      reqCount = 0;
      windowStart = now;
    }

    if (reqCount < maxReqs) {
      reqCount++;
      return true;
    }

    return false;
  }

  void reset() {
    reqCount = 0;
    windowStart = millis();
  }

  float getUsagePercent() const {
    return (float)reqCount / maxReqs * 100.0f;
  }

  uint32_t getTimeUntilReset() const {
    uint32_t elapsed = millis() - windowStart;
    if (elapsed > window) return 0;
    return window - elapsed;
  }

  void printStatus() {
    Serial.printf("Rate: %u/%u (%.1f%%), Reset in %lu ms\n",
      reqCount, maxReqs, getUsagePercent(), getTimeUntilReset());
  }

private:
  uint16_t maxReqs;
  uint32_t window;
  uint16_t reqCount;
  uint32_t windowStart;
};

class TokenBucket {
public:
  TokenBucket(float refillRate = 1.0f, uint32_t maxTokens = 100)
    : rate(refillRate), maxToks(maxTokens), tokens((float)maxTokens),
      lastRefillTime(millis()) {}

  bool consumeTokens(float count = 1.0f) {
    refillTokens();

    if (tokens >= count) {
      tokens -= count;
      return true;
    }

    return false;
  }

  float getAvailableTokens() const {
    return tokens;
  }

private:
  void refillTokens() {
    uint32_t now = millis();
    uint32_t elapsed = now - lastRefillTime;

    float tokensToAdd = (elapsed / 1000.0f) * rate;
    tokens = (tokens + tokensToAdd > maxToks) ? maxToks : tokens + tokensToAdd;

    lastRefillTime = now;
  }

  float rate;
  uint32_t maxToks;
  float tokens;
  uint32_t lastRefillTime;
};

#endif
