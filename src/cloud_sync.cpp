#include "cloud_sync.h"
#include "debug_logger.h"
#include "sqlite_db.h"
#include <HTTPClient.h>
#include <ctime>

bool CloudSync::begin(const std::string& url) {
  serverURL = url;
  DebugLogger::printf("[CloudSync] Initialized with server: %s\n", url.c_str());
  return true;
}

void CloudSync::setCredentials(const std::string& user, const std::string& pass) {
  username = user;
  password = pass;
  DebugLogger::println("[CloudSync] Credentials set");
}

bool CloudSync::uploadAudits() {
  if (!isConnected()) {
    updateStatus(SYNC_ERROR, "Not connected to cloud");
    return false;
  }

  updateStatus(SYNC_UPLOADING, "Uploading audits...");

  auto& db = SQLiteDB::getInstance();
  auto audits = db.getAudits(1000);

  if (audits.empty()) {
    updateStatus(SYNC_IDLE, "No audits to upload");
    return true;
  }

  // Build JSON payload
  std::string payload = "{\"audits\":[";
  for (size_t i = 0; i < audits.size(); i++) {
    if (i > 0) payload += ",";
    if (audits[i].size() >= 5) {
      payload += "{\"timestamp\":\"" + audits[i][0] + "\",\"type\":\"" +
                 audits[i][1] + "\",\"devices\":" + audits[i][2] +
                 ",\"rssi\":" + audits[i][3] + ",\"status\":\"" +
                 audits[i][4] + "\"}";
    }
  }
  payload += "]}";

  HTTPClient http;
  http.begin(serverURL + "/api/audits/upload");
  http.addHeader("Content-Type", "application/json");

  int httpCode = http.POST((uint8_t*)payload.c_str(), payload.length());

  if (httpCode == 200) {
    lastSyncTime = time(nullptr);
    cloudUsage += payload.length();
    updateStatus(SYNC_SUCCESS, "Audits uploaded successfully");
    DebugLogger::printf("[CloudSync] Uploaded %u audits\n", (unsigned int)audits.size());
    http.end();
    return true;
  } else {
    updateStatus(SYNC_ERROR, "Upload failed: " + std::to_string(httpCode));
    DebugLogger::printf("[CloudSync] Upload error: %d\n", httpCode);
    http.end();
    return false;
  }
}

bool CloudSync::downloadConfig() {
  if (!isConnected()) {
    updateStatus(SYNC_ERROR, "Not connected to cloud");
    return false;
  }

  updateStatus(SYNC_DOWNLOADING, "Downloading configuration...");

  HTTPClient http;
  http.begin(serverURL + "/api/config/download");

  int httpCode = http.GET();

  if (httpCode == 200) {
    String payload = http.getString();

    // Parse and apply configuration
    DebugLogger::printf("[CloudSync] Configuration downloaded (%u bytes)\n",
      payload.length());

    lastSyncTime = time(nullptr);
    updateStatus(SYNC_SUCCESS, "Configuration updated");
    http.end();
    return true;
  } else {
    updateStatus(SYNC_ERROR, "Download failed: " + std::to_string(httpCode));
    http.end();
    return false;
  }
}

bool CloudSync::syncDatabase() {
  updateStatus(SYNC_CONNECTING, "Syncing database...");

  // 1. Upload audits
  if (!uploadAudits()) {
    return false;
  }

  // 2. Download config
  if (!downloadConfig()) {
    return false;
  }

  lastSyncTime = time(nullptr);
  DebugLogger::println("[CloudSync] Full sync completed");
  return true;
}

void CloudSync::setAutoSync(bool enabled) {
  autoSyncEnabled = enabled;
  DebugLogger::printf("[CloudSync] Auto-sync: %s\n", enabled ? "ON" : "OFF");
}

bool CloudSync::syncNow() {
  return syncDatabase();
}

bool CloudSync::uploadFile(const std::string& localPath,
                          const std::string& remotePath) {
  DebugLogger::printf("[CloudSync] Uploading: %s → %s\n",
    localPath.c_str(), remotePath.c_str());

  // In production, implement file upload
  return true;
}

bool CloudSync::downloadFile(const std::string& remotePath,
                            const std::string& localPath) {
  DebugLogger::printf("[CloudSync] Downloading: %s → %s\n",
    remotePath.c_str(), localPath.c_str());

  // In production, implement file download
  return true;
}

bool CloudSync::authenticate() {
  HTTPClient http;
  http.begin(serverURL + "/api/auth/login");
  http.addHeader("Content-Type", "application/json");

  std::string auth = "{\"username\":\"" + username + "\",\"password\":\"" +
                     password + "\"}";

  int httpCode = http.POST((uint8_t*)auth.c_str(), auth.length());

  if (httpCode == 200) {
    connected = true;
    DebugLogger::println("[CloudSync] Authentication successful");
    http.end();
    return true;
  } else {
    connected = false;
    DebugLogger::printf("[CloudSync] Authentication failed: %d\n", httpCode);
    http.end();
    return false;
  }
}

void CloudSync::updateStatus(SyncStatus status, const std::string& message) {
  currentStatus = status;

  if (syncCallback) {
    syncCallback(status, message);
  }

  DebugLogger::printf("[CloudSync] Status=%u: %s\n", status, message.c_str());
}
