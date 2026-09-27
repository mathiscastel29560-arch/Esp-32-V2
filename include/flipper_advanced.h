#pragma once

#include <string>
#include <vector>
#include <cstdint>

// Advanced Flipper Zero Tools - CAN Bus, Jamming, JTAG
class FlipperAdvanced {
public:
  enum AdvancedTool {
    TOOL_CAN_BUS,         // CAN bus automotive tools
    TOOL_JAMMING,         // WiFi/BLE jamming
    TOOL_JTAG,            // JTAG/SWD debugging
  };

  // ============ CAN BUS TOOLS ============
  struct CANMessage {
    uint32_t id;
    uint8_t dlc;  // Data Length Code
    uint8_t data[8];
    uint32_t timestamp;
    bool isExtended;
    bool isRemote;
  };

  struct CANBusStats {
    uint32_t messagesReceived;
    uint32_t messagesSent;
    uint32_t errorsDetected;
    uint32_t bitsPerSecond;
    float cpuLoad;
  };

  // ============ JAMMING TOOLS ============
  struct JammingSignal {
    std::string type;        // WiFi, BLE, RF
    uint32_t frequency;
    uint8_t power;          // 0-100%
    bool isActive;
    uint32_t durationMs;
  };

  struct JammedDevice {
    std::string address;
    std::string type;
    int rssiLoss;           // Signal loss in dB
    uint32_t jammingTime;
  };

  // ============ JTAG/SWD TOOLS ============
  struct DebugDevice {
    std::string name;
    std::string manufacturer;
    uint32_t deviceId;
    std::string architecture;  // ARM, MIPS, etc.
    bool isConnected;
  };

  struct MemoryRegion {
    uint32_t startAddress;
    uint32_t size;
    std::string permissions;   // R, W, X
    std::string type;          // Flash, RAM, etc.
    bool isAccessible;
  };

  // Singleton
  static FlipperAdvanced& getInstance() {
    static FlipperAdvanced instance;
    return instance;
  }

  // ========== CAN BUS ==========
  bool initCANBus(uint32_t baudrate = 500000);
  void scanCANNetwork();
  std::vector<CANMessage> captureCANMessages(uint32_t durationMs);
  bool sendCANMessage(const CANMessage& msg);
  bool floodCANBus(uint32_t messageId, uint8_t dataLength, uint32_t countMessages);
  CANBusStats getCANStats() const;
  void analyzeCANTraffic();
  bool fuzzyCANMessages(uint32_t durationMs);  // Send random CAN messages

  // ========== JAMMING TOOLS ==========
  bool startWiFiJamming(uint32_t power = 50);
  bool startBLEJamming(uint32_t power = 50);
  bool startRFJamming(uint32_t frequency, uint32_t power = 50);
  bool stopJamming();
  bool isJammingActive() const;
  std::vector<JammedDevice> getJammedDevices() const;
  float getJamEffectiveness() const;
  void generateNoisePattern(const std::string& pattern);

  // ========== JTAG/SWD TOOLS ==========
  bool initJTAG(uint8_t tckPin, uint8_t tmsPin, uint8_t tdoPin, uint8_t tdiPin);
  bool initSWD(uint8_t clockPin, uint8_t dataPin);
  bool scanJTAGDevices();
  std::vector<DebugDevice> getConnectedDevices() const;
  bool connectToDevice(uint32_t deviceId);
  std::vector<MemoryRegion> readMemoryMap();
  std::vector<uint8_t> readMemory(uint32_t address, uint32_t size);
  bool writeMemory(uint32_t address, const std::vector<uint8_t>& data);
  bool eraseFlash(uint32_t startAddress, uint32_t size);
  bool dumpFirmware(uint32_t startAddress, uint32_t size, const std::string& filepath);
  std::string identifyChip();
  bool setBreakpoint(uint32_t address);
  bool stepDebugger();
  bool runDebugger();
  bool stopDebugger();

  // Status & Monitoring
  bool isHealthy() const;
  std::string getStatus() const;
  uint32_t getLastErrorCode() const;
  std::string getLastError() const;

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
