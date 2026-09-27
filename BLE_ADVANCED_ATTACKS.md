# 🔵 BLE Advanced Attacks - Complete Reference

**Bluetooth Low Energy Attack Vectors & Exploitation Methods**

---

## 🎯 BLE ATTACK CATEGORIES

### **1. BLE SPAM / ADVERTISEMENT FLOODING**

```
🌊 BLE ADVERTISEMENT SPAM
├─ Flood target device with fake advertisements
├─ Overwhelm BLE stack
├─ Cause device crash/reboot
├─ Battery drain (continuous reconnection attempts)
└─ Denial of Service
```

**How it works:**
```
1. Generate random BLE advertisements
2. Broadcast with high frequency
3. Target device receives 100s of messages
4. Device tries to process all ads
5. CPU at 100%, battery drains
6. Eventually crashes/reboots
```

**Attack Code Example:**
```cpp
void blSpamAdvertisements() {
  for (int i = 0; i < 1000; i++) {
    BLEAdvertisementData adv;
    adv.setFlags(0x06);
    adv.setName("Spam_" + String(random(10000)));
    adv.setCompleteServices(BLEUUID("180A"));
    
    // Broadcast fake advertisement
    advertise(adv);
    delay(10);  // Rapid fire
  }
}
```

**Impact:**
- ✅ Disables all BLE devices in range
- ✅ Forces reconnection storms
- ✅ Battery drain 50-80% per hour
- ✅ Temporary DoS (30 seconds - hours)

**Mitigation:**
- Filter advertisement sources
- Implement rate limiting
- Disable BLE scanning when not needed
- Use whitelist of known devices

---

### **2. BLE PAIRING ATTACK / HIJACKING**

```
🔐 BLE PAIRING HIJACK
├─ Intercept pairing handshake
├─ Extract Long Term Key (LTK)
├─ Decrypt future traffic
├─ Impersonate device
└─ Full device control
```

**Attack Flow:**
```
1. Monitor pairing process
2. Capture pairing packets
3. Extract cryptographic keys
4. Derive session encryption key
5. Decrypt all future communications
6. Inject commands/data
```

**Vulnerabilities:**
- Lack of perfect forward secrecy
- Weak random number generation
- Implementation bugs in key derivation

---

### **3. BLE GATT INJECTION / MODIFICATION**

```
📝 GATT CHARACTERISTIC MANIPULATION
├─ Write to GATT characteristics
├─ Inject malicious values
├─ Trigger unintended behavior
├─ Cause device malfunction
└─ Data corruption
```

**Attack Targets:**
```
Service: Battery Service (180F)
  Characteristic: Battery Level (2A19)
  Attack: Write 0% → Device thinks battery dead
  
Service: Generic Access (1800)
  Characteristic: Device Name (2A00)
  Attack: Write hostile name → Confusion
  
Service: Heart Rate (180D)
  Characteristic: Heart Rate Control Point (2A39)
  Attack: Write invalid commands → Crash

Service: Cycling Power (1818)
  Characteristic: Cycling Power Control Point (2A5C)
  Attack: Write malicious power settings → Safety risk
```

---

### **4. BLE SERVICE CLONING / IMPERSONATION**

```
🎭 FAKE BLE SERVICE CREATION
├─ Create fake GATT services
├─ Emulate real device
├─ Intercept client connections
├─ Steal credentials
└─ Send malicious data
```

**Attack Example:**
```cpp
// Fake Fitbit device
BLEServer* fakeServer = BLEDevice::createServer();

// Create fake Battery Service
BLEService* batteryService = fakeServer->createService("180F");
BLECharacteristic* batteryLevel = 
  batteryService->createCharacteristic("2A19", GATT_READ);
batteryLevel->setValue(75);  // Fake 75% battery

// Create fake Device Information
BLEService* deviceInfo = fakeServer->createService("180A");
BLECharacteristic* manufacturer = 
  deviceInfo->createCharacteristic("2A29", GATT_READ);
manufacturer->setValue("Fitbit Inc.");  // FAKE!

// Advertise as legitimate device
advertise->setName("Fitbit Charge 5");
advertise->addServiceUUID(batteryService->getUUID());
```

**Attacks:**
- ✅ Credential harvesting
- ✅ Malware distribution
- ✅ Command & control
- ✅ Data exfiltration

---

### **5. BLE MITM (MAN-IN-THE-MIDDLE)**

```
🕵️ BLUETOOTH MIDDLE POSITION
├─ Position between device & app
├─ Capture encryption keys
├─ Decrypt traffic
├─ Modify packets
└─ Inject malicious commands
```

**Attack Setup:**
```
Real Device ←→ Attacker ←→ Mobile App
                  (MITM)
```

**What Attacker Can Do:**
1. Read all sensor data (heart rate, GPS, etc.)
2. Modify sensor readings (fake fitness data)
3. Send commands to device (lock/unlock, control)
4. Extract credentials/secrets
5. Inject malware
6. Intercept notifications

---

### **6. BLE FUZZING**

```
🎲 RANDOM PAYLOAD INJECTION
├─ Send malformed packets
├─ Invalid characteristic writes
├─ Oversized payloads
├─ Out-of-range values
└─ Trigger vulnerabilities
```

**Fuzz Targets:**
```cpp
// Fuzz battery characteristic
for (int i = 0; i < 1000; i++) {
  uint8_t fuzzValue = random(256);
  batteryLevel->setValue(fuzzValue);  // Invalid: >100%
}

// Fuzz with extreme values
writeCharacteristic(0xFFFFFFFF);      // Max uint32
writeCharacteristic(-1);              // Negative
writeCharacteristic(3.14e308);        // Float overflow
```

**Results:**
- ✅ Buffer overflows
- ✅ Integer overflows
- ✅ Crashes
- ✅ Privilege escalation

---

### **7. BLE NOTIFICATION HIJACKING**

```
🔔 NOTIFICATION INTERCEPTION
├─ Subscribe to notifications
├─ Capture sensor data
├─ Inject fake notifications
├─ Trigger device actions
└─ Cause unintended behavior
```

**Real Scenario:**
```
Medical Alert Wearable:
  Normal Flow: 
    Wearable → Notification → App: "High Heart Rate!"
  
  Attack Flow:
    Attacker → Hijack → App: "EMERGENCY - CALL 911!"
    Result: Victim calls ambulance unnecessarily
```

---

### **8. BLE BONDING ATTACK**

```
🔗 SECURE CONNECTION EXPLOITATION
├─ Exploit pairing procedures
├─ Extract Temporary Keys (TK)
├─ Brute force PIN
├─ Bypass authentication
└─ Gain trusted access
```

**PIN Brute Force:**
```cpp
// 6-digit PIN
for (int pin = 0; pin < 1000000; pin++) {
  if (attemptPairing(pin)) {
    Serial.println("PIN found: " + String(pin));
    // Pairing successful!
    break;
  }
}
```

**Success Rate:**
- 4-digit PIN: 30 seconds
- 6-digit PIN: 2-3 hours
- No rate limiting: Instant

---

### **9. BLE RESOURCE EXHAUSTION**

```
💥 DEVICE RESOURCE DRAIN
├─ Create unlimited GATT connections
├─ Consume all handles
├─ Exhaust memory
├─ Device becomes unresponsive
└─ Permanent DoS
```

**Attack:**
```cpp
// Create 1000 connections
for (int i = 0; i < 1000; i++) {
  BLEClient* client = BLEDevice::createClient();
  if (!client->connect(target)) break;
  
  // Device runs out of connection slots
  // New connections fail
}
```

---

### **10. BLE REPLAY ATTACK**

```
⏮️ CAPTURED COMMAND REPLAY
├─ Capture legitimate command
├─ Replay without understanding
├─ Device executes again
├─ Repetitive attack
└─ Unintended consequences
```

**Medical Example:**
```
Captured Packet: "DELIVER_INSULIN 10_UNITS"
├─ Record legitimate packet
├─ Replay 10 times
├─ Patient receives 100 units
├─ Life-threatening overdose
└─ FATAL
```

---

### **11. BLE PRIVACY ATTACK**

```
🕵️ DEVICE TRACKING
├─ Monitor MAC addresses
├─ Track movement patterns
├─ Create location profiles
├─ Identify individuals
└─ GDPR violation
```

**Attack:**
```
Home ← [Device Seen] → Work ← [Device Seen] → Gym
       9:00 AM              5:00 PM           7:00 PM
       
Attacker can:
1. Know where person lives
2. Know where person works
3. Know their schedule
4. Know leisure activities
5. Predict future locations
```

---

### **12. BLE DENIAL OF SERVICE (GATT LEVEL)**

```
🔒 GATT DESCRIPTOR ATTACK
├─ Modify Client Characteristic Config Descriptor (CCCD)
├─ Disable notifications
├─ Disable indications
├─ Device loses all alerts
└─ Safety critical alerts disabled
```

**Medical Critical:**
```
Pacemaker Alert Service:
  Notification: "Low Battery 5%"
  CCCD Write: Disable notification
  Result: Patient unaware of battery death
          Pacemaker stops → CARDIAC ARREST
```

---

### **13. BLE BLUETOOTH CLASSIC FALLBACK**

```
📱 FORCED DOWNGRADE
├─ Force device to classic Bluetooth
├─ Weaker security
├─ Easier to crack
├─ Establish control
└─ Compromise device
```

---

### **14. BLE PHYSICAL LAYER ATTACK**

```
📡 SIGNAL SPOOFING
├─ Transmit stronger signal
├─ Overwhelm legitimate device
├─ Device connects to attacker instead
├─ Intercept all traffic
└─ Complete compromise
```

---

## 🛡️ DEFENSE STRATEGIES

### **For Developers:**
```
1. Implement ECDH (Elliptic Curve Diffie-Hellman)
2. Use secure random number generation
3. Implement rate limiting
4. Validate all GATT writes
5. Use authenticated encryption
6. Implement reconnection logic
7. Monitor battery drain anomalies
8. Log all connection attempts
9. Implement connection whitelisting
10. Use firmware signing
```

### **For Users:**
```
1. Turn off Bluetooth when not needed
2. Use non-descriptive device names
3. Keep firmware updated
4. Disable old devices after use
5. Monitor battery drain
6. Use strong PIN/pairing
7. Review paired device list
8. Disable unused services
9. Use secure apps/phones
10. Report suspicious behavior
```

---

## ⚠️ LEGAL & ETHICAL

**These attacks should ONLY be used:**
- ✅ On authorized devices you own
- ✅ In authorized security testing
- ✅ With explicit written permission
- ✅ For academic/research purposes
- ✅ To improve security

**Penalties for unauthorized use:**
- ❌ Criminal charges
- ❌ Civil lawsuits
- ❌ Fines up to $100,000
- ❌ Prison time (up to 10 years)
- ❌ Seizure of equipment

---

## 📊 SUMMARY

**Total BLE Attack Vectors: 14**

```
Category 1: Denial of Service
  ├─ BLE Spam/Flooding
  ├─ Resource Exhaustion
  └─ GATT DoS

Category 2: Authentication Bypass
  ├─ Pairing Hijacking
  ├─ Bonding Attack
  └─ Bluetooth Classic Fallback

Category 3: Data Interception
  ├─ MITM Attack
  ├─ Replay Attack
  └─ Privacy/Tracking

Category 4: Device Manipulation
  ├─ GATT Injection
  ├─ Service Cloning
  ├─ Notification Hijacking
  └─ Fuzzing

Category 5: Physical Layer
  └─ Signal Spoofing
```

---

**Document Version:** 1.0  
**Last Updated:** 2026-09-27  
**Status:** Complete Reference  

🔵 **BLE Attacks: Comprehensive Coverage** 🔵
