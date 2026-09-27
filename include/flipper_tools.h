#pragma once

#include <string>
#include <vector>
#include <cstdint>

// Flipper Zero-inspired tools for ESP32-V2
class FlipperTools {
public:
  enum ToolCategory {
    TOOL_RF_TOOLS,        // Sub-Ghz, NFC, Infrared (existing)
    TOOL_BLUETOOTH,       // BLE Scanner & Testing
    TOOL_GPIO,            // GPIO & UART Scanner
    TOOL_BADUSB,          // USB HID Emulation
    TOOL_MALWARE,         // Malware Database & Scanner
    TOOL_IBUTTON,         // iButton Emulation
    TOOL_GAMES,           // Games & Utilities
    TOOL_ARCHIVE,         // File Manager
    TOOL_MAX
  };

  struct BluetoothDevice {
    std::string address;
    std::string name;
    int rssi;
    uint8_t txPower;
    bool connectable;
    uint64_t discoveredTime;
  };

  struct GpioPin {
    uint8_t pin;
    bool isInput;
    bool isOutput;
    uint8_t level;
    std::string description;
  };

  struct MalwareSignature {
    std::string hash;
    std::string name;
    std::string type;
    std::string severity;
    std::string description;
  };

  struct IButtonKey {
    uint8_t family;
    uint8_t serial[8];
    uint8_t crc;
    std::string description;
  };

  // Singleton
  static FlipperTools& getInstance() {
    static FlipperTools instance;
    return instance;
  }

  // Boot screen with tiger
  void displayBootScreen();

  // Bluetooth/BLE Tools
  void startBLEScanning(uint32_t durationMs);
  std::vector<BluetoothDevice> getBTDevices() const;
  bool connectBTDevice(const std::string& address);
  bool emulateBTDevice(const std::string& name);

  // GPIO & UART Tools
  std::vector<GpioPin> scanGPIO();
  bool readPin(uint8_t pin);
  bool writePin(uint8_t pin, uint8_t level);
  std::vector<uint8_t> getUARTDevices();

  // BadUSB/HID Emulation
  bool sendKeyboardSequence(const std::string& sequence);
  bool sendMouseMove(int8_t x, int8_t y);
  bool sendMouseClick(uint8_t button);

  // Malware Scanner
  void loadMalwareDatabase();
  std::vector<MalwareSignature> scanForMalware(const std::string& hash);
  std::string calculateFileHash(const std::string& filepath);

  // iButton Emulation
  void registerIButton(const IButtonKey& key);
  std::vector<IButtonKey> getStoredButtons();
  bool emulateIButton(const IButtonKey& key);

  // Games & Utilities
  void playSnakeGame();
  void playFlappyBirdGame();
  void displayAlarmClock();
  void displayMetronome(uint16_t bpm);

  // Archive/File Manager
  std::vector<std::string> listFiles(const std::string& path);
  bool deleteFile(const std::string& path);
  bool renameFile(const std::string& oldPath, const std::string& newPath);
  uint32_t getFileSize(const std::string& path);

  // Status
  bool isHealthy() const;
  std::string getStatus() const;

private:
  FlipperTools() = default;

  std::vector<BluetoothDevice> btDevices;
  std::vector<MalwareSignature> malwareDatabase;
  std::vector<IButtonKey> storedButtons;
  bool bleScanning = false;
  uint64_t lastScanTime = 0;

  // Helper methods
  void drawTiger();
  void drawBootSequence();
  std::string getMalwareSeverityColor(const std::string& severity);
};
