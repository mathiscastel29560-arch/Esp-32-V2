#include "drivers.h"
#include "config.h"
#include <Wire.h>

bool Drivers::I2C::begin() {
  // TODO: Initialize I2C with custom pins
  Wire.begin(I2C_SDA, I2C_SCL, I2C_FREQ);
  return true;
}

bool Drivers::I2C::scan() {
  // TODO: Scan I2C bus and return found devices
  return true;
}

uint8_t Drivers::I2C::readByte(uint8_t addr, uint8_t reg) {
  // TODO: Read single byte from I2C device
  Wire.beginTransmission(addr);
  Wire.write(reg);
  Wire.endTransmission();
  Wire.requestFrom(addr, 1);
  return Wire.read();
}

void Drivers::I2C::writeByte(uint8_t addr, uint8_t reg, uint8_t value) {
  // TODO: Write single byte to I2C device
  Wire.beginTransmission(addr);
  Wire.write(reg);
  Wire.write(value);
  Wire.endTransmission();
}
