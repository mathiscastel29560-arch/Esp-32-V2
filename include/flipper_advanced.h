#pragma once

#include <string>
#include <vector>
#include <cstdint>

// Advanced Flipper Zero Tools - CAN Bus, Jamming, JTAG/SWD debugging
class FlipperAdvanced {
public:
  // Error codes for operations
  enum ResultCode {
    RESULT_SUCCESS = 0,
    RESULT_ERROR_INVALID_PARAM = -1,
    RESULT_ERROR_HARDWARE = -2,
    RESULT_ERROR_TIMEOUT = -3,
    RESULT_ERROR_NOT_FOUND = -4,
    RESULT_ERROR_NOT_CONNECTED = -5,
    RESULT_ERROR_MEMORY = -6,
    RESULT_ERROR_UNKNOWN = -99
  };

  enum AdvancedTool {
    TOOL_CAN_BUS,         // CAN bus automotive tools
    TOOL_JAMMING,         // WiFi/BLE jamming and RF interference
    TOOL_JTAG,            // JTAG/SWD hardware debugging interface
  };

  // === CAN BUS STRUCTURES ===
  struct CANMessage {
    uint32_t id;                 // CAN message ID (11-bit or 29-bit)
    uint8_t dlc;                 // Data Length Code (0-8)
    uint8_t data[8];             // Message payload
    uint32_t timestamp;          // Capture timestamp
    bool isExtended;             // Extended ID format (29-bit)
    bool isRemote;               // Remote transmission request
    uint8_t errorFlags;          // Error flags if any

    bool isValid() const {
      return dlc <= 8 && (id <= (isExtended ? 0x1FFFFFFF : 0x7FF));
    }
  };

  struct CANBusStats {
    uint32_t messagesReceived;   // Total messages captured
    uint32_t messagesSent;       // Total messages sent
    uint32_t errorsDetected;     // CAN bus errors
    uint32_t bitsPerSecond;      // Baudrate
    float cpuLoad;               // CPU usage percentage
    uint64_t uptime;             // Uptime in milliseconds
  };

  // === JAMMING STRUCTURES ===
  struct JammingSignal {
    std::string type;             // WiFi, BLE, RF, Cellular
    uint32_t frequency;           // Frequency in Hz
    uint8_t power;                // Power 0-100%
    bool isActive;                // Currently jamming
    uint32_t durationMs;          // Duration in milliseconds
    std::string modulation;       // Modulation type

    bool isValid() const {
      return !type.empty() && frequency > 0 && power <= 100;
    }
  };

  struct JammedDevice {
    std::string address;          // Device MAC/ID
    std::string type;             // Device type (WiFi, BLE, etc.)
    int rssiLoss;                 // Signal loss in dB
    uint32_t jammingTime;         // Time jammed
    bool isRecoverable;           // Can device recover
  };

  // === JTAG/SWD STRUCTURES ===
  struct DebugDevice {
    std::string name;             // Device/chip name
    std::string manufacturer;     // Manufacturer name
    uint32_t deviceId;            // JTAG device ID
    std::string architecture;     // ARM, MIPS, RISC-V, etc.
    bool isConnected;             // Currently connected
    uint8_t idcodeVersion;        // IDCODE version

    bool isValid() const {
      return !name.empty() && deviceId != 0;
    }
  };

  struct MemoryRegion {
    uint32_t startAddress;        // Start address
    uint32_t size;                // Region size in bytes
    std::string permissions;      // R, W, X permissions
    std::string type;             // Flash, RAM, EEPROM, etc.
    bool isAccessible;            // Can be accessed
    bool isProtected;             // Write-protected

    bool isValid() const {
      return size > 0 && startAddress < 0xFFFFFFFF;
    }
  };

  // Singleton
  static FlipperAdvanced& getInstance() {
    static FlipperAdvanced instance;
    return instance;
  }

  // === CAN BUS OPERATIONS ===
  ResultCode initCANBus(uint32_t baudrate = 500000);
  ResultCode scanCANNetwork();
  ResultCode captureCANMessages(uint32_t durationMs, std::vector<CANMessage>& messages);
  ResultCode sendCANMessage(const CANMessage& msg);
  ResultCode floodCANBus(uint32_t messageId, uint8_t dataLength, uint32_t countMessages);
  ResultCode getCANStats(CANBusStats& stats) const;
  ResultCode analyzeCANTraffic();
  ResultCode fuzzyCANMessages(uint32_t durationMs);
  bool isCANBusInitialized() const;

  // === JAMMING OPERATIONS ===
  ResultCode startWiFiJamming(uint32_t power = 50);
  ResultCode startBLEJamming(uint32_t power = 50);
  ResultCode startRFJamming(uint32_t frequency, uint32_t power = 50);
  ResultCode stopJamming();
  bool isJammingActive() const;
  ResultCode getJammedDevices(std::vector<JammedDevice>& devices) const;
  float getJamEffectiveness() const;
  ResultCode generateNoisePattern(const std::string& pattern);

  // === JTAG/SWD OPERATIONS ===
  ResultCode initJTAG(uint8_t tckPin, uint8_t tmsPin, uint8_t tdoPin, uint8_t tdiPin);
  ResultCode initSWD(uint8_t clockPin, uint8_t dataPin);
  ResultCode scanJTAGDevices();
  ResultCode getConnectedDevices(std::vector<DebugDevice>& devices) const;
  ResultCode connectToDevice(uint32_t deviceId);
  ResultCode readMemoryMap(std::vector<MemoryRegion>& regions);
  ResultCode readMemory(uint32_t address, uint32_t size, std::vector<uint8_t>& data);
  ResultCode writeMemory(uint32_t address, const std::vector<uint8_t>& data);
  ResultCode eraseFlash(uint32_t startAddress, uint32_t size);
  ResultCode dumpFirmware(uint32_t startAddress, uint32_t size, const std::string& filepath);
  ResultCode identifyChip(std::string& chipName);
  ResultCode setBreakpoint(uint32_t address);
  ResultCode stepDebugger();
  ResultCode runDebugger();
  ResultCode stopDebugger();

  // === STATUS & MONITORING ===
  bool isHealthy() const;
  std::string getStatus() const;
  ResultCode getLastError() const;
  std::string getLastErrorMessage() const;

private:
  FlipperAdvanced() = default;

  // CAN Bus state
  bool canBusInitialized = false;
  uint32_t canBaudrate = 500000;
  std::vector<CANMessage> canMessageBuffer;
  CANBusStats canStats = {0, 0, 0, 0, 0.0f};

  // Jamming state
  bool jammingActive = false;
  JammingSignal currentJam = {"", 0, 0, false, 0};
  std::vector<JammedDevice> jammedDevices;
  float jamEffectiveness = 0.0f;

  // JTAG/SWD state
  bool jtagInitialized = false;
  bool swdInitialized = false;
  uint8_t tckPin, tmsPin, tdoPin, tdiPin;
  uint8_t swdClockPin, swdDataPin;
  std::vector<DebugDevice> debugDevices;
  uint32_t lastError = 0;
  std::string lastErrorMsg = "";

  // Helper methods
  void parseFuzzyMessage(CANMessage& msg);
  void calculateJamEffectiveness();
  uint32_t calculateJTAGID(uint32_t raw);
};
