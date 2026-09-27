#include "drivers.h"
#include "config.h"

// TODO: MFRC522 RFID reader driver using SPI

bool Drivers::RFID::begin() {
  // TODO: Initialize MFRC522 via SPI with CS=26
  return true;
}

bool Drivers::RFID::readCard(uint8_t* uid, uint8_t* uidLen) {
  // TODO: Read RFID 125kHz card UID
  return false;
}

bool Drivers::RFID::writeCard(const uint8_t* uid, const uint8_t* uidLen) {
  // TODO: Write UID to RFID card
  return false;
}
