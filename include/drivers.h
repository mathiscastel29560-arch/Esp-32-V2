#ifndef DRIVERS_H
#define DRIVERS_H

#include <Arduino.h>

// ============= DRIVER DECLARATIONS =============

namespace Drivers {

  // Display Driver
  class Display {
  public:
    static bool begin();
    static void update();
    static void clear();
    static void drawString(int x, int y, const char* text, uint16_t color);
    static void drawRect(int x, int y, int w, int h, uint16_t color);
    static void fillRect(int x, int y, int w, int h, uint16_t color);
  };

  // GPIO Driver
  class GPIO {
  public:
    static bool begin();
    static int readButton(int btn);
    static void buzzOn(int duration_ms);
    static int readBatteryPercent();
    static int readBatteryVoltage();
  };

  // I2C Driver
  class I2C {
  public:
    static bool begin();
    static bool scan();
    static uint8_t readByte(uint8_t addr, uint8_t reg);
    static void writeByte(uint8_t addr, uint8_t reg, uint8_t value);
  };

  // RTC Driver
  class RTC {
  public:
    static bool begin();
    static uint32_t getUnixTime();
    static void setTime(uint32_t unix_time);
    static float getTemperature();
  };

  // NFC Driver (PN532)
  class NFC {
  public:
    static bool begin();
    static bool readCard(uint8_t* uid, uint8_t* uidLen);
    static bool writeCard(const uint8_t* data, uint16_t len);
  };

  // RFID Driver (MFRC522)
  class RFID {
  public:
    static bool begin();
    static bool readCard(uint8_t* uid, uint8_t* uidLen);
    static bool writeCard(const uint8_t* uid, const uint8_t* uidLen);
  };

  // CC1101 Driver (433MHz)
  class CC1101 {
  public:
    static bool begin();
    static bool startListening();
    static bool hasData();
    static uint8_t readData(uint8_t* buffer, uint8_t len);
    static bool transmit(const uint8_t* data, uint8_t len);
    static int getRSSI();
  };

  // NRF24 Driver (2.4GHz)
  class NRF24 {
  public:
    static bool begin();
    static void setChannel(uint8_t channel);
    static bool startListening();
    static bool hasData();
    static uint8_t readData(uint8_t* buffer, uint8_t len);
    static bool transmit(const uint8_t* data, uint8_t len);
    static int getRSSI();
  };

  // SX1262 Driver (868MHz LoRa)
  class SX1262 {
  public:
    static bool begin();
    static bool startListening();
    static bool hasData();
    static uint8_t readData(uint8_t* buffer, uint8_t len);
    static bool transmit(const uint8_t* data, uint8_t len);
    static int getRSSI();
  };

  // GPS Driver
  class GPS {
  public:
    static bool begin();
    static bool hasData();
    static double getLatitude();
    static double getLongitude();
    static uint32_t getSatelliteCount();
  };

  // IR Driver
  class IR {
  public:
    static bool beginReceiver();
    static bool beginTransmitter();
    static bool hasData();
    static uint32_t readCode();
    static void transmitCode(uint32_t code, uint8_t protocol);
  };
}

#endif // DRIVERS_H
