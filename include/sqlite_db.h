#pragma once

#include <string>
#include <vector>
#include <sqlite3.h>

// SQLite database wrapper for audit storage and configuration
class SQLiteDB {
public:
  static SQLiteDB& getInstance() {
    static SQLiteDB instance;
    return instance;
  }

  // Initialize database
  bool begin(const char* dbPath = "/spiffs/audit.db");

  // Close database
  void close();

  // Execute query
  bool execute(const std::string& sql);

  // Query with results
  std::vector<std::vector<std::string>> query(const std::string& sql);

  // Insert audit record
  bool insertAudit(uint32_t timestamp, const std::string& type,
                   uint8_t devicesFound, int8_t maxRSSI,
                   const std::string& status);

  // Query audits
  std::vector<std::vector<std::string>> getAudits(uint32_t limit = 100);

  // Get audit count
  uint32_t getAuditCount();

  // Delete old records (retention policy)
  bool deleteOldRecords(uint32_t ageSeconds);

  // Backup database
  bool backup(const char* backupPath);

  // Restore database
  bool restore(const char* backupPath);

  // Get database size
  uint32_t getSize() const { return dbSize; }

  // Optimize database
  bool optimize();

  // Check if initialized
  bool isInitialized() const { return initialized; }

private:
  SQLiteDB() = default;

  sqlite3* db = nullptr;
  bool initialized = false;
  uint32_t dbSize = 0;

  // Helper functions
  bool createTables();
  void updateSize();
  static int resultCallback(void* data, int argc, char** argv, char** colName);
};

#endif // SQLITE_DB_H
