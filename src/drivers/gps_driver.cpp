#include "drivers.h"
#include "config.h"

// TODO: NEO-6M GPS driver using UART1

bool Drivers::GPS::begin() {
  // TODO: Initialize UART1 for GPS at 9600 baud
  // RX=18, TX=17
  return true;
}

bool Drivers::GPS::hasData() {
  // TODO: Check if GPS data available
  return false;
}

double Drivers::GPS::getLatitude() {
  // TODO: Get latitude from GPS
  return 0.0;
}

double Drivers::GPS::getLongitude() {
  // TODO: Get longitude from GPS
  return 0.0;
}

uint32_t Drivers::GPS::getSatelliteCount() {
  // TODO: Get number of satellites
  return 0;
}
