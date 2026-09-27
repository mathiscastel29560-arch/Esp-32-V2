#ifndef ATTACK_FRAMEWORK_H
#define ATTACK_FRAMEWORK_H

#include <Arduino.h>
#include <vector>
#include <cstring>

// ============= ATTACK RESULT TYPES =============

enum class AttackStatus {
  IDLE = 0,        // Not running
  SCANNING = 1,    // In progress
  ATTACKING = 2,   // Active attack
  SUCCESS = 3,     // Completed successfully
  PARTIAL = 4,     // Partial success
  FAILED = 5,      // Failed
  ERROR = 6        // Error occurred
};

enum class ResultType {
  SCAN = 0,        // Scan result (list)
  PACKET = 1,      // Single packet capture
  CODE = 2,        // Code/data extracted
  STATUS = 3,      // Status message
  ERROR = 4        // Error message
};

// ============= RESULT STRUCTURE =============

struct AttackResult {
  ResultType type;
  AttackStatus status;
  uint32_t timestamp;
  int8_t rssi;           // Signal strength (-100 to 0 dBm)
  uint16_t dataLen;
  uint8_t* data;         // Payload (allocated)
  char description[128]; // Human readable

  AttackResult() :
    type(ResultType::STATUS),
    status(AttackStatus::IDLE),
    timestamp(0),
    rssi(-100),
    dataLen(0),
    data(nullptr) {
    memset(description, 0, 128);
  }

  ~AttackResult() {
    if (data) free(data);
  }

  void setData(const uint8_t* src, uint16_t len) {
    if (data) free(data);
    if (len > 0 && src) {
      data = (uint8_t*)malloc(len);
      memcpy(data, src, len);
      dataLen = len;
    }
  }
};

// ============= ATTACK BASE CLASS =============

class Attack {
public:
  Attack(const char* name) : attackName(name), isRunning(false), currentStatus(AttackStatus::IDLE) {
    memset(attackName, 0, 64);
    strncpy(attackName, name, 63);
  }

  virtual ~Attack() {}

  // Lifecycle
  virtual bool begin() { return true; }
  virtual bool start() {
    isRunning = true;
    currentStatus = AttackStatus::SCANNING;
    return true;
  }
  virtual bool stop() {
    isRunning = false;
    currentStatus = AttackStatus::IDLE;
    return true;
  }
  virtual void update() {}
  virtual bool cleanup() { return true; }

  // Configuration
  virtual bool setParameter(const char* key, const char* value) { return false; }
  virtual const char* getParameter(const char* key) { return nullptr; }

  // Results
  virtual uint16_t getResultCount() const { return results.size(); }
  virtual AttackResult* getResult(uint16_t index) {
    if (index < results.size()) return results[index];
    return nullptr;
  }
  virtual void clearResults() {
    for (auto* r : results) delete r;
    results.clear();
  }

  // Status
  AttackStatus getStatus() const { return currentStatus; }
  bool isActive() const { return isRunning; }
  const char* getName() const { return attackName; }
  uint32_t getUptimeMs() const { return millis() - startTime; }

  // Utility
  void addResult(AttackResult* result) {
    if (result) results.push_back(result);
  }

protected:
  char attackName[64];
  bool isRunning;
  AttackStatus currentStatus;
  uint32_t startTime;
  std::vector<AttackResult*> results;

  void setStatus(AttackStatus status) { currentStatus = status; }
};

// ============= ATTACK RESULT BUILDER (Convenience) =============

class ResultBuilder {
public:
  static AttackResult* createScan(const char* item, int8_t rssi = -100) {
    AttackResult* r = new AttackResult();
    r->type = ResultType::SCAN;
    r->status = AttackStatus::SUCCESS;
    r->rssi = rssi;
    r->timestamp = millis();
    strncpy(r->description, item, 127);
    return r;
  }

  static AttackResult* createPacket(const uint8_t* data, uint16_t len, const char* desc = "") {
    AttackResult* r = new AttackResult();
    r->type = ResultType::PACKET;
    r->status = AttackStatus::SUCCESS;
    r->timestamp = millis();
    r->setData(data, len);
    if (desc) strncpy(r->description, desc, 127);
    return r;
  }

  static AttackResult* createStatus(const char* msg, AttackStatus status = AttackStatus::SCANNING) {
    AttackResult* r = new AttackResult();
    r->type = ResultType::STATUS;
    r->status = status;
    r->timestamp = millis();
    strncpy(r->description, msg, 127);
    return r;
  }

  static AttackResult* createError(const char* error) {
    AttackResult* r = new AttackResult();
    r->type = ResultType::ERROR;
    r->status = AttackStatus::ERROR;
    r->timestamp = millis();
    strncpy(r->description, error, 127);
    return r;
  }
};

#endif // ATTACK_FRAMEWORK_H
