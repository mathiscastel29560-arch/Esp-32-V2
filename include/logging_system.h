#ifndef LOGGING_SYSTEM_H
#define LOGGING_SYSTEM_H

#include <Arduino.h>
#include <vector>
#include <cstring>
#include "attack_framework.h"

// ============= LOG LEVELS =============

enum class LogLevel {
  DEBUG = 0,    // Detailed diagnostic info
  INFO = 1,     // General information
  WARN = 2,     // Warning messages
  ERROR = 3     // Error messages
};

// ============= LOG ENTRY STRUCTURE =============

struct LogEntry {
  LogLevel level;
  uint32_t timestamp;
  char module[32];      // "WiFi", "BLE", "NRF24", etc.
  char message[256];    // Log message

  LogEntry() : level(LogLevel::INFO), timestamp(0) {
    memset(module, 0, 32);
    memset(message, 0, 256);
  }
};

// ============= LOGGER CLASS =============

class Logger {
public:
  static Logger& getInstance() {
    static Logger instance;
    return instance;
  }

  // Logging methods
  void debug(const char* module, const char* msg) {
    log(LogLevel::DEBUG, module, msg);
  }

  void info(const char* module, const char* msg) {
    log(LogLevel::INFO, module, msg);
  }

  void warn(const char* module, const char* msg) {
    log(LogLevel::WARN, module, msg);
  }

  void error(const char* module, const char* msg) {
    log(LogLevel::ERROR, module, msg);
  }

  // Core logging
  void log(LogLevel level, const char* module, const char* msg);

  // Results tracking
  void logAttackStart(const char* attackName, const char* target = "") {
    char msg[256];
    snprintf(msg, 255, "ATTACK START: %s %s", attackName, target);
    info("ATTACK", msg);
  }

  void logAttackEnd(const char* attackName, AttackStatus status) {
    char msg[256];
    const char* statusStr = getStatusString(status);
    snprintf(msg, 255, "ATTACK END: %s [%s]", attackName, statusStr);
    status == AttackStatus::SUCCESS ? info("ATTACK", msg) : warn("ATTACK", msg);
  }

  void logResult(const char* module, const AttackResult* result);

  // History access
  uint16_t getLogCount() const { return logs.size(); }
  LogEntry* getLog(uint16_t index) {
    if (index < logs.size()) return logs[index];
    return nullptr;
  }

  void clearLogs() {
    for (auto* log : logs) delete log;
    logs.clear();
  }

  // Statistics
  uint16_t getErrorCount() const;
  uint16_t getWarnCount() const;

  void setLogLevel(LogLevel level) { minLogLevel = level; }

private:
  Logger() : minLogLevel(LogLevel::DEBUG) {}

  std::vector<LogEntry*> logs;
  LogLevel minLogLevel;
  static const uint16_t MAX_LOGS = 500;

  const char* getStatusString(AttackStatus status) const {
    switch (status) {
      case AttackStatus::SUCCESS: return "SUCCESS";
      case AttackStatus::PARTIAL: return "PARTIAL";
      case AttackStatus::FAILED: return "FAILED";
      case AttackStatus::ERROR: return "ERROR";
      default: return "UNKNOWN";
    }
  }
};

// ============= RESULTS FORMATTER =============

class ResultsFormatter {
public:
  static ResultsFormatter& getInstance() {
    static ResultsFormatter instance;
    return instance;
  }

  // Format result as human-readable string
  const char* format(const AttackResult* result);

  // Format result with color codes for terminal
  const char* formatColored(const AttackResult* result);

  // Format RSSI indicator
  const char* formatRSSI(int8_t rssi);

  // Format status indicator
  const char* formatStatus(AttackStatus status);

  // Format data as hex string
  const char* formatDataHex(const uint8_t* data, uint16_t len, uint16_t maxDisplay = 16);

private:
  ResultsFormatter() {}
  char buffer[512];
  char hexBuffer[256];
};

// ============= RESULTS EXPORTER =============

class ResultsExporter {
public:
  static ResultsExporter& getInstance() {
    static ResultsExporter instance;
    return instance;
  }

  // Export attack results to CSV format
  bool exportToCSV(const Attack* attack, char* output, uint16_t maxLen);

  // Export attack results to JSON format
  bool exportToJSON(const Attack* attack, char* output, uint16_t maxLen);

  // Export log history to CSV
  bool exportLogsToCSV(char* output, uint16_t maxLen);

  // Format result entry as CSV row
  const char* formatResultCSV(const AttackResult* result);

  // Format result entry as JSON object
  const char* formatResultJSON(const AttackResult* result);

private:
  ResultsExporter() {}
  char csvBuffer[1024];
  char jsonBuffer[1024];
};

// ============= STATISTICS TRACKER =============

struct AttackStats {
  const char* attackName;
  uint16_t runCount;
  uint16_t successCount;
  uint16_t failureCount;
  uint32_t totalDurationMs;
  uint32_t minDurationMs;
  uint32_t maxDurationMs;
  int8_t avgRSSI;
};

class StatsTracker {
public:
  static StatsTracker& getInstance() {
    static StatsTracker instance;
    return instance;
  }

  // Record attack execution
  void recordAttack(const char* attackName, AttackStatus status, uint32_t durationMs, int8_t rssi = 0);

  // Get statistics for attack
  AttackStats* getStats(const char* attackName);

  // Get all statistics
  uint16_t getAllStats(AttackStats* output, uint16_t maxCount);

  // Clear statistics
  void clearStats() {
    for (auto* stat : stats) delete stat;
    stats.clear();
  }

private:
  StatsTracker() {}
  std::vector<AttackStats*> stats;
  static const uint16_t MAX_TRACKED_ATTACKS = 50;

  AttackStats* findOrCreate(const char* attackName);
};

#endif // LOGGING_SYSTEM_H
