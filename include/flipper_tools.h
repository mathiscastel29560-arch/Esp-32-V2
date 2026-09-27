#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <memory>

// Flipper Zero-inspired tools for ESP32-V2 - Core Security Audit Platform
class FlipperTools {
public:
  // Tool categories with complete coverage
  enum ToolCategory {
    TOOL_RF_TOOLS,        // Sub-Ghz (433MHz), NFC, Infrared
    TOOL_BLUETOOTH,       // BLE Scanner & Advanced Testing
    TOOL_GPIO,            // GPIO & UART Scanner
    TOOL_BADUSB,          // USB HID Emulation & Injection
    TOOL_MALWARE,         // Malware Database & Scanner
    TOOL_IBUTTON,         // iButton Emulation & Cloning
    TOOL_GAMES,           // Games & System Utilities
    TOOL_ARCHIVE,         // File Manager & Storage
    TOOL_MAX
  };

  // Result codes for operations
  enum ResultCode {
    RESULT_SUCCESS = 0,
    RESULT_ERROR_INVALID_PARAM = -1,
    RESULT_ERROR_HARDWARE = -2,
    RESULT_ERROR_TIMEOUT = -3,
    RESULT_ERROR_NOT_FOUND = -4,
    RESULT_ERROR_PERMISSION = -5,
    RESULT_ERROR_MEMORY = -6,
    RESULT_ERROR_UNKNOWN = -99
  };

  // Bluetooth device with extended fields
  struct BluetoothDevice {
    std::string address;          // BLE MAC address (e.g., "AA:BB:CC:DD:EE:FF")
    std::string name;             // Device name from advertisement
    int rssi;                      // Signal strength (dBm)
    uint8_t txPower;              // Transmit power
    bool connectable;             // Can be connected to
    uint64_t discoveredTime;      // First discovery timestamp
    std::string version;          // BLE version

    bool operator==(const BluetoothDevice& other) const {
      return address == other.address;
    }
  };

  // GPIO pin information
  struct GpioPin {
    uint8_t pin;                  // GPIO pin number
    bool isInput;                 // Can read
    bool isOutput;                // Can write
    uint8_t level;                // Current level (0 or 1)
    std::string description;      // Pin purpose/label
    uint32_t frequency;           // PWM frequency if applicable

    bool isValid() const { return pin < 50; }
  };

  // Malware signature with severity levels
  struct MalwareSignature {
    std::string hash;             // SHA256 or MD5 hash
    std::string name;             // Malware name
    std::string type;             // Category (virus, trojan, worm, etc.)
    std::string severity;         // CRITICAL, HIGH, MEDIUM, LOW
    std::string description;      // Detection details
    uint64_t lastUpdate;          // Last signature update timestamp

    int getSeverityLevel() const {
      if (severity == "CRITICAL") return 4;
      if (severity == "HIGH") return 3;
      if (severity == "MEDIUM") return 2;
      if (severity == "LOW") return 1;
      return 0;
    }
  };

  // iButton key for emulation and cloning
  struct IButtonKey {
    uint8_t family;               // iButton family code
    uint8_t serial[8];            // 64-bit serial number
    uint8_t crc;                  // CRC checksum
    std::string description;      // Key description/label
    uint64_t createdTime;         // Key creation timestamp
    bool verified;                // Has been verified

    bool isValid() const;
    std::string toHexString() const;
  };

  // Singleton
  static FlipperTools& getInstance() {
    static FlipperTools instance;
    return instance;
  }

  // Boot screen with tiger
  void displayBootScreen();

  // === BLUETOOTH/BLE TOOLS ===
  ResultCode startBLEScanning(uint32_t durationMs);
  ResultCode stopBLEScanning();
  std::vector<BluetoothDevice> getBTDevices() const;
  ResultCode connectBTDevice(const std::string& address);
  ResultCode disconnectBTDevice(const std::string& address);
  ResultCode emulateBTDevice(const std::string& name);
  uint32_t getBTDeviceCount() const;
  bool isBLEScanning() const;

  // === GPIO & UART TOOLS ===
  ResultCode scanGPIO(std::vector<GpioPin>& pins);
  ResultCode readPin(uint8_t pin, uint8_t& level);
  ResultCode writePin(uint8_t pin, uint8_t level);
  ResultCode setPWMFrequency(uint8_t pin, uint32_t frequency);
  std::vector<uint8_t> getUARTDevices();
  ResultCode scanUART();

  // === BadUSB/HID EMULATION ===
  ResultCode sendKeyboardSequence(const std::string& sequence);
  ResultCode sendMouseMove(int8_t x, int8_t y);
  ResultCode sendMouseClick(uint8_t button);
  ResultCode sendMouseDrag(int8_t x, int8_t y, uint8_t button);
  bool isUSBReady() const;

  // === MALWARE SCANNER ===
  ResultCode loadMalwareDatabase();
  ResultCode scanForMalware(const std::string& hash, std::vector<MalwareSignature>& results);
  ResultCode calculateFileHash(const std::string& filepath, std::string& hashOutput);
  uint32_t getMalwareSignatureCount() const;
  bool isMalwareDbLoaded() const;

  // === iButton EMULATION ===
  ResultCode registerIButton(const IButtonKey& key);
  ResultCode getStoredButtons(std::vector<IButtonKey>& buttons);
  ResultCode emulateIButton(const IButtonKey& key);
  ResultCode deleteIButton(const std::string& id);
  uint32_t getIButtonCount() const;

  // === GAMES & UTILITIES ===
  void playSnakeGame();
  void playFlappyBirdGame();
  void displayAlarmClock();
  void displayMetronome(uint16_t bpm);
  void displayMemoryStats();

  // === ARCHIVE/FILE MANAGER ===
  ResultCode listFiles(const std::string& path, std::vector<std::string>& files);
  ResultCode deleteFile(const std::string& path);
  ResultCode renameFile(const std::string& oldPath, const std::string& newPath);
  ResultCode getFileSize(const std::string& path, uint32_t& size);
  ResultCode copyFile(const std::string& source, const std::string& dest);

  // === SYSTEM STATUS ===
  bool isHealthy() const;
  std::string getStatus() const;

private:
  FlipperTools() = default;

  // Private helper methods for display
  void drawTiger();
  void drawBootSequence();

  friend class FlipperMenu;
  friend class FlipperAdvancedMenu;
  friend class FlipperUltimateMenu;
};
