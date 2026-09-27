#include "drivers.h"
#include "config.h"

// TODO: PN532 NFC reader driver using I2C

bool Drivers::NFC::begin() {
  // TODO: Initialize PN532 via I2C at address 0x24
  return true;
}

bool Drivers::NFC::readCard(uint8_t* uid, uint8_t* uidLen) {
  // TODO: Read NFC card UID
  return false;
}

bool Drivers::NFC::writeCard(const uint8_t* data, uint16_t len) {
  // TODO: Write data to NFC card
  return false;
}
