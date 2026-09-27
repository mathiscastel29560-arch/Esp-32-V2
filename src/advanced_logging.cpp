#include "advanced_logging.h"
#include "logging_system.h"

// ============= DATA LOGGER IMPLEMENTATION =============

void DataLogger::logMetric(const char* metric, float value, const char* unit) {
  if (dataPoints.size() >= MAX_DATA_POINTS) {
    delete dataPoints[0];
    dataPoints.erase(dataPoints.begin());
  }

  DataPoint* point = new DataPoint();
  point->timestamp = millis();
  point->metric = metric;
  point->value = value;
  point->unit = unit;

  dataPoints.push_back(point);
}

void DataLogger::logAttackMetric(const char* attackName, const char* metric, float value) {
  char fullMetric[128];
  snprintf(fullMetric, 127, "%s.%s", attackName, metric);
  logMetric(fullMetric, value, "");
}

void DataLogger::logBatteryMetric(float voltage, uint8_t percent, float current) {
  logMetric("battery.voltage", voltage, "V");
  logMetric("battery.percent", percent, "%");
  logMetric("battery.current", current, "mA");
}

void DataLogger::logRFMetric(const char* module, int8_t rssi, float snr) {
  char metric[128];
  snprintf(metric, 127, "%s.rssi", module);
  logMetric(metric, rssi, "dBm");

  snprintf(metric, 127, "%s.snr", module);
  logMetric(metric, snr, "dB");
}

DataPoint* DataLogger::getDataPoint(uint16_t index) {
  if (index < dataPoints.size()) {
    return dataPoints[index];
  }
  return nullptr;
}

bool DataLogger::exportToCSV(char* output, uint16_t maxLen) {
  if (!output || maxLen < 64) return false;

  uint16_t written = 0;
  int ret = snprintf(output + written, maxLen - written,
    "timestamp,metric,value,unit\n");
  if (ret < 0 || ret >= (int)(maxLen - written)) return false;
  written += ret;

  for (auto* dp : dataPoints) {
    if (written >= maxLen - 100) break;
    ret = snprintf(output + written, maxLen - written,
      "%u,%s,%.2f,%s\n", dp->timestamp, dp->metric, dp->value, dp->unit);
    if (ret < 0 || ret >= (int)(maxLen - written)) break;
    written += ret;
  }

  output[written] = '\0';
  return written < maxLen;
}

bool DataLogger::exportToJSON(char* output, uint16_t maxLen) {
  if (!output || maxLen < 64) return false;

  uint16_t written = 0;
  int ret = snprintf(output + written, maxLen - written, "[");
  if (ret < 0 || ret >= (int)(maxLen - written)) return false;
  written += ret;

  for (uint16_t i = 0; i < dataPoints.size(); i++) {
    if (written >= maxLen - 150) break;
    if (i > 0) {
      ret = snprintf(output + written, maxLen - written, ",");
      if (ret < 0 || ret >= (int)(maxLen - written)) break;
      written += ret;
    }
    DataPoint* dp = dataPoints[i];
    ret = snprintf(output + written, maxLen - written,
      "{\"ts\":%u,\"m\":\"%s\",\"v\":%.2f,\"u\":\"%s\"}",
      dp->timestamp, dp->metric, dp->value, dp->unit);
    if (ret < 0 || ret >= (int)(maxLen - written)) break;
    written += ret;
  }

  ret = snprintf(output + written, maxLen - written, "]");
  if (ret >= 0) written += ret;
  output[written < maxLen ? written : maxLen - 1] = '\0';
  return written < maxLen;
}

void DataLogger::clearOldData(uint32_t maxAgeSeconds) {
  uint32_t cutoffTime = millis() - (maxAgeSeconds * 1000);

  for (uint16_t i = 0; i < dataPoints.size(); i++) {
    if (dataPoints[i]->timestamp < cutoffTime) {
      delete dataPoints[i];
      dataPoints.erase(dataPoints.begin() + i);
      i--;
    }
  }
}

void DataLogger::clearAllData() {
  for (auto* dp : dataPoints) delete dp;
  dataPoints.clear();
}

// ============= FILE LOGGER IMPLEMENTATION =============

bool FileLogger::begin(const char* sdCardPath) {
  strncpy(sdPath, sdCardPath, 63);
  sdPath[63] = '\0';
  ready = true; // TODO: Implement actual SD card init
  Logger::getInstance().info("FileLogger", "Initialized");
  return true;
}

void FileLogger::end() {
  ready = false;
}

bool FileLogger::writeLog(const char* filename, const char* data) {
  if (!ready) return false;
  // TODO: Implement file writing
  return true;
}

bool FileLogger::appendLog(const char* filename, const char* data) {
  if (!ready) return false;
  // TODO: Implement file append
  return true;
}

bool FileLogger::exportAttackResults(const char* attackName, const char* filename) {
  if (!ready) return false;
  Logger::getInstance().info("FileLogger", "Exporting attack results");
  return true;
}

bool FileLogger::exportSystemLogs(const char* filename) {
  if (!ready) return false;
  Logger::getInstance().info("FileLogger", "Exporting system logs");
  return true;
}

bool FileLogger::exportDataLog(const char* filename) {
  if (!ready) return false;
  Logger::getInstance().info("FileLogger", "Exporting data log");
  return true;
}

uint32_t FileLogger::getAvailableSpace() {
  // TODO: Implement actual space check
  return 1024 * 1024; // 1MB estimate
}

uint32_t FileLogger::getTotalSpace() {
  // TODO: Implement actual total space check
  return 32 * 1024 * 1024; // 32MB estimate
}

void FileLogger::printFileInfo() {
  Serial.println("\n========== File Logger Info ==========");
  Serial.printf("SD Card: %s\n", ready ? "Ready" : "Not Ready");
  Serial.printf("Available: %u KB\n", getAvailableSpace() / 1024);
  Serial.printf("Total: %u KB\n", getTotalSpace() / 1024);
  Serial.println("======================================\n");
}

// ============= CLOUD LOGGER IMPLEMENTATION =============

bool CloudLogger::configure(const CloudConfig& cfg) {
  config = cfg;
  Logger::getInstance().info("CloudLogger", "Configured");
  return true;
}

bool CloudLogger::connect() {
  connected = true;
  Logger::getInstance().info("CloudLogger", "Connected to cloud");
  lastSyncTime = millis();
  return true;
}

void CloudLogger::disconnect() {
  connected = false;
  Logger::getInstance().info("CloudLogger", "Disconnected from cloud");
}

bool CloudLogger::uploadAttackResults(const char* attackName, uint32_t duration) {
  if (!connected) return false;
  Logger::getInstance().info("CloudLogger", "Uploading attack results");
  return true;
}

bool CloudLogger::uploadBatteryStats(uint8_t percent, float voltage) {
  if (!connected) return false;
  Logger::getInstance().info("CloudLogger", "Uploading battery stats");
  return true;
}

bool CloudLogger::uploadSystemStats() {
  if (!connected) return false;
  Logger::getInstance().info("CloudLogger", "Uploading system stats");
  return true;
}

bool CloudLogger::syncBuffer() {
  if (!buffer.empty() && connected) {
    Logger::getInstance().info("CloudLogger", "Syncing buffer to cloud");
    lastSyncTime = millis();
    return true;
  }
  return false;
}

void CloudLogger::clearBuffer() {
  buffer.clear();
}

bool CloudLogger::sendHTTP(const char* payload) {
  // TODO: Implement HTTP request
  return true;
}

bool CloudLogger::sendHTTPS(const char* payload) {
  // TODO: Implement HTTPS request
  return true;
}

// ============= RING BUFFER IMPLEMENTATION =============

void RingBuffer::write(const char* message) {
  RingBufferEntry& entry = buffer[writePos];
  entry.timestamp = millis();
  strncpy(entry.message, message, 255);
  entry.message[255] = '\0';

  writePos = (writePos + 1) % maxSize;
  if (writePos == readPos) {
    readPos = (readPos + 1) % maxSize;
  }
}

const char* RingBuffer::read() {
  if (!hasData()) return nullptr;

  const char* msg = buffer[readPos].message;
  readPos = (readPos + 1) % maxSize;

  return msg;
}

void RingBuffer::printAll() {
  Serial.println("\n========== Ring Buffer Contents ==========");
  uint16_t pos = readPos;
  while (pos != writePos) {
    Serial.printf("[%u] %s\n", buffer[pos].timestamp, buffer[pos].message);
    pos = (pos + 1) % maxSize;
  }
  Serial.println("=========================================\n");
}

void RingBuffer::exportToBuffer(char* output, uint16_t maxLen) {
  if (!output || maxLen < 64) return;

  uint16_t written = 0;
  uint16_t pos = readPos;

  while (pos != writePos && written < maxLen - 100) {
    int ret = snprintf(output + written, maxLen - written,
      "[%u] %s\n", buffer[pos].timestamp, buffer[pos].message);
    if (ret < 0 || ret >= (int)(maxLen - written - 1)) break;
    written += ret;
    pos = (pos + 1) % maxSize;
  }

  output[written < maxLen ? written : maxLen - 1] = '\0';
}

// ============= LOG AGGREGATOR IMPLEMENTATION =============

LogAggregator::LogAggregator()
  : dataLoggingEnabled(true),
    fileLoggingEnabled(false),
    cloudLoggingEnabled(false),
    ringBufferEnabled(true),
    lastSyncTime(0) {

  ringBuffer = new RingBuffer(100);
}

void LogAggregator::begin() {
  Logger::getInstance().info("LogAggregator", "Initialized");
}

void LogAggregator::update() {
  uint32_t now = millis();

  if (cloudLoggingEnabled && now - lastSyncTime > SYNC_INTERVAL) {
    CloudLogger::getInstance().syncBuffer();
    lastSyncTime = now;
  }
}

void LogAggregator::generateMasterReport(char* output, uint16_t maxLen) {
  if (!output || maxLen < 128) return;

  uint16_t written = 0;
  int ret;

  ret = snprintf(output + written, maxLen - written, "LOG AGGREGATOR REPORT\n");
  if (ret < 0 || ret >= (int)(maxLen - written)) return;
  written += ret;

  ret = snprintf(output + written, maxLen - written, "Data Logging: %s\n",
    dataLoggingEnabled ? "ON" : "OFF");
  if (ret < 0 || ret >= (int)(maxLen - written)) return;
  written += ret;

  ret = snprintf(output + written, maxLen - written, "File Logging: %s\n",
    fileLoggingEnabled ? "ON" : "OFF");
  if (ret < 0 || ret >= (int)(maxLen - written)) return;
  written += ret;

  ret = snprintf(output + written, maxLen - written, "Cloud Logging: %s\n",
    cloudLoggingEnabled ? "ON" : "OFF");
  if (ret < 0 || ret >= (int)(maxLen - written)) return;
  written += ret;

  ret = snprintf(output + written, maxLen - written, "Data Points: %u\n",
    DataLogger::getInstance().getDataPointCount());
  if (ret >= 0) written += ret;
  output[written < maxLen ? written : maxLen - 1] = '\0';
}

void LogAggregator::printLogStatus() {
  Serial.println("\n========== Log Aggregator Status ==========");
  Serial.printf("Data Logging: %s\n", dataLoggingEnabled ? "✓ ON" : "✗ OFF");
  Serial.printf("File Logging: %s\n", fileLoggingEnabled ? "✓ ON" : "✗ OFF");
  Serial.printf("Cloud Logging: %s\n", cloudLoggingEnabled ? "✓ ON" : "✗ OFF");
  Serial.printf("Ring Buffer: %s\n", ringBufferEnabled ? "✓ ON" : "✗ OFF");
  Serial.printf("Data Points: %u\n", DataLogger::getInstance().getDataPointCount());
  Serial.println("==========================================\n");
}
