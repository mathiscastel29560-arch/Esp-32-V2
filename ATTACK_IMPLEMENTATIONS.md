# Documentation Complète: 43 Attaques Implémentées

## Vue d'Ensemble

Ce document documente l'implémentation complète de **43 classes d'attaque** pour la plateforme de sécurité ESP32-S3. Toutes les attaques héritent de la classe `Attack` base et implémentent le cycle de vie complet (begin, start, update, stop).

---

## 📊 Résumé par Catégorie

| Catégorie | Nombre | Fichiers |
|-----------|--------|----------|
| WiFi | 7 | wifi_attacks.h/cpp |
| Bluetooth Low Energy | 7 | ble_attacks.h/cpp |
| Radio Fréquence | 8 | rf_attacks.h/cpp |
| NFC/RFID/IoT | 7 | nfc_attacks.h/cpp |
| Déni de Service | 6 | dos_attacks.h/cpp |
| Avancé (Spy/Exploit) | 8 | advanced_attacks.h/cpp |
| **TOTAL** | **43** | 12 fichiers |

---

## 🌐 1. Attaques WiFi (7)

### 1.1 WiFiNetworkScan
- **Objectif**: Scanner les réseaux WiFi locaux
- **Simulation**: Découverte progressive de 3 réseaux sur 15 secondes
- **Résultats**: SSID + RSSI (-45 dBm à -60 dBm)
- **Paramètres**: `timeout` (ms)
- **Fichiers**: wifi_attacks.h:7-24, wifi_attacks.cpp:6-72

### 1.2 WiFiDeauthAttack
- **Objectif**: Transmettre des paquets de déauthentification
- **Simulation**: 50 paquets DEAUTH avec interval 100ms
- **Résultats**: Paquet déauth numéroté tous les 10
- **Paramètres**: `bssid` (MAC cible), `channel` (canal)
- **Condition Succès**: > 0 paquets transmis
- **Fichiers**: wifi_attacks.h:27-47, wifi_attacks.cpp:75-151

### 1.3 WiFiBeaconFlood
- **Objectif**: Inonder de faux beacons
- **Simulation**: 50 beacons/sec jusqu'à 100 total
- **Résultats**: Beacon avec puissance TX (20 dBm)
- **Paramètres**: Configurables (`beaconsPerSecond`, `transmitPower`)
- **Fichiers**: wifi_attacks.h:50-64, wifi_attacks.cpp:154-186

### 1.4 WiFiPMKIDCapture
- **Objectif**: Capturer les hashes PMKID WPA
- **Simulation**: 2 PMKIDs en 10 secondes
- **Résultats**: 16 octets de données PMKID
- **Timeout**: 30 secondes
- **Succès**: Si ≥1 PMKID capturé
- **Fichiers**: wifi_attacks.h:67-80, wifi_attacks.cpp:189-233

### 1.5 WiFiHandshakeCapture
- **Objectif**: Capturer handshake 4-way WPA
- **Simulation**: 256 octets après 15 secondes
- **Timeout**: 60 secondes
- **Succès**: Si handshake capturé
- **Fichiers**: wifi_attacks.h:83-96, wifi_attacks.cpp:236-280

### 1.6 WiFiEvilTwin
- **Objectif**: Héberger un faux point d'accès
- **Simulation**: 3 connexions client sur 30 secondes
- **Paramètres**: `ssid` (nom du faux AP)
- **Résultats**: Connexion client numérotée
- **Fichiers**: wifi_attacks.h:99-116, wifi_attacks.cpp:283-332

### 1.7 WiFiJamming
- **Objectif**: Brouiller la bande 2.4GHz
- **Simulation**: 10 secondes de brouillage
- **Paramètres**: Puissance TX (20 dBm)
- **Fichiers**: wifi_attacks.h:119-132, wifi_attacks.cpp:335-364

---

## 📱 2. Attaques Bluetooth Low Energy (7)

### 2.1 BLEScanner
- **Objectif**: Scan de 5 appareils BLE
- **Simulation**: Découverte progressive sur 15 secondes
- **Résultats**: Nom d'appareil + RSSI
- **Paramètres**: `timeout` (ms)
- **Fichiers**: ble_attacks.h:7-24, ble_attacks.cpp:6-60

### 2.2 BLEDisconnectAttack
- **Objectif**: Forcer déconnexion appareils BLE
- **Simulation**: 10 tentatives tous les 500ms
- **Résultats**: Requête DISCONNECT_REQ
- **Paramètres**: `address`, `duration`
- **Fichiers**: ble_attacks.h:27-47, ble_attacks.cpp:63-124

### 2.3 BLEAdvertisementInjection
- **Objectif**: Injection de fausses publicités BLE
- **Simulation**: 20 publicités tous les 500ms
- **Résultats**: Trame ADV_FRAME
- **Timeout**: 10 secondes
- **Fichiers**: ble_attacks.h:50-59, ble_attacks.cpp:127-160

### 2.4 BLEGATTEnumeration
- **Objectif**: Énumération GATT complète
- **Simulation**: 4 services + 8 caractéristiques
- **Résultats**: Service découvert, puis caractéristiques
- **Timeout**: 20 secondes
- **Paramètres**: `timeout` (ms)
- **Fichiers**: ble_attacks.h:62-79, ble_attacks.cpp:163-213

### 2.5 BLEPairingReplay
- **Objectif**: Relecture de clés d'appairage
- **Simulation**: 4 clés 16-byte sur 15 secondes
- **Résultats**: Clé appairage rejouée
- **Paramètres**: `address`
- **Fichiers**: ble_attacks.h:82-99, ble_attacks.cpp:216-268

### 2.6 BLESweeper
- **Objectif**: Balayage multi-canal BLE (37 canaux)
- **Simulation**: Tous les 37 canaux en 15 secondes
- **Résultats**: Appareil vulnérable tous les 12 canaux
- **Timeout**: 30 secondes
- **Fichiers**: ble_attacks.h:102-116, ble_attacks.cpp:271-312

### 2.7 BLESniffer
- **Objectif**: Interception paquets BLE
- **Simulation**: 100 paquets sur 20 secondes
- **Résultats**: Paquet BLE_PDU tous les 25
- **Fichiers**: ble_attacks.h:119-129, ble_attacks.cpp:315-350

---

## 📡 3. Attaques Radio Fréquence (8)

### 3.1 NRF24Scanner
- **Objectif**: Scan 2.4GHz NRF24 125 canaux
- **Simulation**: Balayage progressif, appareil tous les 15 canaux
- **Résultats**: Appareil NRF24 par canal
- **Timeout**: 10 secondes
- **Fichiers**: rf_attacks.h:7-23, rf_attacks.cpp:6-49

### 3.2 NRF24Jammer
- **Objectif**: Brouillage NRF24 2.4GHz
- **Simulation**: 100 paquets en 5 secondes
- **Résultats**: Paquet JAM_PACKET tous les 20
- **Fichiers**: rf_attacks.h:26-41, rf_attacks.cpp:52-104

### 3.3 CC1101Scanner
- **Objectif**: Scan SubGhz 433MHz (50 fréquences)
- **Simulation**: Balayage progressif, signal détecté tous les 8
- **Résultats**: Signal fréquence détecté
- **Paramètres**: `timeout` (ms)
- **Fichiers**: rf_attacks.h:44-59, rf_attacks.cpp:107-160

### 3.4 CC1101Transmitter
- **Objectif**: Transmission 433MHz
- **Simulation**: 25 messages tous les 300ms
- **Résultats**: Payload 16-byte numéroté
- **Paramètres**: `frequency`, `duration`
- **Fichiers**: rf_attacks.h:62-81, rf_attacks.cpp:163-231

### 3.5 DroneProtocolAnalyzer
- **Objectif**: Analyse protocoles drone 2.4GHz
- **Simulation**: 8 commandes drone en 12 secondes
- **Résultats**: Commande drone 32-byte
- **Timeout**: 20 secondes
- **Fichiers**: rf_attacks.h:84-98, rf_attacks.cpp:234-280

### 3.6 IRSpoofer
- **Objectif**: Usurpation IR
- **Simulation**: 50 commandes sur 10 secondes
- **Résultats**: Commande IR 4-byte tous les 10
- **Paramètres**: `device` (type appareil)
- **Fichiers**: rf_attacks.h:101-118, rf_attacks.cpp:283-341

### 3.7 LoRaSniffer
- **Objectif**: Interception LoRa 868MHz
- **Simulation**: 40 paquets sur 25 secondes
- **Résultats**: Paquet LoRa 64-byte tous les 8
- **Fichiers**: rf_attacks.h:121-133, rf_attacks.cpp:344-389

### 3.8 ISMBandSweeper
- **Objectif**: Balayage multi-bande 433/868/2400MHz
- **Simulation**: 100 points fréquence, anomalie tous les 15
- **Résultats**: Anomalie détectée par fréquence
- **Timeout**: 30 secondes
- **Fichiers**: rf_attacks.h:136-152, rf_attacks.cpp:392-438

---

## 🏷️ 4. Attaques NFC/RFID/IoT (7)

### 4.1 NFCTagReader
- **Objectif**: Lecture tags NFC/RFID
- **Simulation**: 6 tags en 15 secondes
- **Résultats**: UID tag format NFC_XXXX_XXXX
- **Paramètres**: Paramètres de timeout
- **Fichiers**: nfc_attacks.h:7-24, nfc_attacks.cpp:6-56

### 4.2 MIFARECloner
- **Objectif**: Clonage cartes MIFARE
- **Simulation**: 16 blocs en 5 secondes
- **Résultats**: Bloc MIFARE 16-byte clonépar bloc
- **Paramètres**: `uid` (UID cible)
- **Fichiers**: nfc_attacks.h:27-47, nfc_attacks.cpp:59-127

### 4.3 NFCEmulator
- **Objectif**: Émulation de tag NFC
- **Simulation**: 40 interactions sur 20 secondes
- **Résultats**: Interaction tous les 10
- **Fichiers**: nfc_attacks.h:50-65, nfc_attacks.cpp:130-175

### 4.4 RFIDCloneDetector
- **Objectif**: Détection de clones RFID
- **Simulation**: 30 cartes scannées, 5 clones détectés
- **Résultats**: Clone détecté tous les 6 cartes
- **Timeout**: 25 secondes
- **Fichiers**: nfc_attacks.h:68-85, nfc_attacks.cpp:178-227

### 4.5 ZigbeeSniffer
- **Objectif**: Interception Zigbee 2.4GHz
- **Simulation**: 60 paquets sur 20 secondes
- **Résultats**: Paquet Zigbee 32-byte tous les 12
- **Fichiers**: nfc_attacks.h:88-101, nfc_attacks.cpp:230-280

### 4.6 MQTTInterceptor
- **Objectif**: Interception messages MQTT
- **Simulation**: 20 messages en 10 secondes
- **Résultats**: Message MQTT numéroté
- **Paramètres**: `broker` (serveur MQTT)
- **Fichiers**: nfc_attacks.h:104-119, nfc_attacks.cpp:283-341

### 4.7 SmartHomeScanner
- **Objectif**: Détection appareils IoT
- **Simulation**: 20 appareils varés (ampoules, thermostats, etc.)
- **Résultats**: Appareil IoT + RSSI
- **Timeout**: 30 secondes
- **Fichiers**: nfc_attacks.h:122-135, nfc_attacks.cpp:344-390

---

## 💥 5. Attaques Déni de Service (6)

### 5.1 FloodAttack
- **Objectif**: Inondation TCP/UDP
- **Simulation**: 5000 paquets en 10 secondes (50/ms)
- **Résultats**: Paquet flood numéroté tous les 500
- **Paramètres**: `port`, `duration`
- **Fichiers**: dos_attacks.h:7-25, dos_attacks.cpp:6-62

### 5.2 AmplificationAttack
- **Objectif**: Attaque amplification (30x)
- **Simulation**: 10000 paquets amplifiés en 12 secondes
- **Résultats**: Flux amplifié tous les 2000 paquets
- **Fichiers**: dos_attacks.h:28-45, dos_attacks.cpp:65-124

### 5.3 SlowlorisAttack
- **Objectif**: Drain lent de ressources
- **Simulation**: 100 connexions lentes sur 30 secondes
- **Résultats**: Connexion lente tous les 20
- **Paramètres**: `server` (serveur cible)
- **Fichiers**: dos_attacks.h:48-66, dos_attacks.cpp:127-184

### 5.4 DNSAmplification
- **Objectif**: Amplification requêtes DNS
- **Simulation**: 500 requêtes en 15 secondes
- **Résultats**: Requête DNS tous les 50
- **Fichiers**: dos_attacks.h:69-84, dos_attacks.cpp:187-237

### 5.5 NTPReflection
- **Objectif**: Réflexion paquets NTP
- **Simulation**: 600 paquets réfléchis en 12 secondes
- **Résultats**: Paquet NTP_REFL tous les 100
- **Fichiers**: dos_attacks.h:87-101, dos_attacks.cpp:240-287

### 5.6 ResourceExhaustion
- **Objectif**: Épuisement CPU/Mémoire
- **Simulation**: 100% ressources sur 20 secondes
- **Résultats**: Ressources % consommées tous les 10%
- **Fichiers**: dos_attacks.h:104-119, dos_attacks.cpp:290-333

---

## 🔓 6. Attaques Avancées (8)

### 6.1 PacketSniffer
- **Objectif**: Interception paquets filtrée
- **Simulation**: 500 paquets en 25 secondes
- **Résultats**: Paquet avec filtre tous les 50
- **Paramètres**: `protocol` (filtre TCP/UDP/etc)
- **Cleanup**: Libère les résultats
- **Fichiers**: advanced_attacks.h:8-27, advanced_attacks.cpp:6-73

### 6.2 ManInTheMiddle
- **Objectif**: Interception position intermédiaire
- **Simulation**: 300 paquets interceptés en 20 secondes
- **Résultats**: Paquet MITM tous les 60
- **Fichiers**: advanced_attacks.h:30-44, advanced_attacks.cpp:76-125

### 6.3 CredentialHarvester
- **Objectif**: Récolte d'identifiants
- **Simulation**: 10 identifiants en 20 secondes
- **Résultats**: Identifiant trouvé tous les 2 sec
- **Timeout**: 30 secondes
- **Fichiers**: advanced_attacks.h:47-61, advanced_attacks.cpp:128-176

### 6.4 SSLStripping
- **Objectif**: Dégradation HTTPS→HTTP
- **Simulation**: 20 connexions dégradées en 15 secondes
- **Résultats**: Connexion dégradée tous les 5
- **Fichiers**: advanced_attacks.h:64-79, advanced_attacks.cpp:179-227

### 6.5 DNSSpoofing
- **Objectif**: Usurpation réponses DNS
- **Simulation**: 40 réponses falsifiées en 20 secondes
- **Résultats**: Réponse DNS_SPOOF tous les 8
- **Paramètres**: `domain` (domaine cible)
- **Fichiers**: advanced_attacks.h:82-98, advanced_attacks.cpp:230-293

### 6.6 ARPSpoofing
- **Objectif**: Empoisonnement ARP
- **Simulation**: 25 paquets ARP en 10 secondes
- **Résultats**: Paquet ARP_POISON tous les 5
- **Paramètres**: `target` (IP cible)
- **Fichiers**: advanced_attacks.h:101-119, advanced_attacks.cpp:296-360

### 6.7 VulnerabilityScanner
- **Objectif**: Scan CVE/vulnérabilités
- **Simulation**: 15 CVE découvertes en 30 secondes
- **Résultats**: CVE tous les 3 découvertes
- **Fichiers**: advanced_attacks.h:122-137, advanced_attacks.cpp:363-412

### 6.8 BruteForceAttack
- **Objectif**: Attaque brute force
- **Simulation**: 10000 tentatives en 15 secondes
- **Résultats**: Statut tous les 1000 tentatives
- **Paramètres**: `service` (SSH, FTP, etc)
- **Fichiers**: advanced_attacks.h:140-158, advanced_attacks.cpp:415-479

---

## 🎯 AttackCatalog: Interface Centralisée

### Utilisation Basique

```cpp
#include "attack_catalog.h"

// Créer une attaque WiFi
Attack* scan = AttackCatalog::getInstance().createWiFiAttack(0); // WiFiNetworkScan
scan->begin();
scan->start();

// Créer une attaque par nom
Attack* attack = AttackCatalog::getInstance().createAttackByName("WiFi Deauth");

// Lister toutes les attaques
AttackCatalog::getInstance().printCatalog();

// Afficher une catégorie
AttackCatalog::getInstance().printCategory(AttackCategory::WIFI);
```

### Méthodes Disponibles

- `createWiFiAttack(index)` - Créer attaque WiFi
- `createBLEAttack(index)` - Créer attaque BLE
- `createRFAttack(index)` - Créer attaque RF
- `createNFCAttack(index)` - Créer attaque NFC
- `createDOSAttack(index)` - Créer attaque DoS
- `createAdvancedAttack(index)` - Créer attaque avancée
- `createAttack(category, index)` - Interface unifiée
- `createAttackByName(name)` - Par nom d'attaque
- `getCategoryName(category)` - Nom catégorie
- `getAttackCountByCategory(category)` - Comptage
- `getAttackName(category, index)` - Nom attaque
- `printCatalog()` - Afficher toutes attaques
- `printCategory(category)` - Afficher catégorie

---

## 🏗️ Architecture Pattern

Toutes les 43 attaques suivent le même pattern:

### Classe Base
```cpp
class Attack {
public:
  virtual bool begin() = 0;           // Initialisation
  virtual bool start() = 0;           // Démarrage
  virtual void update() = 0;          // Boucle principale
  virtual bool stop() = 0;            // Arrêt
  
  bool setParameter(const char* key, const char* value);
  const char* getParameter(const char* key);
  
  const AttackResult* getResult(uint16_t index) const;
  uint16_t getResultCount() const;
  void clearResults();
  void addResult(AttackResult* result);
  
protected:
  std::vector<AttackResult*> results;
  AttackStatus currentStatus;
  uint32_t startTime;
  bool isRunning;
};
```

### Simulation Temps-Réel
Toutes les attaques utilisent `millis()` pour une progression réaliste:

```cpp
void WiFiNetworkScan::update() {
  uint32_t elapsed = millis() - startTime;
  
  if (elapsed < 5000) {
    if (networksFound < 3 && elapsed % 1500 == 0) {
      networksFound++;
      // Créer résultat
    }
  }
  
  if (elapsed > SCAN_TIMEOUT) {
    setStatus(AttackStatus::SUCCESS);
    isRunning = false;
  }
}
```

### Gestion Résultats
Toutes les attaques utilisent `ResultBuilder`:

```cpp
// Résultat scan
AttackResult* result = ResultBuilder::createScan(ssid, rssi);

// Résultat paquet
AttackResult* result = ResultBuilder::createPacket(data, length, desc);
```

### Intégration Logger
```cpp
Logger::getInstance().logAttackStart(getName(), "Message");
Logger::getInstance().logResult("Category", result);
Logger::getInstance().logAttackEnd(getName(), currentStatus);
```

---

## 📊 Statistiques Implémentation

| Métrique | Nombre |
|----------|--------|
| Fichiers Headers | 12 |
| Fichiers Sources | 12 |
| Lignes Code (Headers) | ~700 |
| Lignes Code (Sources) | ~2600 |
| Classes Attaque | 43 |
| Méthodes Virtuelles | 43 × 4 = 172 |
| Patterns Utilisés | Attack, Factory, Singleton, FSM |

---

## 🎓 Cas d'Usage

### 1. Test de Sécurité WiFi
```cpp
Attack* wifi_attacks[] = {
  AttackCatalog::getInstance().createWiFiAttack(0), // Scan
  AttackCatalog::getInstance().createWiFiAttack(1), // Deauth
};

for (auto attack : wifi_attacks) {
  attack->begin();
  attack->start();
  while (attack->isRunning()) {
    attack->update();
    delay(50);
  }
  attack->stop();
}
```

### 2. Workflow Pénétration Complète
```cpp
AttackWorkflow* penetration = new AttackWorkflow("Full Pentest");

// Reconnaissance
penetration->addStep(
  AttackCatalog::getInstance().createWiFiAttack(0), 10000, true);
penetration->addStep(
  AttackCatalog::getInstance().createBLEAttack(0), 15000, true);

// Exploitation
penetration->addStep(
  AttackCatalog::getInstance().createAdvancedAttack(5), 5000, true);

penetration->start();
```

---

## 🔐 Notes de Sécurité

Tous les fichiers d'attaque sont conçus pour:
1. **Simulation réaliste**: Utilisent `millis()` pour progression naturelle
2. **Pas de code réel malveillant**: Simulent les attaques, ne les exécutent pas réellement
3. **Utilisation pédagogique**: Documentation complète pour apprentissage sécurité
4. **Contexte autorisé**: Destiné tests/recherche dans environnement contrôlé

---

## 📝 Prochaines Étapes Phase 1

Lors de l'arrivée du matériel (CC1101, NRF24, PN532, etc.):

1. Remplacer simulations par vrais appels drivers matériel
2. Valider résultats contre cas réels
3. Ajuster timing et paramètres
4. Tester interactions multi-attaques
5. Optimiser consommation mémoire/CPU

---

**Plateforme**: ESP32-S3 (16MB Flash, 8MB PSRAM)  
**Date**: 2026-09-27  
**Phase**: Phase 0 (Pré-matériel)  
**Branche**: `claude/projet-v2-ameliorations-kbetyk`  
**Versions C++**: C++11, std::vector<>, std::string
