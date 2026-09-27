#include "drivers.h"
#include "config.h"

// TODO: CC1101 433MHz driver using RadioLib
// Frequency: 433.92 MHz
// Power: 10 dBm
// Modulation: 2-FSK

bool Drivers::CC1101::begin() {
  // TODO: Initialize CC1101 via SPI with RadioLib
  // Set CS=10, GDO0=4 pins
  // Configure frequency, modulation, power
  return true;
}

bool Drivers::CC1101::startListening() {
  // TODO: Start RX mode listening for signals
  return true;
}

bool Drivers::CC1101::hasData() {
  // TODO: Check if data received
  return false;
}

uint8_t Drivers::CC1101::readData(uint8_t* buffer, uint8_t len) {
  // TODO: Read received packet from CC1101
  return 0;
}

bool Drivers::CC1101::transmit(const uint8_t* data, uint8_t len) {
  // TODO: Transmit packet via CC1101
  return true;
}

int Drivers::CC1101::getRSSI() {
  // TODO: Get RSSI (signal strength) in dBm
  return -100;
}
