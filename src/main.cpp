#include <Arduino.h>
#include "config.h"
#include "menu.h"
#include "drivers.h"
#include "initialization_manager.h"
#include "async_logger.h"
#include "non_blocking_timer.h"
#include "resource_cache.h"

// ============= GLOBAL OBJECTS =============

Menu gMenu;

// ============= SETUP =============

void setup() {
  Serial.begin(115200);

  LOG_I("=== ESP32-S3 V2 - Offensive Security Platform ===");
  LOG_I("Version: %s", VERSION);
  LOG_I("Build Date: %s", BUILD_DATE);
  LOG_I("Hardware: %s", HARDWARE_REVISION);

  InitializationManager& initMgr = InitializationManager::getInstance();

  initMgr.registerDriver("Display", []() { return Drivers::Display::begin(); }, true);
  initMgr.registerDriver("GPIO", []() { return Drivers::GPIO::begin(); }, false);
  initMgr.registerDriver("I2C", []() { return Drivers::I2C::begin(); }, false);
  initMgr.registerDriver("RTC", []() { return Drivers::RTC::begin(); }, false);
  initMgr.registerDriver("NFC", []() { return Drivers::NFC::begin(); }, false);
  initMgr.registerDriver("RFID", []() { return Drivers::RFID::begin(); }, false);
  initMgr.registerDriver("CC1101", []() { return Drivers::CC1101::begin(); }, false);
  initMgr.registerDriver("NRF24", []() { return Drivers::NRF24::begin(); }, false);
  initMgr.registerDriver("SX1262", []() { return Drivers::SX1262::begin(); }, false);
  initMgr.registerDriver("GPS", []() { return Drivers::GPS::begin(); }, false);
  initMgr.registerDriver("IR_RX", []() { return Drivers::IR::beginReceiver(); }, false);
  initMgr.registerDriver("IR_TX", []() { return Drivers::IR::beginTransmitter(); }, false);
  initMgr.registerDriver("Menu", []() { gMenu.begin(); return true; }, true);

  if (!initMgr.initializeAll()) {
    LOG_E("CRITICAL: Initialization failed!");
    AsyncLogger::getInstance().flush();
    delay(2000);
    ESP.restart();
  }

  initMgr.printInitReport();
  AsyncLogger::getInstance().flush();

  LOG_I("=== SYSTEM READY ===");
  AsyncLogger::getInstance().flush();
}

// ============= MAIN LOOP =============

void loop() {
  TimerManager& timerMgr = TimerManager::getInstance();

  gMenu.update();
  gMenu.display();

  timerMgr.updateAll();

  static uint32_t lastFlush = 0;
  if (millis() - lastFlush > 100) {
    AsyncLogger::getInstance().flush();
    lastFlush = millis();
  }
}

// ============= DEBUG HELPERS =============

void printHeapInfo() {
  Serial.printf("[HEAP] Free: %d bytes, Max block: %d bytes\n",
    ESP.getFreeHeap(),
    ESP.getMaxAllocHeap());
}

void printPSRAMInfo() {
  Serial.printf("[PSRAM] Free: %d bytes, Max block: %d bytes\n",
    ESP.getFreePsram(),
    ESP.getMaxAllocPsram());
}
