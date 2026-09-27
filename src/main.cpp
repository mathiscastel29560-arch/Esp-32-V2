#include <Arduino.h>
#include "config.h"
#include "menu.h"
#include "drivers.h"

// ============= GLOBAL OBJECTS =============

Menu gMenu;

// ============= SETUP =============

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n\n=== ESP32-S3 V2 - Offensive Security Platform ===");
  Serial.println("Version: " VERSION);
  Serial.println("Build Date: " BUILD_DATE);
  Serial.println("Hardware: " HARDWARE_REVISION);
  Serial.println("========================================\n");

  // Initialize display first
  Serial.print("[INIT] Display...");
  if (!Drivers::Display::begin()) {
    Serial.println(" FAILED!");
    Serial.println("[ERROR] Display initialization failed - system unstable!");
    delay(2000);
    ESP.restart();  // FIX: Attempt restart instead of hard freeze
  }
  Serial.println(" OK");

  // Initialize GPIO (buttons, buzzer, battery)
  Serial.print("[INIT] GPIO...");
  if (!Drivers::GPIO::begin()) {
    Serial.println(" WARNING - continuing anyway");
  } else {
    Serial.println(" OK");
  }

  // Initialize I2C bus
  Serial.print("[INIT] I2C Bus...");
  if (!Drivers::I2C::begin()) {
    Serial.println(" FAILED!");
    Serial.println("[ERROR] I2C bus initialization failed - RTC/NFC may not work!");
    // I2C not critical - continue with warning
  } else {
    Serial.println(" OK");
  }

  // Initialize RTC
  Serial.print("[INIT] RTC (DS3231)...");
  if (!Drivers::RTC::begin()) {
    Serial.println(" WARNING - clock not available");
  } else {
    Serial.println(" OK");
  }

  // Initialize NFC Reader
  Serial.print("[INIT] NFC Reader (PN532)...");
  if (!Drivers::NFC::begin()) {
    Serial.println(" WARNING - NFC not available");
  } else {
    Serial.println(" OK");
  }

  // Initialize RFID Reader
  Serial.print("[INIT] RFID Reader (MFRC522)...");
  if (!Drivers::RFID::begin()) {
    Serial.println(" WARNING - RFID not available");
  } else {
    Serial.println(" OK");
  }

  // Initialize CC1101 (433MHz)
  Serial.print("[INIT] CC1101 (433MHz)...");
  if (!Drivers::CC1101::begin()) {
    Serial.println(" WARNING - CC1101 not available");
  } else {
    Serial.println(" OK");
  }

  // Initialize NRF24 (2.4GHz)
  Serial.print("[INIT] NRF24 (2.4GHz)...");
  if (!Drivers::NRF24::begin()) {
    Serial.println(" WARNING - NRF24 not available");
  } else {
    Serial.println(" OK");
  }

  // Initialize SX1262 (868MHz LoRa)
  Serial.print("[INIT] SX1262 (868MHz)...");
  if (!Drivers::SX1262::begin()) {
    Serial.println(" WARNING - SX1262 not available");
  } else {
    Serial.println(" OK");
  }

  // Initialize GPS
  Serial.print("[INIT] GPS (NEO-6M)...");
  if (!Drivers::GPS::begin()) {
    Serial.println(" WARNING - GPS not available");
  } else {
    Serial.println(" OK");
  }

  // Initialize IR (Receiver + Transmitter)
  Serial.print("[INIT] IR Receiver...");
  if (!Drivers::IR::beginReceiver()) {
    Serial.println(" WARNING - IR receiver not available");
  } else {
    Serial.println(" OK");
  }

  Serial.print("[INIT] IR Transmitter...");
  if (!Drivers::IR::beginTransmitter()) {
    Serial.println(" WARNING - IR transmitter not available");
  } else {
    Serial.println(" OK");
  }

  // Initialize Menu
  Serial.print("[INIT] Menu System...");
  gMenu.begin();
  Serial.println(" OK");

  Serial.println("\n=== INITIALIZATION COMPLETE ===\n");
}

// ============= MAIN LOOP =============

void loop() {
  // Update menu display
  gMenu.update();
  gMenu.display();

  // Check button inputs
  for (int i = 0; i < 4; i++) {
    // Will implement button polling
  }

  // Small delay to prevent watchdog trigger
  delay(50);
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
