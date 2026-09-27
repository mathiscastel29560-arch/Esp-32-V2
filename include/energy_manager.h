#ifndef ENERGY_MANAGER_H
#define ENERGY_MANAGER_H

#include <Arduino.h>
#include "config_manager.h"

// ============= POWER STATE DEFINITIONS =============

enum class PowerState {
  FULL_POWER = 0,      // All systems active, 240MHz
  HIGH_POWER = 1,      // Most systems active, 160MHz
  BALANCED = 2,        // Selective systems, 80MHz
  LOW_POWER = 3,       // Minimal systems, 40MHz
  CRITICAL = 4         // Emergency mode, 20MHz, WiFi/BLE off
};

enum class BatteryHealth {
  EXCELLENT = 0,       // > 80%
  GOOD = 1,            // 60-80%
  FAIR = 2,            // 40-60%
  POOR = 3,            // 20-40%
  CRITICAL = 4         // < 20%
};

struct BatteryStats {
  uint8_t currentPercent;
  uint8_t minPercent;
  uint8_t maxPercent;
  float voltage;        // Current voltage in volts
  float avgCurrent;     // Average current draw in mA
  BatteryHealth health;
  uint32_t estimatedRuntime; // Remaining seconds
  uint32_t chargeTime;       // Estimated charge time
  uint16_t cycleCount;
};

struct PowerConsumption {
  const char* moduleName;
  float powerMW;        // Milliwatts
  bool active;
  uint32_t uptimeMs;    // How long it's been active

  PowerConsumption() : moduleName(""), powerMW(0), active(false), uptimeMs(0) {}
};

struct EnergyReport {
  uint8_t batteryPercent;
  float totalPowerMW;
  uint16_t moduleCount;
  PowerConsumption* modules;
  uint32_t timeUntilShutdown; // ms
  uint32_t reportTime;

  EnergyReport() : batteryPercent(0), totalPowerMW(0), moduleCount(0),
                   modules(nullptr), timeUntilShutdown(0), reportTime(0) {}
};

// ============= ENERGY MANAGER =============

class EnergyManager {
public:
  static EnergyManager& getInstance() {
    static EnergyManager instance;
    return instance;
  }

  // Initialization & update
  void begin();
  void update();

  // Battery monitoring
  uint8_t getBatteryPercent() const { return batteryStats.currentPercent; }
  float getBatteryVoltage() const { return batteryStats.voltage; }
  BatteryHealth getBatteryHealth() const { return batteryStats.health; }
  uint32_t getEstimatedRuntime() const { return batteryStats.estimatedRuntime; }
  BatteryStats* getBatteryStats() { return &batteryStats; }

  // Power state management
  void setPowerState(PowerState newState);
  PowerState getPowerState() const { return currentPowerState; }
  void transitionToPowerState(PowerState target);

  // Dynamic power scaling
  void enableAutoPowerScaling(bool enable) { autoPowerScaling = enable; }
  void setAutoPowerThresholds(uint8_t high, uint8_t low);

  // Module power management
  void setModulePower(const char* moduleName, bool enabled, float powerMW);
  void getModulePower(const char* moduleName, bool& enabled, float& powerMW);

  // Predictions & estimates
  uint32_t estimateRuntimeForAttack(const char* attackName, float avgPowerMW);
  bool canRunAttack(const char* attackName, uint32_t durationMs);

  // Power consumption tracking
  void startTrackingModule(const char* moduleName, float powerMW);
  void stopTrackingModule(const char* moduleName);
  float getTotalPowerConsumption() const;
  uint16_t getActiveModuleCount() const;

  // Alerts & notifications
  void setBatteryAlertThresholds(uint8_t critical, uint8_t warning);
  bool isBatteryLow() const { return batteryStats.currentPercent < criticalBatteryThreshold; }
  bool isBatteryWarning() const { return batteryStats.currentPercent < warningBatteryThreshold; }
  void checkBatteryAlerts();

  // Energy efficiency
  void enableEnergyOptimizations();
  void disableEnergyOptimizations();
  void optimizeForAttack(const char* attackName);

  // Reports
  EnergyReport* generateReport();
  void printEnergyReport();
  void printDetailedAnalysis();

  // CPU frequency control
  void setCPUFrequency(uint32_t freqMHz);
  uint32_t getCPUFrequency() const { return cpuFrequencyMHz; }

  // WiFi/BLE power control
  void enableWiFi(bool enable);
  void enableBLE(bool enable);
  bool isWiFiEnabled() const { return wifiEnabled; }
  bool isBLEEnabled() const { return bleEnabled; }

  // Charger detection
  bool isCharging() const { return chargingDetected; }
  void setChargingState(bool charging) { chargingDetected = charging; }

private:
  EnergyManager();

  BatteryStats batteryStats;
  PowerState currentPowerState;
  PowerState targetPowerState;

  std::vector<PowerConsumption*> trackedModules;

  // Thresholds
  uint8_t criticalBatteryThreshold;  // Default 15%
  uint8_t warningBatteryThreshold;   // Default 30%
  uint8_t autoPowerHighThreshold;    // Default 60%
  uint8_t autoPowerLowThreshold;     // Default 40%

  // State
  bool autoPowerScaling;
  bool energyOptimizationsEnabled;
  uint32_t cpuFrequencyMHz;
  bool wifiEnabled;
  bool bleEnabled;
  bool chargingDetected;

  // Timing
  uint32_t lastUpdateTime;
  uint32_t lastBatteryCheckTime;

  // Power profiles (watts)
  static const float POWER_PROFILE[5][3]; // [PowerState][CPU/WiFi/Others]

  // Private methods
  void updateBatteryStats();
  void readBatteryADC();
  void calculateEstimatedRuntime();
  void applyPowerState(PowerState state);
  void manageAutoScaling();
  void calculateBatteryHealth();
  void notifyLowBattery();
  PowerConsumption* findModule(const char* moduleName);
};

// ============= POWER SCHEDULER =============

struct PowerSchedule {
  const char* name;
  PowerState targetState;
  uint32_t startTime;    // Time of day in seconds
  uint32_t duration;     // Duration in seconds

  PowerSchedule() : name(""), targetState(PowerState::BALANCED),
                    startTime(0), duration(0) {}
};

class PowerScheduler {
public:
  static PowerScheduler& getInstance() {
    static PowerScheduler instance;
    return instance;
  }

  void addSchedule(const PowerSchedule& schedule);
  void removeSchedule(const char* name);
  void update();

  uint16_t getScheduleCount() const { return schedules.size(); }

private:
  PowerScheduler() {}
  std::vector<PowerSchedule*> schedules;
};

#endif // ENERGY_MANAGER_H
