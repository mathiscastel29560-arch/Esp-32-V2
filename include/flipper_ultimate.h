#pragma once

#include <string>
#include <vector>
#include <cstdint>

// Final Flipper Tools - LoRa/Wireless & Cellular/LTE (5/5)
// Ultimate professional network hacking and security testing framework
class FlipperUltimate {
public:
  // Error codes for operations
  enum ResultCode {
    RESULT_SUCCESS = 0,
    RESULT_ERROR_INVALID_PARAM = -1,
    RESULT_ERROR_HARDWARE = -2,
    RESULT_ERROR_TIMEOUT = -3,
    RESULT_ERROR_NOT_FOUND = -4,
    RESULT_ERROR_CONNECTION = -5,
    RESULT_ERROR_MEMORY = -6,
    RESULT_ERROR_SECURITY = -7,
    RESULT_ERROR_UNKNOWN = -99
  };

  // === LORA/WIRELESS STRUCTURES ===
  struct LoRaDevice {
    std::string deviceId;        // Device identifier
    int32_t rssi;                // Signal strength (dBm)
    int8_t snr;                  // Signal to noise ratio (dB)
    uint32_t frequency;          // Frequency in Hz
    std::string spreadingFactor; // SF7-SF12
    uint8_t bandwidth;           // Bandwidth in kHz
    uint32_t lastSeen;           // Timestamp
    bool isGateway;              // Is LoRaWAN gateway
    std::string version;         // LoRa version

    bool isValid() const {
      return !deviceId.empty() && frequency > 0 && rssi > -200;
    }
  };

  struct LoRaPacket {
    uint8_t data[256];           // Payload data
    uint16_t length;             // Payload length
    int32_t rssi;                // Received signal strength
    int8_t snr;                  // Signal to noise ratio
    uint32_t timestamp;          // Packet timestamp
    std::string sourceId;        // Source device ID
    std::string destId;          // Destination device ID
    bool isCrypted;              // Encrypted payload
    uint16_t crc;                // CRC checksum

    bool isValid() const {
      return length > 0 && length <= 256;
    }
  };

  // === CELLULAR/LTE STRUCTURES ===
  struct CellularNetwork {
    std::string mcc;             // Mobile Country Code (e.g., "310")
    std::string mnc;             // Mobile Network Code (e.g., "410")
    std::string operatorName;    // Operator name (e.g., "Verizon")
    uint32_t lac;                // Location Area Code
    uint32_t cellId;             // Cell ID
    int32_t rsrp;                // Reference Signal Received Power (dBm)
    int32_t sinr;                // Signal to Interference + Noise Ratio
    uint8_t bands[16];           // Active LTE bands
    uint8_t bandCount;           // Number of active bands
    std::string technology;      // 4G, 5G, LTE-M, NB-IoT
    bool isConnected;            // Currently connected

    bool isValid() const {
      return !mcc.empty() && !mnc.empty() && rsrp > -200;
    }
  };

  struct CellularGateway {
    std::string imsi;            // International Mobile Subscriber Identity
    std::string imei;            // International Mobile Equipment Identity
    std::string msin;            // Mobile Subscriber Identification Number
    uint32_t tmsi;               // Temporary Mobile Subscriber Identity
    std::string lac;             // Location Area Code
    bool isLogged;               // Logged into network
    bool supportsFakeBS;         // Supports fake base station
    uint64_t connectionTime;     // Connection timestamp
  };

  // Singleton
  static FlipperUltimate& getInstance() {
    static FlipperUltimate instance;
    return instance;
  }

  // ========== LORA/WIRELESS SCANNING ==========
  bool initLoRa(uint32_t frequency = 868000000, uint8_t spreadFactor = 7);
  void scanLoRaDevices(uint32_t durationMs = 30000);
  std::vector<LoRaDevice> getDiscoveredLoRaDevices() const;
  bool sendLoRaPacket(const std::string& destId, const std::vector<uint8_t>& data);
  std::vector<LoRaPacket> captureLoRaPackets(uint32_t durationMs);
  bool jamLoRaNetwork(uint32_t power = 50);
  void analyzeLoRaTraffic();
  bool decryptLoRaPayload(const LoRaPacket& packet, const std::string& key);

  // LoRa Frequencies
  enum LoRaBand {
    LORA_EU868,      // 868 MHz (Europe)
    LORA_US915,      // 915 MHz (Americas)
    LORA_AS923,      // 923 MHz (Asia)
    LORA_AU915,      // 915 MHz (Australia)
    LORA_IN865,      // 865 MHz (India)
  };

  // ========== CELLULAR/LTE SCANNING ==========
  bool initCellularModem();
  void scanCellularNetworks(uint32_t durationMs = 60000);
  std::vector<CellularNetwork> getDiscoveredNetworks() const;
  CellularGateway getCurrentConnection() const;
  bool connectToNetwork(const CellularNetwork& network);
  bool disconnectNetwork();

  // LTE Attacks
  bool performFakeBSAttack(const std::string& operatorName);
  bool performIMSIGrab();                    // Capture IMSI
  bool performSimSwap(const std::string& phoneNumber);
  bool performSSLStrip();
  bool performDNSHijack();
  bool captureLocationData();
  bool analyzeSignalStrength();
  bool detectFakeBaseStations();

  // 5G Specific
  bool scan5G();
  std::vector<std::string> get5GNetworks() const;
  bool analyze5GSecurityGaps();

  // Signal Analysis
  struct SignalAnalysis {
    int32_t minRSSI;
    int32_t maxRSSI;
    int32_t avgRSSI;
    int32_t minSINR;
    int32_t maxSINR;
    float coveragePercentage;
    std::string dominantTechnology;
  };

  SignalAnalysis performDetailedAnalysis();

  // Status & Monitoring
  bool isHealthy() const;
  std::string getStatus() const;
  uint32_t getLastErrorCode() const;
  std::string getLastError() const;

private:
  FlipperUltimate() = default;

  // LoRa state
  bool loraInitialized = false;
  uint32_t loraFrequency = 868000000;
  std::vector<LoRaDevice> loraDevices;
  std::vector<LoRaPacket> loraPackets;

  // Cellular state
  bool cellularInitialized = false;
  CellularGateway currentConnection = {"", "", "", 0, "", false, false};
  std::vector<CellularNetwork> discoveredNetworks;
  bool fakeBasStationActive = false;

  // Error tracking
  uint32_t lastError = 0;
  std::string lastErrorMsg = "";

  // Helper methods
  void parseLoRaDeviceInfo(LoRaDevice& device);
  void parseCellularInfo(CellularNetwork& network);
  void calculateSignalStrength();
};
