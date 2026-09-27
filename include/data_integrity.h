#ifndef DATA_INTEGRITY_H
#define DATA_INTEGRITY_H

#include <Arduino.h>
#include <cstring>

class ChecksumCalculator {
public:
  // Simple CRC8 checksum
  static uint8_t crc8(const uint8_t* data, uint16_t len) {
    uint8_t crc = 0xFF;
    for (uint16_t i = 0; i < len; i++) {
      crc ^= data[i];
      for (int j = 0; j < 8; j++) {
        if (crc & 0x80) {
          crc = (crc << 1) ^ 0x07;
        } else {
          crc = crc << 1;
        }
      }
    }
    return crc;
  }

  // Fletcher's checksum (16-bit)
  static uint16_t fletcher16(const uint8_t* data, uint16_t len) {
    uint16_t sum1 = 0, sum2 = 0;

    for (uint16_t i = 0; i < len; i++) {
      sum1 = (sum1 + data[i]) % 255;
      sum2 = (sum2 + sum1) % 255;
    }

    return (sum2 << 8) | sum1;
  }

  // Adler-32 checksum (32-bit)
  static uint32_t adler32(const uint8_t* data, uint16_t len) {
    uint32_t a = 1, b = 0;
    const uint32_t MOD_ADLER = 65521;

    for (uint16_t i = 0; i < len; i++) {
      a = (a + data[i]) % MOD_ADLER;
      b = (b + a) % MOD_ADLER;
    }

    return (b << 16) | a;
  }
};

class DataValidator {
public:
  bool validateChecksum(const uint8_t* data, uint16_t len, uint8_t expectedCrc) {
    uint8_t calculated = ChecksumCalculator::crc8(data, len);
    return calculated == expectedCrc;
  }

  bool validateIntegrity(const uint8_t* data, uint16_t len,
                        const uint8_t* signature, uint16_t sigLen) {
    if (sigLen < 4) return false;

    uint32_t expectedAdler = *(uint32_t*)signature;
    uint32_t calculatedAdler = ChecksumCalculator::adler32(data, len);

    return expectedAdler == calculatedAdler;
  }

  void printChecksum(const uint8_t* data, uint16_t len) {
    Serial.printf("CRC8: 0x%02X\n", ChecksumCalculator::crc8(data, len));
    Serial.printf("Fletcher16: 0x%04X\n", ChecksumCalculator::fletcher16(data, len));
    Serial.printf("Adler32: 0x%08lX\n", ChecksumCalculator::adler32(data, len));
  }
};

#endif
