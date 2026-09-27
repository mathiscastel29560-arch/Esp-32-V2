#include "energy_manager.h"
#include "logging_system.h"

// Power consumption profiles (milliwatts)
// Format: [PowerState][0=CPU, 1=WiFi/BLE, 2=RF/Display/Other]
const float EnergyManager::POWER_PROFILE[5][3] = {
  {240, 200, 300},   // FULL_POWER: 240MHz CPU + all modules
  {160, 150, 200},   // HIGH_POWER: 160MHz CPU + most modules
  {80, 80, 100},     // BALANCED: 80MHz CPU + selective modules
  {40, 30, 40},      // LOW_POWER: 40MHz CPU + minimal modules
  {20, 0, 10}        // CRITICAL: 20MHz CPU + emergency only
};

// ============= ENERGY MANAGER IMPLEMENTATION =============

EnergyManager::EnergyManager()
  : currentPowerState(PowerState::BALANCED),
    targetPowerState(PowerState::BALANCED),
    criticalBatteryThreshold(15),
    warningBatteryThreshold(30),
    autoPowerHighThreshold(60),
    autoPowerLowThreshold(40),
    autoPowerScaling(false),
    energyOptimizationsEnabled(false),
    cpuFrequencyMHz(240),
    wifiEnabled(true),
    bleEnabled(true),
    chargingDetected(false),
    lastUpdateTime(0),
    lastBatteryCheckTime(0) {

  // Initialize battery stats
  batteryStats.currentPercent = 100;
  batteryStats.minPercent = 100;
  batteryStats.maxPercent = 100;
  batteryStats.voltage = 4.2f;
  batteryStats.avgCurrent = 0;
  batteryStats.health = BatteryHealth::EXCELLENT;
  batteryStats.estimatedRuntime = 3600; // 1 hour estimate
  batteryStats.cycleCount = 0;
}

void EnergyManager::begin() {
  Logger::getInstance().info("Energy", "Manager initialized");
  updateBatteryStats();
}

void EnergyManager::update() {
  uint32_t now = millis();

  // Update battery stats every 1 second
  if (now - lastBatteryCheckTime > 1000) {
    updateBatteryStats();
    lastBatteryCheckTime = now;
  }

  // Check battery alerts
  checkBatteryAlerts();

  // Manage auto power scaling
  if (autoPowerScaling) {
    manageAutoScaling();
  }

  // Apply pending power state transitions
  if (targetPowerState != currentPowerState) {
    applyPowerState(targetPowerState);
    currentPowerState = targetPowerState;
  }
}

void EnergyManager::setPowerState(PowerState newState) {
  if (newState == currentPowerState) return;

  targetPowerState = newState;

  char msg[128];
  const char* stateStr = "";
  switch (newState) {
    case PowerState::FULL_POWER: stateStr = "FULL_POWER"; break;
    case PowerState::HIGH_POWER: stateStr = "HIGH_POWER"; break;
    case PowerState::BALANCED: stateStr = "BALANCED"; break;
    case PowerState::LOW_POWER: stateStr = "LOW_POWER"; break;
    case PowerState::CRITICAL: stateStr = "CRITICAL"; break;
  }

  snprintf(msg, 127, "Power state transition: %s", stateStr);
  Logger::getInstance().info("Energy", msg);
}

void EnergyManager::transitionToPowerState(PowerState target) {
  applyPowerState(target);
  currentPowerState = target;
  targetPowerState = target;
}

void EnergyManager::setAutoPowerThresholds(uint8_t high, uint8_t low) {
  autoPowerHighThreshold = high;
  autoPowerLowThreshold = low;
  Logger::getInstance().info("Energy", "Auto power thresholds updated");
}

void EnergyManager::setModulePower(const char* moduleName, bool enabled, float powerMW) {
  PowerConsumption* module = findModule(moduleName);

  if (module) {
    module->active = enabled;
    module->powerMW = powerMW;
  } else {
    PowerConsumption* newModule = new PowerConsumption();
    newModule->moduleName = moduleName;
    newModule->active = enabled;
    newModule->powerMW = powerMW;
    trackedModules.push_back(newModule);
  }
}

void EnergyManager::getModulePower(const char* moduleName, bool& enabled, float& powerMW) {
  PowerConsumption* module = findModule(moduleName);
  if (module) {
    enabled = module->active;
    powerMW = module->powerMW;
  } else {
    enabled = false;
    powerMW = 0;
  }
}

uint32_t EnergyManager::estimateRuntimeForAttack(const char* attackName, float avgPowerMW) {
  if (batteryStats.currentPercent == 0) return 0;

  // LiPo battery: 3S 2000mAh = ~11.1V nominal
  // Energy = 11.1V * 2Ah = 22.2Wh = 80000mWs
  float batteryEnergy = 22.2f * (batteryStats.currentPercent / 100.0f);

  // Runtime = energy / power
  uint32_t runtimeSeconds = (uint32_t)((batteryEnergy * 3600.0f) / avgPowerMW);

  return runtimeSeconds;
}

bool EnergyManager::canRunAttack(const char* attackName, uint32_t durationMs) {
  float avgPower = getTotalPowerConsumption();
  uint32_t estimatedRuntime = estimateRuntimeForAttack(attackName, avgPower);

  return (estimatedRuntime * 1000) >= durationMs;
}

void EnergyManager::startTrackingModule(const char* moduleName, float powerMW) {
  setModulePower(moduleName, true, powerMW);

  char msg[128];
  snprintf(msg, 127, "Tracking %s (%.1f mW)", moduleName, powerMW);
  Logger::getInstance().info("Energy", msg);
}

void EnergyManager::stopTrackingModule(const char* moduleName) {
  PowerConsumption* module = findModule(moduleName);
  if (module) {
    module->active = false;
  }
}

float EnergyManager::getTotalPowerConsumption() const {
  float total = 0;
  for (auto* module : trackedModules) {
    if (module->active) {
      total += module->powerMW;
    }
  }
  return total;
}

uint16_t EnergyManager::getActiveModuleCount() const {
  uint16_t count = 0;
  for (auto* module : trackedModules) {
    if (module->active) count++;
  }
  return count;
}

void EnergyManager::setBatteryAlertThresholds(uint8_t critical, uint8_t warning) {
  criticalBatteryThreshold = critical;
  warningBatteryThreshold = warning;
  Logger::getInstance().info("Energy", "Battery alert thresholds updated");
}

void EnergyManager::checkBatteryAlerts() {
  if (isBatteryLow()) {
    if (currentPowerState != PowerState::CRITICAL) {
      notifyLowBattery();
      setPowerState(PowerState::CRITICAL);
    }
  } else if (isBatteryWarning()) {
    if (currentPowerState == PowerState::FULL_POWER) {
      Logger::getInstance().warn("Energy", "Battery warning: consider reducing power");
    }
  }
}

void EnergyManager::enableEnergyOptimizations() {
  energyOptimizationsEnabled = true;
  Logger::getInstance().info("Energy", "Energy optimizations enabled");
}

void EnergyManager::disableEnergyOptimizations() {
  energyOptimizationsEnabled = false;
  Logger::getInstance().info("Energy", "Energy optimizations disabled");
}

void EnergyManager::optimizeForAttack(const char* attackName) {
  // Determine power profile for specific attack
  if (strstr(attackName, "Deauth") || strstr(attackName, "Jamming")) {
    setPowerState(PowerState::HIGH_POWER); // Needs sustained power
  } else if (strstr(attackName, "Scan")) {
    setPowerState(PowerState::BALANCED);   // Moderate power
  } else {
    setPowerState(PowerState::LOW_POWER);  // Default conservative
  }
}

EnergyReport* EnergyManager::generateReport() {
  EnergyReport* report = new EnergyReport();
  report->batteryPercent = batteryStats.currentPercent;
  report->totalPowerMW = getTotalPowerConsumption();
  report->moduleCount = getActiveModuleCount();
  report->reportTime = millis();

  // Estimate time until shutdown
  if (report->totalPowerMW > 0) {
    float batteryEnergy = 22.2f * (batteryStats.currentPercent / 100.0f);
    report->timeUntilShutdown = (uint32_t)((batteryEnergy * 3600000.0f) / report->totalPowerMW);
  }

  return report;
}

void EnergyManager::printEnergyReport() {
  EnergyReport* report = generateReport();

  Serial.println("\n========== Energy Report ==========");
  Serial.printf("Battery: %u%%\n", report->batteryPercent);
  Serial.printf("Total Power: %.1f mW\n", report->totalPowerMW);
  Serial.printf("Active Modules: %u\n", report->moduleCount);
  Serial.printf("Time Until Shutdown: %u s\n", report->timeUntilShutdown / 1000);
  Serial.printf("Current Power State: %u\n", (uint8_t)currentPowerState);
  Serial.println("==================================\n");

  delete report;
}

void EnergyManager::printDetailedAnalysis() {
  Serial.println("\n========== Detailed Energy Analysis ==========");
  Serial.printf("Battery Voltage: %.2f V\n", batteryStats.voltage);
  Serial.printf("Battery Health: %u\n", (uint8_t)batteryStats.health);
  Serial.printf("Estimated Runtime: %u s\n", batteryStats.estimatedRuntime);
  Serial.printf("CPU Frequency: %u MHz\n", cpuFrequencyMHz);
  Serial.printf("WiFi: %s, BLE: %s\n", wifiEnabled ? "ON" : "OFF",
                                        bleEnabled ? "ON" : "OFF");

  Serial.println("\nModule Power Consumption:");
  for (auto* module : trackedModules) {
    Serial.printf("  %s: %.1f mW (%s)\n", module->moduleName, module->powerMW,
                  module->active ? "ACTIVE" : "IDLE");
  }

  Serial.println("==============================================\n");
}

void EnergyManager::setCPUFrequency(uint32_t freqMHz) {
  cpuFrequencyMHz = freqMHz;
  // TODO: Set ESP32 CPU frequency using setCpuFrequencyMhz()

  char msg[128];
  snprintf(msg, 127, "CPU frequency set to %u MHz", freqMHz);
  Logger::getInstance().info("Energy", msg);
}

void EnergyManager::enableWiFi(bool enable) {
  wifiEnabled = enable;
  // TODO: Control WiFi power
}

void EnergyManager::enableBLE(bool enable) {
  bleEnabled = enable;
  // TODO: Control BLE power
}

void EnergyManager::updateBatteryStats() {
  readBatteryADC();
  calculateEstimatedRuntime();
  calculateBatteryHealth();
}

void EnergyManager::readBatteryADC() {
  // Read from GPIO 7 (battery ADC)
  // LiPo 3S: 12.6V max (fully charged) -> ADC 4095
  // LiPo 3S: 9.0V min (cutoff) -> ADC ~2880

  // TODO: Implement actual ADC reading
  // For now, simulate battery drain
  if (!chargingDetected && batteryStats.currentPercent > 0) {
    batteryStats.currentPercent = max(0, (int)batteryStats.currentPercent - 1);
  }

  // Map percentage to voltage
  batteryStats.voltage = 9.0f + (batteryStats.currentPercent / 100.0f) * 3.6f;
}

void EnergyManager::calculateEstimatedRuntime() {
  float currentPower = getTotalPowerConsumption();
  if (currentPower == 0) {
    batteryStats.estimatedRuntime = 3600; // Default 1 hour if no modules active
    return;
  }

  float batteryEnergy = 22.2f * (batteryStats.currentPercent / 100.0f);
  batteryStats.estimatedRuntime = (uint32_t)((batteryEnergy * 3600.0f) / currentPower);
}

void EnergyManager::applyPowerState(PowerState state) {
  setCPUFrequency(state == PowerState::FULL_POWER ? 240 :
                  state == PowerState::HIGH_POWER ? 160 :
                  state == PowerState::BALANCED ? 80 :
                  state == PowerState::LOW_POWER ? 40 : 20);

  if (state == PowerState::CRITICAL) {
    enableWiFi(false);
    enableBLE(false);
  }
}

void EnergyManager::manageAutoScaling() {
  if (batteryStats.currentPercent >= autoPowerHighThreshold) {
    setPowerState(PowerState::BALANCED);
  } else if (batteryStats.currentPercent <= autoPowerLowThreshold) {
    setPowerState(PowerState::LOW_POWER);
  }
}

void EnergyManager::calculateBatteryHealth() {
  if (batteryStats.currentPercent > 80) {
    batteryStats.health = BatteryHealth::EXCELLENT;
  } else if (batteryStats.currentPercent > 60) {
    batteryStats.health = BatteryHealth::GOOD;
  } else if (batteryStats.currentPercent > 40) {
    batteryStats.health = BatteryHealth::FAIR;
  } else if (batteryStats.currentPercent > 20) {
    batteryStats.health = BatteryHealth::POOR;
  } else {
    batteryStats.health = BatteryHealth::CRITICAL;
  }
}

void EnergyManager::notifyLowBattery() {
  Logger::getInstance().error("Energy", "CRITICAL: Battery level critical!");
  // TODO: Show visual/audio alert on device
}

PowerConsumption* EnergyManager::findModule(const char* moduleName) {
  for (auto* module : trackedModules) {
    if (strcmp(module->moduleName, moduleName) == 0) {
      return module;
    }
  }
  return nullptr;
}

// ============= POWER SCHEDULER IMPLEMENTATION =============

void PowerScheduler::addSchedule(const PowerSchedule& schedule) {
  PowerSchedule* newSched = new PowerSchedule(schedule);
  schedules.push_back(newSched);
  Logger::getInstance().info("Scheduler", "Power schedule added");
}

void PowerScheduler::removeSchedule(const char* name) {
  for (uint16_t i = 0; i < schedules.size(); i++) {
    if (strcmp(schedules[i]->name, name) == 0) {
      delete schedules[i];
      schedules.erase(schedules.begin() + i);
      return;
    }
  }
}

void PowerScheduler::update() {
  // TODO: Implement schedule checking and power state updates
}
