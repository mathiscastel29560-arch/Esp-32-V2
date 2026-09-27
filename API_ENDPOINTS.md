# ESP32-V2 REST API Endpoints

**Complete API Reference for Advanced Features**

## Base URL
```
http://<device-ip>:80
```

## Core Endpoints

### System Status
```
GET /api/status
Response: {uptime, freeHeap, freePSRAM, brightness, volume, contrast, batteryMode, cpuFreq}
```

### Audit History
```
GET /api/history
Response: {count, records[]}
```

### Audit Statistics
```
GET /api/stats
Response: {totalAudits, successRate, totalDevices, avgDevicesPerAudit, efficiency}
```

### Audit Records
```
GET /api/audits
Response: {audits[{timestamp, type, devices, rssi, status}]}
```

### Device Control
```
POST /api/control
Body: {action: "brightness|volume", value: <0-100>}
Response: {status: "ok"}
```

---

## Advanced API Endpoints

### 🚨 Alerts System
```
GET /api/alerts
Response: {
  stats: {total, unacknowledged, critical, warnings},
  alerts: [{type, level, message, timestamp, acknowledged}]
}
```

**Alert Types:**
- `0` - Battery Low
- `1` - Battery Critical
- `2` - Memory Low
- `3` - CPU High
- `4` - WiFi Disconnect
- `5` - Audit Failed
- `6` - Anomaly Detected
- `7` - Overheat
- `8` - Device Error

**Alert Levels:**
- `0` - Info
- `1` - Warning
- `2` - Critical

### ⏰ Scheduled Audits
```
GET /api/schedules
Response: {
  total: <count>,
  enabled: <count>,
  nextRun: <timestamp>,
  audits: [{id, name, type, recurrence, enabled}]
}
```

**Recurrence Types:**
- `0` - Once
- `1` - Hourly
- `2` - Daily
- `3` - Weekly
- `4` - Monthly

### 🔐 Authentication & API Keys
```
GET /api/auth
Response: {keys: [{key, description, role}]}

POST /api/auth
Body: {action: "generate_key", description: "...", role: <0-2>}
Response: {key: "..."}
```

**User Roles:**
- `0` - Admin (full access)
- `1` - Operator (run audits, modify params)
- `2` - Viewer (read-only)

### 📤 Export
```
GET /api/export?format=csv
Response: CSV data (text/csv)

GET /api/export?format=json
Response: {audits: [{timestamp, type, devices, rssi, status}]}
```

**Supported Formats:**
- `csv` - Comma-separated values
- `json` - JSON array

### 🔒 Encryption
```
GET /api/encryption
Response: {status: "ready|initializing", cipher: "AES-256-GCM"}
```

### 🔄 OTA Updates
```
GET /api/ota
Response: {
  state: <0-6>,
  progress: <0-100>,
  deltaSize: <bytes>,
  bytesSaved: <bytes>
}
```

**OTA States:**
- `0` - Idle
- `1` - Checking
- `2` - Downloading
- `3` - Patching
- `4` - Verifying
- `5` - Success
- `6` - Error

### ☁️ Cloud Sync
```
GET /api/cloud
Response: {
  connected: <true|false>,
  status: <0-5>,
  lastSync: <timestamp>,
  usage: <bytes>
}

POST /api/cloud
Body: {action: "sync|upload|download"}
Response: {status: "ok|error", message: "..."}
```

**Sync Status:**
- `0` - Idle
- `1` - Connecting
- `2` - Uploading
- `3` - Downloading
- `4` - Success
- `5` - Error

### 💾 Database
```
GET /api/database
Response: {
  initialized: <true|false>,
  auditCount: <count>,
  size: <bytes>
}

POST /api/database
Body: {action: "backup|restore|optimize|cleanup"}
Response: {status: "ok"}
```

---

## Authentication

### API Key
Include in request header:
```
X-API-Key: <your-api-key>
```

### Token
Include in request header:
```
Authorization: Bearer <token>
```

---

## Error Responses

### 400 Bad Request
```json
{"error": "Invalid format"}
```

### 401 Unauthorized
```json
{"error": "API key required"}
```

### 404 Not Found
```json
{"error": "Not found"}
```

### 500 Internal Server Error
```json
{"error": "Server error"}
```

---

## Rate Limiting

- **Status updates**: 1 request per 5 seconds
- **Audit operations**: 10 requests per minute
- **Export operations**: 5 requests per minute

---

## Example Usage

### Get Alert Statistics
```bash
curl http://192.168.1.100:80/api/alerts
```

### Generate New API Key
```bash
curl -X POST http://192.168.1.100:80/api/auth \
  -H "Content-Type: application/json" \
  -d '{"action":"generate_key","description":"Mobile App","role":1}'
```

### Export Audits as CSV
```bash
curl "http://192.168.1.100:80/api/export?format=csv" > audits.csv
```

### Get OTA Status
```bash
curl http://192.168.1.100:80/api/ota
```

### Trigger Cloud Sync
```bash
curl -X POST http://192.168.1.100:80/api/cloud \
  -H "Content-Type: application/json" \
  -d '{"action":"sync"}'
```

---

## WebSocket Events (Port 81)

### Status Update
```json
{
  "type": "status",
  "uptime": 3600,
  "freeHeap": 102400,
  "brightness": 80,
  "volume": 70
}
```

### Statistics Update
```json
{
  "type": "stats",
  "totalAudits": 42,
  "successRate": 98,
  "totalDevices": 215
}
```

### Audit Completion
```json
{
  "type": "audit",
  "timestamp": 1704067200,
  "type": "wifi_scan",
  "devices": 15,
  "rssi": -65
}
```

---

## Performance Notes

- Average response time: < 100ms
- Database queries optimized with indexes
- Compression enabled for large responses
- Connection pooling for cloud sync

---

**Last Updated:** 2026-09-27  
**API Version:** 2.1.0
