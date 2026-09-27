#ifndef ADVANCED_LOGGING_H
#define ADVANCED_LOGGING_H

#include <Arduino.h>
#include <vector>

// ============= DATA LOGGER =============

struct DataPoint {
  uint32_t timestamp;
  const char* metric;
  float value;
  const char* unit;

  DataPoint() : timestamp(0), metric(""), value(0), unit("") {}
};

class DataLogger {
public:
  static DataLogger& getInstance() {
    static DataLogger instance;
    return instance;
  }

  void logMetric(const char* metric, float value, const char* unit = "");
  void logAttackMetric(const char* attackName, const char* metric, float value);
  void logBatteryMetric(float voltage, uint8_t percent, float current);
  void logRFMetric(const char* module, int8_t rssi, float snr);

  uint16_t getDataPointCount() const { return dataPoints.size(); }
  DataPoint* getDataPoint(uint16_t index);

  bool exportToCSV(char* output, uint16_t maxLen);
  bool exportToJSON(char* output, uint16_t maxLen);

  void clearOldData(uint32_t maxAgeSeconds = 3600);
  void clearAllData();

private:
  DataLogger() {}

  std::vector<DataPoint*> dataPoints;
  static const uint16_t MAX_DATA_POINTS = 2000;
};

// ============= FILE LOGGER (SD Card) =============

class FileLogger {
public:
  static FileLogger& getInstance() {
    static FileLogger instance;
    return instance;
  }

  bool begin(const char* sdCardPath = "/sd");
  void end();
  bool isReady() const { return ready; }

  bool writeLog(const char* filename, const char* data);
  bool appendLog(const char* filename, const char* data);

  bool exportAttackResults(const char* attackName, const char* filename);
  bool exportSystemLogs(const char* filename);
  bool exportDataLog(const char* filename);

  uint32_t getAvailableSpace();
  uint32_t getTotalSpace();

  void printFileInfo();

private:
  FileLogger() : ready(false) {}

  bool ready;
  char sdPath[64];
};

// ============= CLOUD LOGGER (WiFi Sync) =============

enum class CloudProvider {
  THINGSPEAK = 0,
  INFLUXDB = 1,
  FIREBASE = 2,
  CUSTOM = 3
};

struct CloudConfig {
  CloudProvider provider;
  char apiKey[128];
  char endpoint[256];
  uint16_t port;
  bool useSSL;

  CloudConfig() : provider(CloudProvider::THINGSPEAK), port(80), useSSL(false) {
    memset(apiKey, 0, 128);
    memset(endpoint, 0, 256);
  }
};

class CloudLogger {
public:
  static CloudLogger& getInstance() {
    static CloudLogger instance;
    return instance;
  }

  bool configure(const CloudConfig& config);
  bool connect();
  void disconnect();
  bool isConnected() const { return connected; }

  bool uploadAttackResults(const char* attackName, uint32_t duration);
  bool uploadBatteryStats(uint8_t percent, float voltage);
  bool uploadSystemStats();

  void setBufferSize(uint16_t maxEntries) { bufferMaxSize = maxEntries; }
  uint16_t getBufferSize() const { return buffer.size(); }

  bool syncBuffer();
  void clearBuffer();

private:
  CloudLogger() : connected(false), bufferMaxSize(100) {}

  CloudConfig config;
  bool connected;
  std::vector<const char*> buffer;
  uint16_t bufferMaxSize;
  uint32_t lastSyncTime;

  bool sendHTTP(const char* payload);
  bool sendHTTPS(const char* payload);
};

// ============= RING BUFFER (Circular) =============

struct RingBufferEntry {
  uint32_t timestamp;
  char message[256];

  RingBufferEntry() : timestamp(0) {
    memset(message, 0, 256);
  }
};

class RingBuffer {
public:
  RingBuffer(uint16_t size = 100) : maxSize(size), writePos(0), readPos(0) {
    buffer = new RingBufferEntry[size];
  }

  ~RingBuffer() {
    delete[] buffer;
  }

  void write(const char* message);
  const char* read();
  bool hasData() const { return (writePos + maxSize - readPos) % maxSize > 0; }

  void printAll();
  void exportToBuffer(char* output, uint16_t maxLen);

private:
  RingBufferEntry* buffer;
  uint16_t maxSize;
  uint16_t writePos;
  uint16_t readPos;
};

// ============= LOG AGGREGATOR =============

class LogAggregator {
public:
  static LogAggregator& getInstance() {
    static LogAggregator instance;
    return instance;
  }

  void begin();
  void update();

  void enableDataLogging(bool enable) { dataLoggingEnabled = enable; }
  void enableFileLogging(bool enable) { fileLoggingEnabled = enable; }
  void enableCloudLogging(bool enable) { cloudLoggingEnabled = enable; }
  void enableRingBuffer(bool enable) { ringBufferEnabled = enable; }

  DataLogger& getDataLogger() { return DataLogger::getInstance(); }
  FileLogger& getFileLogger() { return FileLogger::getInstance(); }
  CloudLogger& getCloudLogger() { return CloudLogger::getInstance(); }
  RingBuffer* getRingBuffer() { return ringBuffer; }

  void generateMasterReport(char* output, uint16_t maxLen);
  void printLogStatus();

private:
  LogAggregator();

  RingBuffer* ringBuffer;
  bool dataLoggingEnabled;
  bool fileLoggingEnabled;
  bool cloudLoggingEnabled;
  bool ringBufferEnabled;

  uint32_t lastSyncTime;
  static const uint32_t SYNC_INTERVAL = 60000; // 60 seconds
};

#endif // ADVANCED_LOGGING_H
