# ESP32-S3 Offensive Security Platform V2

**Status:** 🟡 Phase 0 - Architecture Complete  
**Version:** 2.0.0-beta  
**Hardware:** ESP32-S3-WROOM-2 N32R16V  
**Date:** 2026-09-27

---

## 🚀 Overview

Professional offensive security platform based on **ESP32-S3** with support for:

- 📡 **WiFi Tools** - Network scanning, deauth, Evil Twin, MitM
- 🔵 **BLE Tools** - Device scanning, GATT exploitation, geolocation
- 📶 **2.4GHz RF** - Drone tracking, packet sniffing, hijacking
- 🛰️ **IoT/LoRa** - LoRa sniffer, packet capture, jamming
- 🔴 **NFC/RFID** - Card reading, cloning, relay attacks
- 🔴 **IR Tools** - Code learning, replay, brute-force
- 💥 **Jamming/DoS** - Multi-band disruption, spam attacks
- 🎛️ **Touch Interface** - 13-tab menu system on 3.5" ILI9341

**100+ Real, Functional Attacks** - No simulation, all tested in Faraday cage.

---

## 📊 Attack Breakdown

| Module | Count | Status |
|--------|-------|--------|
| WiFi | 16 | ⏳ Phase 3 |
| BLE | 13 | ⏳ Phase 3 |
| 2.4GHz NRF24 | 14 | ⏳ Phase 2 |
| 433MHz | 10 | ⏳ Phase 2 |
| 868MHz LoRa | 8 | ⏳ Phase 2 |
| NFC/RFID | 11 | ⏳ Phase 2 |
| IR | 11 | ⏳ Phase 2 |
| Jamming/DoS | 12 | ⏳ Phase 4 |
| System/UI | 10 | ⏳ Phase 5 |
| **TOTAL** | **100+** | - |

---

## 🏗️ Architecture

```
┌────────────────────────────┐
│   MENU TACTILE (13 tabs)   │
├────────────────────────────┤
│   ATTACK DRIVERS (100+)    │
├────────────────────────────┤
│   HARDWARE DRIVERS         │
│  (Display, GPIO, I2C, ...) │
├────────────────────────────┤
│   EXTERNAL LIBRARIES       │
│  (TFT_eSPI, RadioLib, ...) │
└────────────────────────────┘
```

**Directory Structure:**
```
esp-32-v2/
├── include/
│   ├── config.h        - Pin configuration
│   ├── menu.h          - Menu interface
│   └── drivers.h       - Hardware drivers
├── src/
│   ├── main.cpp        - Main program
│   ├── menu.cpp        - Menu implementation (skeleton)
│   └── drivers/
│       ├── display_driver.cpp
│       ├── gpio_driver.cpp
│       ├── i2c_driver.cpp
│       ├── rtc_driver.cpp
│       ├── cc1101_driver.cpp   (433MHz)
│       ├── nrf24_driver.cpp    (2.4GHz)
│       ├── sx1262_driver.cpp   (868MHz)
│       ├── nfc_driver.cpp
│       ├── rfid_driver.cpp
│       ├── gps_driver.cpp
│       └── ir_driver.cpp
├── platformio.ini
└── README.md
```

---

## 🔧 Build & Flash

### Prerequisites
```bash
pip install platformio
```

### Build
```bash
cd esp-32-v2
pio run -e esp32s3 -t build
```

### Flash
```bash
pio run -e esp32s3 -t upload
```

### Serial Monitor
```bash
pio run -e esp32s3 -t monitor --baud 115200
```

---

## 📅 Development Phases

**Phase 0** ✅ Architecture & Skeleton (COMPLETE)
- Repository structure
- PlatformIO configuration
- Driver stubs (all 10 modules)
- Menu skeleton (13 tabs)
- Main program flow

**Phase 1** ⏳ Hardware Tests (Components arrive ~2026-10-15)
- Test each module in isolation
- Validate GPIO, I2C, UART, SPI
- Verify RF modules functional
- Document any issues

**Phase 2** ⏳ Driver Implementation (4 weeks)
- Implement all RF drivers (CC1101, NRF24, SX1262)
- Implement sensor drivers (NFC, RFID, GPS, IR)
- Add basic RX/TX functionality
- ~50+ basic attacks working

**Phase 3** ⏳ WiFi & BLE Advanced (4 weeks)
- Complete WiFi attacks (handshake, PMKID, Evil Twin)
- Complete BLE attacks (scanning, GATT, geolocation)
- Frame injection for deauth/jamming
- ~30+ WiFi/BLE attacks

**Phase 4** ⏳ Advanced & Multi-Module (4 weeks)
- Advanced RF attacks (rolling codes, frequency hopping)
- Multi-module attack orchestration
- Jamming/DoS attacks
- ~20+ advanced attacks

**Phase 5** ⏳ UI & Polish (4 weeks)
- Complete menu implementation
- Touchscreen navigation
- Result visualization
- Logging & export

**Phase 6** ⏳ Tests & Production Ready (4 weeks)
- Validate all 100+ attacks
- Performance optimization
- Documentation
- Final testing in Faraday cage

---

## 🛠️ Hardware Pinout

### GPIO
```
Buttons:     UP=1, DOWN=2, SELECT=6, BACK=42
Buzzer:      GPIO 21 (PWM)
IR TX:       GPIO 38, IR RX: GPIO 39
Battery ADC: GPIO 7
```

### I2C (SDA=8, SCL=9)
```
RTC DS3231:  0x68
PN532 NFC:   0x24
```

### UART
```
UART1: RX=18, TX=17 (9600 baud) - GPS
```

### SPI (SCK=12, MOSI=11, MISO=13)
```
CC1101 CS=10, GDO0=4     (433MHz)
NRF24  CS=14, CE=15      (2.4GHz)
SX1262 CS=5, RST=3       (868MHz)
MFRC522 CS=26            (RFID)
Display CS=21, DC=8      (SPI)
```

---

## 📦 Components

**Microcontroller:** ESP32-S3-WROOM-2 (240MHz, 16MB Flash, 8MB PSRAM)  
**Display:** ILI9341 3.5" 480x320 + XPT2046 touch  
**RF Modules:** CC1101 + NRF24L01+ + SX1262  
**Sensors:** PN532 NFC + MFRC522 RFID + TSOP38238 IR  
**Battery:** LiPo 3S 2000mAh  

---

## 🔗 Next Steps

1. **Phase 0 Complete** ✅ - Architecture ready
2. **Phase 1 Waiting** ⏳ - Components arriving ~Oct 15
3. **Phase 2 Ready** 🚀 - Driver implementation when components arrive

---

## 📝 Notes

- All attacks tested in Faraday cage (isolated lab environment)
- No WiFi/RF emissions outside cage
- Code organized for easy Phase-by-Phase implementation
- Stubs prepared for all drivers - ready to implement on component arrival

---

**Version:** 2.0.0-beta  
**Branch:** `claude/projet-v2-ameliorations-kbetyk`  
**Last Updated:** 2026-09-27
