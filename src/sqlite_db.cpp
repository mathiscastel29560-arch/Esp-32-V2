#include "sqlite_db.h"
#include "debug_logger.h"
#include <cstring>
#include <ctime>

bool SQLiteDB::begin(const char* dbPath) {
  if (initialized) return true;

  int rc = sqlite3_open(dbPath, &db);
  if (rc) {
    DebugLogger::printf("[SQLite] Failed to open database: %s\n", sqlite3_errmsg(db));
    return false;
  }

  if (!createTables()) {
    DebugLogger::println("[SQLite] Failed to create tables");
    sqlite3_close(db);
    return false;
  }

  initialized = true;
  updateSize();
  DebugLogger::printf("[SQLite] Database initialized: %s (%.1f KB)\n",
    dbPath, dbSize / 1024.0f);

  return true;
}

void SQLiteDB::close() {
  if (db) {
    sqlite3_close(db);
    db = nullptr;
    initialized = false;
  }
}

bool SQLiteDB::execute(const std::string& sql) {
  if (!initialized) return false;

  char* errMsg = nullptr;
  int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);

  if (rc != SQLITE_OK) {
    DebugLogger::printf("[SQLite] Execute error: %s\n", errMsg);
    sqlite3_free(errMsg);
    return false;
  }

  return true;
}

std::vector<std::vector<std::string>> SQLiteDB::query(const std::string& sql) {
  std::vector<std::vector<std::string>> results;
  if (!initialized) return results;

  sqlite3_stmt* stmt;
  int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);

  if (rc != SQLITE_OK) {
    DebugLogger::printf("[SQLite] Query prepare error: %s\n", sqlite3_errmsg(db));
    return results;
  }

  while (sqlite3_step(stmt) == SQLITE_ROW) {
    std::vector<std::string> row;
    int cols = sqlite3_column_count(stmt);

    for (int i = 0; i < cols; i++) {
      const char* val = (const char*)sqlite3_column_text(stmt, i);
      row.push_back(val ? val : "");
    }

    results.push_back(row);
  }

  sqlite3_finalize(stmt);
  return results;
}

bool SQLiteDB::insertAudit(uint32_t timestamp, const std::string& type,
                           uint8_t devicesFound, int8_t maxRSSI,
                           const std::string& status) {
  char sql[256];
  snprintf(sql, sizeof(sql),
    "INSERT INTO audits (timestamp, type, devices_found, max_rssi, status) "
    "VALUES (%u, '%s', %u, %d, '%s');",
    timestamp, type.c_str(), devicesFound, maxRSSI, status.c_str());

  return execute(sql);
}

std::vector<std::vector<std::string>> SQLiteDB::getAudits(uint32_t limit) {
  char sql[128];
  snprintf(sql, sizeof(sql),
    "SELECT timestamp, type, devices_found, max_rssi, status "
    "FROM audits ORDER BY timestamp DESC LIMIT %u;", limit);

  return query(sql);
}

uint32_t SQLiteDB::getAuditCount() {
  auto results = query("SELECT COUNT(*) FROM audits;");
  if (results.empty() || results[0].empty()) return 0;
  return std::stoul(results[0][0]);
}

bool SQLiteDB::deleteOldRecords(uint32_t ageSeconds) {
  uint32_t cutoffTime = time(nullptr) - ageSeconds;

  char sql[256];
  snprintf(sql, sizeof(sql),
    "DELETE FROM audits WHERE timestamp < %u;", cutoffTime);

  bool success = execute(sql);
  if (success) {
    DebugLogger::printf("[SQLite] Deleted records older than %u seconds\n", ageSeconds);
    updateSize();
  }

  return success;
}

bool SQLiteDB::backup(const char* backupPath) {
  if (!initialized) return false;

  sqlite3_backup* pBackup;
  sqlite3* pFile;

  int rc = sqlite3_open(backupPath, &pFile);
  if (rc != SQLITE_OK) {
    DebugLogger::printf("[SQLite] Backup open error: %s\n", sqlite3_errmsg(pFile));
    sqlite3_close(pFile);
    return false;
  }

  pBackup = sqlite3_backup_init(pFile, "main", db, "main");
  if (!pBackup) {
    DebugLogger::printf("[SQLite] Backup init error: %s\n", sqlite3_errmsg(pFile));
    sqlite3_close(pFile);
    return false;
  }

  sqlite3_backup_step(pBackup, -1);
  sqlite3_backup_finish(pBackup);

  int finalRc = sqlite3_errcode(pFile);
  sqlite3_close(pFile);

  if (finalRc == SQLITE_OK) {
    DebugLogger::printf("[SQLite] Backup successful: %s\n", backupPath);
    return true;
  }

  return false;
}

bool SQLiteDB::restore(const char* backupPath) {
  if (!initialized) return false;

  sqlite3* pBackupFile;
  int rc = sqlite3_open(backupPath, &pBackupFile);
  if (rc != SQLITE_OK) {
    DebugLogger::printf("[SQLite] Restore open error: %s\n", sqlite3_errmsg(pBackupFile));
    sqlite3_close(pBackupFile);
    return false;
  }

  sqlite3_backup* pBackup = sqlite3_backup_init(db, "main", pBackupFile, "main");
  if (!pBackup) {
    DebugLogger::printf("[SQLite] Restore init error: %s\n", sqlite3_errmsg(db));
    sqlite3_close(pBackupFile);
    return false;
  }

  sqlite3_backup_step(pBackup, -1);
  sqlite3_backup_finish(pBackup);

  int finalRc = sqlite3_errcode(db);
  sqlite3_close(pBackupFile);

  if (finalRc == SQLITE_OK) {
    DebugLogger::printf("[SQLite] Restore successful from: %s\n", backupPath);
    updateSize();
    return true;
  }

  return false;
}

bool SQLiteDB::optimize() {
  if (!initialized) return false;

  bool success = execute("VACUUM;");
  if (success) {
    updateSize();
    DebugLogger::println("[SQLite] Database optimized");
  }

  return success;
}

bool SQLiteDB::createTables() {
  const char* sql =
    "CREATE TABLE IF NOT EXISTS audits ("
    "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
    "  timestamp INTEGER NOT NULL,"
    "  type TEXT NOT NULL,"
    "  devices_found INTEGER,"
    "  max_rssi INTEGER,"
    "  status TEXT,"
    "  created_at DATETIME DEFAULT CURRENT_TIMESTAMP"
    ");"
    "CREATE INDEX IF NOT EXISTS idx_timestamp ON audits(timestamp);"
    "CREATE INDEX IF NOT EXISTS idx_type ON audits(type);";

  char* errMsg = nullptr;
  int rc = sqlite3_exec(db, sql, nullptr, nullptr, &errMsg);

  if (rc != SQLITE_OK) {
    DebugLogger::printf("[SQLite] Table creation error: %s\n", errMsg);
    sqlite3_free(errMsg);
    return false;
  }

  return true;
}

void SQLiteDB::updateSize() {
  // Estimate size: get file size in bytes
  // In production, use actual file stat
  dbSize = sqlite3_memory_used();
}

int SQLiteDB::resultCallback(void* data, int argc, char** argv, char** colName) {
  return 0;
}
