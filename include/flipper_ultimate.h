#pragma once

#include <string>
#include <vector>
#include <cstdint>

// Final Flipper Tools - LoRa/Wireless & Cellular/LTE (5/5)
class FlipperUltimate {
public:
  // ============ LORA/WIRELESS TOOLS ============
  struct LoRaDevice {
    std::string deviceId;
    int32_t rssi;              // Signal strength
    int8_t snr;                // Signal to noise ratio
    uint32_t frequency;
    std::string spreadingFactor;
    uint8_t bandwidth;         // kHz
    uint32_t lastSeen;
    bool isGateway;
  };

  struct LoRaPacket {
    uint8_t data[256];
    uint16_t length;
    int32_t rssi;
    int8_t snr;
    uint32_t timestamp;
    std::string sourceId;
    std::string destId;
    bool isCrypted;
  };

  // ============ CELLULAR/LTE TOOLS ============
  struct CellularNetwork {
    std::string mcc;           // Mobile Country Code
    std::string mnc;           // Mobile Network Code
    std::string operatorName;
    uint32_t lac;              // Location Area Code
    uint32_t cellId;
    int32_t rsrp;              // Reference Signal Received Power
    int32_t sinr;              // Signal to Interference + Noise Ratio
    uint8_t bands[16];         // Active LTE bands
    uint8_t bandCount;
    std::string technology;    // 4G, 5G, LTE-M, NB-IoT
  };

  struct CellularGateway {
    std::string imsi;          // Subscriber identity
    std::string imei;          // Device identity
    std::string msin;          // Subscriber number
    uint32_t tmsi;             // Temporary identity
    std::string lac;           // Location Area Code
    bool isLogged;
    bool supportsFakeBS;       // Fake base station support
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
