#include "logging_system.h"

// ============= LOGGER IMPLEMENTATION =============

void Logger::log(LogLevel level, const char* module, const char* msg) {
  if (level < minLogLevel) return;

  if (logs.size() >= MAX_LOGS) {
    delete logs[0];
    logs.erase(logs.begin());
  }

  LogEntry* entry = new LogEntry();
  entry->level = level;
  entry->timestamp = millis();
  strncpy(entry->module, module, 31);
  strncpy(entry->message, msg, 255);
  logs.push_back(entry);

  // Also print to serial with timestamps
  const char* levelStr = "";
  uint16_t color = 0xFFFF;

  switch (level) {
    case LogLevel::DEBUG:
      levelStr = "[DEBUG]";
      color = 0x07E0; // Cyan
      break;
    case LogLevel::INFO:
      levelStr = "[INFO]";
      color = 0x07E0; // Green
      break;
    case LogLevel::WARN:
      levelStr = "[WARN]";
      color = 0xFFE0; // Yellow
      break;
    case LogLevel::ERROR:
      levelStr = "[ERROR]";
      color = 0xF800; // Red
      break;
  }

  Serial.printf("%u %s [%s] %s\n", entry->timestamp, levelStr, module, msg);
}

void Logger::logResult(const char* module, const AttackResult* result) {
  if (!result) return;

  char msg[256];
  const char* typeStr = "";

  switch (result->type) {
    case ResultType::SCAN:
      typeStr = "SCAN";
      break;
    case ResultType::PACKET:
      typeStr = "PACKET";
      break;
    case ResultType::CODE:
      typeStr = "CODE";
      break;
    case ResultType::STATUS:
      typeStr = "STATUS";
      break;
    case ResultType::ERROR:
      typeStr = "ERROR";
      break;
  }

  snprintf(msg, 255, "%s [%s] RSSI=%d", typeStr, result->description, result->rssi);

  if (result->status == AttackStatus::SUCCESS) {
    info(module, msg);
  } else if (result->status == AttackStatus::ERROR) {
    error(module, msg);
  } else {
    warn(module, msg);
  }
}

uint16_t Logger::getErrorCount() const {
  uint16_t count = 0;
  for (auto* log : logs) {
    if (log->level == LogLevel::ERROR) count++;
  }
  return count;
}

uint16_t Logger::getWarnCount() const {
  uint16_t count = 0;
  for (auto* log : logs) {
    if (log->level == LogLevel::WARN) count++;
  }
  return count;
}

// ============= RESULTS FORMATTER IMPLEMENTATION =============

const char* ResultsFormatter::format(const AttackResult* result) {
  if (!result) return "";

  memset(buffer, 0, 512);

  const char* typeStr = "";
  const char* statusStr = "";

  switch (result->type) {
    case ResultType::SCAN: typeStr = "SCAN"; break;
    case ResultType::PACKET: typeStr = "PACKET"; break;
    case ResultType::CODE: typeStr = "CODE"; break;
    case ResultType::STATUS: typeStr = "STATUS"; break;
    case ResultType::ERROR: typeStr = "ERROR"; break;
  }

  switch (result->status) {
    case AttackStatus::SUCCESS: statusStr = "✓ SUCCESS"; break;
    case AttackStatus::PARTIAL: statusStr = "⚠ PARTIAL"; break;
    case AttackStatus::FAILED: statusStr = "✗ FAILED"; break;
    case AttackStatus::ERROR: statusStr = "✗ ERROR"; break;
    default: statusStr = "? UNKNOWN"; break;
  }

  snprintf(buffer, 511, "%s | %s | %s | RSSI: %d dBm | Time: %u ms",
    typeStr, statusStr, result->description, result->rssi, result->timestamp);

  return buffer;
}

const char* ResultsFormatter::formatColored(const AttackResult* result) {
  // Same as format() - color codes handled at display level
  return format(result);
}

const char* ResultsFormatter::formatRSSI(int8_t rssi) {
  memset(buffer, 0, 512);

  if (rssi >= -50) {
    snprintf(buffer, 511, "████████░░ Excellent (%d dBm)", rssi);
  } else if (rssi >= -60) {
    snprintf(buffer, 511, "██████░░░░ Good (%d dBm)", rssi);
  } else if (rssi >= -70) {
    snprintf(buffer, 511, "████░░░░░░ Fair (%d dBm)", rssi);
  } else if (rssi >= -80) {
    snprintf(buffer, 511, "██░░░░░░░░ Poor (%d dBm)", rssi);
  } else {
    snprintf(buffer, 511, "░░░░░░░░░░ Very Poor (%d dBm)", rssi);
  }

  return buffer;
}

const char* ResultsFormatter::formatStatus(AttackStatus status) {
  switch (status) {
    case AttackStatus::IDLE: return "⏸ IDLE";
    case AttackStatus::SCANNING: return "🔍 SCANNING";
    case AttackStatus::ATTACKING: return "⚡ ATTACKING";
    case AttackStatus::SUCCESS: return "✓ SUCCESS";
    case AttackStatus::PARTIAL: return "⚠ PARTIAL";
    case AttackStatus::FAILED: return "✗ FAILED";
    case AttackStatus::ERROR: return "✗ ERROR";
    default: return "? UNKNOWN";
  }
}

const char* ResultsFormatter::formatDataHex(const uint8_t* data, uint16_t len, uint16_t maxDisplay) {
  memset(hexBuffer, 0, 256);

  uint16_t displayLen = (len < maxDisplay) ? len : maxDisplay;
  uint16_t pos = 0;

  for (uint16_t i = 0; i < displayLen && pos < 250; i++) {
    pos += snprintf(hexBuffer + pos, 250 - pos, "%02X ", data[i]);
  }

  if (len > maxDisplay) {
    snprintf(hexBuffer + pos, 256 - pos, "... (%u bytes total)", len);
  }

  return hexBuffer;
}

// ============= RESULTS EXPORTER IMPLEMENTATION =============

bool ResultsExporter::exportToCSV(const Attack* attack, char* output, uint16_t maxLen) {
  if (!attack || !output) return false;

  uint16_t written = 0;
  written += snprintf(output + written, maxLen - written,
    "Attack,Status,RSSI,Type,DataLen,Description,Timestamp\n");

  uint16_t resultCount = attack->getResultCount();
  for (uint16_t i = 0; i < resultCount && written < maxLen - 256; i++) {
    AttackResult* result = attack->getResult(i);
    if (result) {
      const char* csv = formatResultCSV(result);
      written += snprintf(output + written, maxLen - written, "%s\n", csv);
    }
  }

  return written < maxLen;
}

bool ResultsExporter::exportToJSON(const Attack* attack, char* output, uint16_t maxLen) {
  if (!attack || !output) return false;

  uint16_t written = 0;
  written += snprintf(output + written, maxLen - written,
    "{\"attack\":\"%s\",\"results\":[", attack->getName());

  uint16_t resultCount = attack->getResultCount();
  for (uint16_t i = 0; i < resultCount && written < maxLen - 512; i++) {
    AttackResult* result = attack->getResult(i);
    if (result) {
      if (i > 0) {
        written += snprintf(output + written, maxLen - written, ",");
      }
      const char* json = formatResultJSON(result);
      written += snprintf(output + written, maxLen - written, "%s", json);
    }
  }

  written += snprintf(output + written, maxLen - written, "]}");
  return written < maxLen;
}

bool ResultsExporter::exportLogsToCSV(char* output, uint16_t maxLen) {
  if (!output) return false;

  Logger& logger = Logger::getInstance();
  uint16_t written = 0;
  written += snprintf(output + written, maxLen - written,
    "Timestamp,Level,Module,Message\n");

  uint16_t logCount = logger.getLogCount();
  for (uint16_t i = 0; i < logCount && written < maxLen - 256; i++) {
    LogEntry* entry = logger.getLog(i);
    if (entry) {
      const char* levelStr = "";
      switch (entry->level) {
        case LogLevel::DEBUG: levelStr = "DEBUG"; break;
        case LogLevel::INFO: levelStr = "INFO"; break;
        case LogLevel::WARN: levelStr = "WARN"; break;
        case LogLevel::ERROR: levelStr = "ERROR"; break;
      }
      written += snprintf(output + written, maxLen - written,
        "%u,%s,%s,%s\n", entry->timestamp, levelStr, entry->module, entry->message);
    }
  }

  return written < maxLen;
}

const char* ResultsExporter::formatResultCSV(const AttackResult* result) {
  if (!result) return "";

  const char* typeStr = "";
  const char* statusStr = "";

  switch (result->type) {
    case ResultType::SCAN: typeStr = "SCAN"; break;
    case ResultType::PACKET: typeStr = "PACKET"; break;
    case ResultType::CODE: typeStr = "CODE"; break;
    case ResultType::STATUS: typeStr = "STATUS"; break;
    case ResultType::ERROR: typeStr = "ERROR"; break;
  }

  switch (result->status) {
    case AttackStatus::SUCCESS: statusStr = "SUCCESS"; break;
    case AttackStatus::PARTIAL: statusStr = "PARTIAL"; break;
    case AttackStatus::FAILED: statusStr = "FAILED"; break;
    case AttackStatus::ERROR: statusStr = "ERROR"; break;
    default: statusStr = "UNKNOWN"; break;
  }

  snprintf(csvBuffer, 1023, "\"%s\",\"%s\",%d,\"%s\",%u,\"%s\",%u",
    "Attack", statusStr, result->rssi, typeStr, result->dataLen, result->description, result->timestamp);

  return csvBuffer;
}

const char* ResultsExporter::formatResultJSON(const AttackResult* result) {
  if (!result) return "";

  const char* typeStr = "";
  const char* statusStr = "";

  switch (result->type) {
    case ResultType::SCAN: typeStr = "SCAN"; break;
    case ResultType::PACKET: typeStr = "PACKET"; break;
    case ResultType::CODE: typeStr = "CODE"; break;
    case ResultType::STATUS: typeStr = "STATUS"; break;
    case ResultType::ERROR: typeStr = "ERROR"; break;
  }

  switch (result->status) {
    case AttackStatus::SUCCESS: statusStr = "SUCCESS"; break;
    case AttackStatus::PARTIAL: statusStr = "PARTIAL"; break;
    case AttackStatus::FAILED: statusStr = "FAILED"; break;
    case AttackStatus::ERROR: statusStr = "ERROR"; break;
    default: statusStr = "UNKNOWN"; break;
  }

  snprintf(jsonBuffer, 1023,
    "{\"type\":\"%s\",\"status\":\"%s\",\"rssi\":%d,\"dataLen\":%u,\"description\":\"%s\",\"timestamp\":%u}",
    typeStr, statusStr, result->rssi, result->dataLen, result->description, result->timestamp);

  return jsonBuffer;
}

// ============= STATS TRACKER IMPLEMENTATION =============

void StatsTracker::recordAttack(const char* attackName, AttackStatus status, uint32_t durationMs, int8_t rssi) {
  AttackStats* stat = findOrCreate(attackName);
  if (!stat) return;

  stat->runCount++;

  if (status == AttackStatus::SUCCESS) {
    stat->successCount++;
  } else {
    stat->failureCount++;
  }

  stat->totalDurationMs += durationMs;

  if (stat->minDurationMs == 0 || durationMs < stat->minDurationMs) {
    stat->minDurationMs = durationMs;
  }

  if (durationMs > stat->maxDurationMs) {
    stat->maxDurationMs = durationMs;
  }

  if (rssi != 0) {
    stat->avgRSSI = (stat->avgRSSI + rssi) / 2;
  }
}

AttackStats* StatsTracker::getStats(const char* attackName) {
  for (auto* stat : stats) {
    if (strcmp(stat->attackName, attackName) == 0) {
      return stat;
    }
  }
  return nullptr;
}

uint16_t StatsTracker::getAllStats(AttackStats* output, uint16_t maxCount) {
  uint16_t count = (stats.size() < maxCount) ? stats.size() : maxCount;
  for (uint16_t i = 0; i < count; i++) {
    output[i] = *stats[i];
  }
  return count;
}

AttackStats* StatsTracker::findOrCreate(const char* attackName) {
  for (auto* stat : stats) {
    if (strcmp(stat->attackName, attackName) == 0) {
      return stat;
    }
  }

  if (stats.size() >= MAX_TRACKED_ATTACKS) return nullptr;

  AttackStats* newStat = new AttackStats();
  newStat->attackName = attackName;
  newStat->runCount = 0;
  newStat->successCount = 0;
  newStat->failureCount = 0;
  newStat->totalDurationMs = 0;
  newStat->minDurationMs = 0;
  newStat->maxDurationMs = 0;
  newStat->avgRSSI = 0;

  stats.push_back(newStat);
  return newStat;
}
