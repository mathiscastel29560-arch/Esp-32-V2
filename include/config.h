#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ============= DISPLAY =============
#define TFT_MOSI 11
#define TFT_MISO 13
#define TFT_SCLK 12
#define TFT_CS 37        // FIXED: was 21 (conflict with BUZZER_PIN)
#define TFT_DC 19        // FIXED: was 8 (conflict with I2C_SDA)
#define TFT_RST -1
#define TOUCH_CS 16      // FIXED: was 9 (conflict with I2C_SCL)

// ============= GPIO =============
#define BTN_UP 1
#define BTN_DOWN 2
#define BTN_SELECT 6
#define BTN_BACK 42
#define BUZZER_PIN 21
#define IR_RX_PIN 39
#define IR_TX_PIN 38
#define BATTERY_ADC_PIN 7

// ============= I2C =============
#define I2C_SDA 8
#define I2C_SCL 9
#define I2C_FREQ 400000

// RTC DS3231
#define RTC_ADDR 0x68

// NFC PN532
#define NFC_ADDR 0x24

// ============= UART =============
#define GPS_RX 18
#define GPS_TX 17
#define GPS_BAUD 9600

// ============= SPI (RF Modules) =============
#define SPI_MOSI 11
#define SPI_MISO 13
#define SPI_SCLK 12

// CC1101 433MHz
#define CC1101_CS 10
#define CC1101_GDO0 4
#define CC1101_GDO2 40

// NRF24 2.4GHz
#define NRF24_CS 14
#define NRF24_CE 15
#define NRF24_IRQ 41

// SX1262 868MHz (LoRa)
#define SX1262_CS 5
#define SX1262_RST 3
#define SX1262_BUSY 44       // FIXED: was 2 (conflict with BTN_DOWN)
#define SX1262_DIO1 43       // FIXED: was 1 (conflict with BTN_UP)

// MFRC522 RFID
#define MFRC522_CS 26
#define MFRC522_RST 27

// ============= POWER =============
#define BATTERY_ADC_CHANNEL ADC1_CHANNEL_6
#define BATTERY_MIN_MV 3000
#define BATTERY_MAX_MV 4200
#define BATTERY_FULL_PERCENT 100

// ============= DISPLAY SETTINGS =============
#define DISPLAY_WIDTH 480
#define DISPLAY_HEIGHT 320
#define TOUCH_CALIBRATION_REQUIRED false

// ============= BUILD INFO =============
#define VERSION "2.0.0-beta"
#define BUILD_DATE "2026-09-27"
#define HARDWARE_REVISION "ESP32-S3-N16R8"

#endif // CONFIG_H
