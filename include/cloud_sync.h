#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <functional>

// Cloud synchronization and backup system
class CloudSync {
public:
  enum SyncStatus {
    SYNC_IDLE = 0,
    SYNC_CONNECTING = 1,
    SYNC_UPLOADING = 2,
    SYNC_DOWNLOADING = 3,
    SYNC_SUCCESS = 4,
    SYNC_ERROR = 5
  };

  using SyncCallback = std::function<void(SyncStatus, const std::string&)>;

  static CloudSync& getInstance() {
    static CloudSync instance;
    return instance;
  }

  // Initialize cloud sync
  bool begin(const std::string& serverURL);

  // Set authentication credentials
  void setCredentials(const std::string& username, const std::string& password);

  // Upload audit data to cloud
  bool uploadAudits();

  // Download configuration from cloud
  bool downloadConfig();

  // Sync entire database
  bool syncDatabase();

  // Check cloud connectivity
  bool isConnected() const { return connected; }

  // Get current sync status
  SyncStatus getStatus() const { return currentStatus; }

  // Set sync callback
  void setSyncCallback(SyncCallback cb) { syncCallback = cb; }

  // Enable/disable auto-sync
  void setAutoSync(bool enabled);

  // Force sync immediately
  bool syncNow();

  // Get last sync time
  uint32_t getLastSyncTime() const { return lastSyncTime; }

  // Get cloud storage usage
  uint64_t getCloudUsage() const { return cloudUsage; }

private:
  CloudSync() = default;

  std::string serverURL;
  std::string username;
  std::string password;
  bool connected = false;
  bool autoSyncEnabled = false;
  SyncStatus currentStatus = SYNC_IDLE;
  uint32_t lastSyncTime = 0;
  uint64_t cloudUsage = 0;
  SyncCallback syncCallback = nullptr;

  // Helper functions
  bool uploadFile(const std::string& localPath, const std::string& remotePath);
  bool downloadFile(const std::string& remotePath, const std::string& localPath);
  bool authenticate();
  void updateStatus(SyncStatus status, const std::string& message);
};

#endif // CLOUD_SYNC_H
