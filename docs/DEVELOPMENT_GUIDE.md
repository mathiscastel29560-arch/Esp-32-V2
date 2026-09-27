# Guide de Développement - Plateforme de Sécurité Offensive ESP32-S3

## Table des Matières

1. [Architecture du Framework](#architecture-du-framework)
2. [Implémentation d'une Attaque](#implémentation-dune-attaque)
3. [Système de Logging](#système-de-logging)
4. [Menu Avancé](#menu-avancé)
5. [Tests et Benchmarking](#tests-et-benchmarking)
6. [Bonnes Pratiques](#bonnes-pratiques)
7. [Exemples Complets](#exemples-complets)

---

## Architecture du Framework

### Composants Principaux

**1. Attack Framework** (`include/attack_framework.h`)
- Classe de base `Attack` pour tous les modules d'attaque
- Enum `AttackStatus` pour la gestion d'état
- Enum `ResultType` pour catégoriser les résultats
- Classe `ResultBuilder` pour créer des résultats normalisés

**2. Logging System** (`include/logging_system.h`)
- `Logger` : Enregistrement centralisé avec niveaux (DEBUG, INFO, WARN, ERROR)
- `ResultsFormatter` : Formatage professionnel des résultats
- `ResultsExporter` : Export CSV/JSON des résultats et logs
- `StatsTracker` : Suivi des statistiques d'attaque

**3. Menu Advanced** (`include/menu_advanced.h`)
- Navigation tactile et par boutons
- Rendu dynamique des éléments de menu
- Système de notifications (Toast)
- Dialogues de saisie (InputDialog)

**4. Testing Framework** (`include/testing_framework.h`)
- Suites de test pour chaque module
- Mock drivers pour tests isolés
- Benchmarking et profiling mémoire
- Assertions pour vérification d'état

---

## Implémentation d'une Attaque

### Structure de Base

```cpp
#include "attack_framework.h"
#include "logging_system.h"

// Créer une classe dérivée de Attack
class WiFiNetworkScan : public Attack {
public:
  WiFiNetworkScan() : Attack("WiFi Network Scan") {
    // Initialiser les paramètres spécifiques
  }
  
  bool begin() override {
    // Initialiser le matériel WiFi
    Logger::getInstance().info("WiFi", "Scanner initialized");
    return true;
  }
  
  bool start() override {
    Attack::start(); // Appeler la classe de base
    startTime = millis();
    Logger::getInstance().logAttackStart(getName(), "Local networks");
    return true;
  }
  
  void update() override {
    if (!isRunning) return;
    
    // Effectuer une itération du scan
    // Créer des résultats via ResultBuilder
    
    if (/* scan terminé */) {
      setStatus(AttackStatus::SUCCESS);
    }
  }
  
  bool stop() override {
    Attack::stop();
    Logger::getInstance().logAttackEnd(getName(), currentStatus);
    return true;
  }
  
  bool cleanup() override {
    clearResults();
    return true;
  }
};
```

### Ajouter des Résultats

```cpp
void WiFiNetworkScan::update() {
  // Créer un résultat de scan
  AttackResult* result = ResultBuilder::createScan(
    "MyNetwork-5G",  // Description
    -45              // RSSI en dBm
  );
  
  addResult(result); // Ajouter au vecteur de résultats
}

void WiFiNetworkScan::captureHandshake() {
  // Créer un résultat de paquet
  uint8_t packetData[] = {/* données */};
  AttackResult* result = ResultBuilder::createPacket(
    packetData,
    sizeof(packetData),
    "WPA2 Handshake"
  );
  
  addResult(result);
}
```

### Gestion d'État

```cpp
void Attack::setStatus(AttackStatus status) {
  currentStatus = status;
  
  // Logger le changement d'état
  Logger& logger = Logger::getInstance();
  const char* statusStr = "";
  
  switch (status) {
    case AttackStatus::SCANNING:
      statusStr = "Scanning...";
      break;
    case AttackStatus::ATTACKING:
      statusStr = "Attack in progress";
      break;
    case AttackStatus::SUCCESS:
      statusStr = "Attack successful";
      break;
    case AttackStatus::FAILED:
      statusStr = "Attack failed";
      break;
    case AttackStatus::ERROR:
      statusStr = "Error occurred";
      break;
  }
  
  logger.info(getName(), statusStr);
}
```

---

## Système de Logging

### Utilisation de Base

```cpp
Logger& logger = Logger::getInstance();

// Messages simples
logger.debug("Module", "Detailed diagnostic info");
logger.info("Module", "Normal operation message");
logger.warn("Module", "Warning: degraded state");
logger.error("Module", "Critical failure");

// Logging d'attaque
logger.logAttackStart("WiFi Deauth", "Target: BSSID");
logger.logAttackEnd("WiFi Deauth", AttackStatus::SUCCESS);

// Logging de résultats
AttackResult* result = /* ... */;
logger.logResult("WiFi", result);
```

### Export de Résultats

```cpp
ResultsExporter& exporter = ResultsExporter::getInstance();
Attack* myAttack = /* ... */;

char csvOutput[4096];
if (exporter.exportToCSV(myAttack, csvOutput, sizeof(csvOutput))) {
  Serial.println(csvOutput);
  // Envoyer à UART/SD Card/WiFi
}

char jsonOutput[4096];
if (exporter.exportToJSON(myAttack, jsonOutput, sizeof(jsonOutput))) {
  Serial.println(jsonOutput);
}
```

### Statistiques d'Attaque

```cpp
StatsTracker& stats = StatsTracker::getInstance();

// Enregistrer une exécution
stats.recordAttack("WiFi Deauth", AttackStatus::SUCCESS, 2500, -65);

// Récupérer les statistiques
AttackStats* stat = stats.getStats("WiFi Deauth");
if (stat) {
  Serial.printf("Runs: %u, Success: %u, Failures: %u\n",
    stat->runCount, stat->successCount, stat->failureCount);
}
```

---

## Menu Avancé

### Ajouter des Éléments de Menu

```cpp
MenuAdvanced menu;
menu.begin();

// Créer des callbacks pour les actions
void onWiFiScanClicked() {
  MenuAdvanced::showStatusMessage("Starting WiFi scan...");
  wifiScanner.start();
}

// Ajouter des éléments
MenuItem wifiScan("WiFi Scan", "Network scanner", true, onWiFiScanClicked);
menu.addMenuItem(wifiScan);

MenuItem wifiDeauth("WiFi Deauth", "Deauth attacker", true, onWiFiDeauthClicked);
menu.addMenuItem(wifiDeauth);
```

### Gestion de l'Entrée Utilisateur

```cpp
// Saisie de texte
char ssid[32];
if (menu.promptForInput("Enter SSID:", ssid, 32)) {
  // Utiliser la valeur saisie
  Serial.println(ssid);
}

// Saisie numérique
uint32_t channel = 0;
if (menu.promptForNumber("Channel (1-13):", channel, 1, 13)) {
  scanner.setChannel(channel);
}

// Dialogue de confirmation
if (menu.confirmDialog("Start deauth attack on this network?")) {
  deauthAttack.start();
}
```

### Notifications

```cpp
// Toast de succès (2 secondes, vert)
MenuAdvanced::showSuccessMessage("Handshake captured!");

// Toast d'erreur (3 secondes, rouge)
MenuAdvanced::showErrorMessage("Failed to connect to device");

// Message personnalisé (2 secondes, blanc)
MenuAdvanced::showStatusMessage("Processing results...", 2000);
```

---

## Tests et Benchmarking

### Créer une Suite de Tests

```cpp
class MyAttackTestSuite : public TestSuite {
public:
  MyAttackTestSuite() : TestSuite("My Attack Tests") {
    addTest("Initialization", testInit);
    addTest("Scanning", testScanning);
    addTest("Result Collection", testResults);
  }
  
private:
  static bool testInit() {
    MyAttack attack;
    ASSERT_TRUE(attack.begin());
    ASSERT_TRUE(attack.start());
    return true;
  }
  
  static bool testScanning() {
    MyAttack attack;
    attack.start();
    delay(100);
    attack.update();
    ASSERT_TRUE(attack.getResultCount() > 0);
    return true;
  }
  
  static bool testResults() {
    MyAttack attack;
    attack.generateTestResults(5);
    ASSERT_EQUAL(attack.getResultCount(), 5);
    return true;
  }
};
```

### Exécuter les Tests

```cpp
void setup() {
  Serial.begin(115200);
  delay(1000);
  
  // Exécuter la suite complète
  MyAttackTestSuite suite;
  suite.runAll();
  
  // Ou exécuter un seul test
  suite.runTest(0);
}
```

### Benchmarking

```cpp
void loop() {
  PerformanceBenchmark& bench = PerformanceBenchmark::getInstance();
  
  WiFiDeauth attack;
  attack.begin();
  
  // Mesurer start()
  bench.startMeasure("WiFi Deauth Start");
  attack.start();
  bench.stopMeasure();
  
  // Mesurer update()
  for (int i = 0; i < 10; i++) {
    bench.startMeasure("WiFi Deauth Update");
    attack.update();
    bench.stopMeasure();
    delay(100);
  }
  
  attack.stop();
  
  // Afficher les résultats
  bench.printAllResults();
}
```

### Profiling Mémoire

```cpp
void loop() {
  MemoryProfiler& profiler = MemoryProfiler::getInstance();
  
  Serial.println("Memory before attack:");
  profiler.captureSnapshot("Before");
  
  WiFiDeauth attack;
  attack.start();
  for (int i = 0; i < 50; i++) {
    attack.update();
    delay(50);
  }
  attack.stop();
  
  Serial.println("Memory after attack:");
  profiler.captureSnapshot("After");
  
  profiler.printReport();
}
```

---

## Bonnes Pratiques

### 1. Gestion de la Mémoire

```cpp
// ✓ BON: Utiliser des structures compactes
struct CompactResult {
  int8_t rssi;
  uint16_t dataLen;
  uint32_t timestamp;
  // Total: 8 bytes vs 30+ bytes pour AttackResult complète
};

// ✗ MAUVAIS: Allouer trop de mémoire
std::vector<char[256]> descriptions; // Gaspille la mémoire
```

### 2. Gestion d'Énergie

```cpp
// ✓ BON: Vérifier avant de scanner
if (/* battery > 20% */) {
  attack.start();
} else {
  logger.warn("Attack", "Battery too low");
}

// ✗ MAUVAIS: Ignorer le niveau de batterie
attack.start(); // Peut arrêter soudainement le matériel
```

### 3. Sécurité des Threads

```cpp
// ✓ BON: Créer une attaque par utilisateur
class AttackManager {
  std::vector<Attack*> activeAttacks;
  void addAttack(Attack* a) { activeAttacks.push_back(a); }
};

// ✗ MAUVAIS: Attaques statiques partagées
static WiFiDeauth globalAttack; // Conflit si deux utilisateurs
```

### 4. Gestion d'Erreurs

```cpp
// ✓ BON: Vérifier et logger chaque erreur
bool result = attack.start();
if (!result) {
  logger.error("Attack", "Failed to start attack");
  menuAdvanced.showErrorMessage("Startup failed");
  return;
}

// ✗ MAUVAIS: Ignorer les erreurs
attack.start(); // Continuer même si ça échoue
```

### 5. Logging Approprié

```cpp
// ✓ BON: Logs détaillés pendant le développement
logger.debug("WiFi", "Scanning channel 1");
logger.debug("WiFi", "Found network: SSID=TEST, RSSI=-65");

// ✗ MAUVAIS: Trop de logs en production
logger.debug("WiFi", "Tick 1");
logger.debug("WiFi", "Tick 2");
logger.debug("WiFi", "Tick 3"); // Remplir les logs
```

---

## Exemples Complets

### Exemple 1: Scan WiFi Simple

```cpp
#include "attack_framework.h"
#include "logging_system.h"

class SimpleWiFiScan : public Attack {
public:
  SimpleWiFiScan() : Attack("Simple WiFi Scan") {}
  
  bool begin() override {
    // WiFi init code
    return true;
  }
  
  bool start() override {
    Attack::start();
    Logger::getInstance().logAttackStart(getName());
    
    // Commencer le scan
    // WiFi.mode(WIFI_STA);
    // WiFi.scanNetworks(async=true);
    
    return true;
  }
  
  void update() override {
    if (!isRunning) return;
    
    // int16_t netCount = WiFi.scanComplete();
    // if (netCount == WIFI_SCAN_FAILED) {
    //   setStatus(AttackStatus::FAILED);
    //   return;
    // }
    // if (netCount >= 0) {
    //   for (int i = 0; i < netCount; i++) {
    //     AttackResult* r = ResultBuilder::createScan(
    //       WiFi.SSID(i).c_str(),
    //       WiFi.RSSI(i)
    //     );
    //     addResult(r);
    //   }
    //   WiFi.scanDelete();
    //   setStatus(AttackStatus::SUCCESS);
    //   isRunning = false;
    // }
  }
  
  bool stop() override {
    Attack::stop();
    Logger::getInstance().logAttackEnd(getName(), currentStatus);
    return true;
  }
};

// Utilisation
void loop() {
  SimpleWiFiScan scanner;
  scanner.begin();
  scanner.start();
  
  while (scanner.isActive()) {
    scanner.update();
    delay(100);
  }
  
  // Afficher les résultats
  for (uint16_t i = 0; i < scanner.getResultCount(); i++) {
    AttackResult* result = scanner.getResult(i);
    Serial.println(ResultsFormatter::getInstance().format(result));
  }
}
```

### Exemple 2: Attaque avec Paramètres

```cpp
class ParametrizedDeauth : public Attack {
private:
  char targetBSSID[18];
  uint8_t channel;
  uint16_t packets;
  
public:
  ParametrizedDeauth() : Attack("Deauth Attack"), channel(1), packets(100) {
    memset(targetBSSID, 0, 18);
  }
  
  bool setParameter(const char* key, const char* value) override {
    if (strcmp(key, "bssid") == 0) {
      strncpy(targetBSSID, value, 17);
      return true;
    }
    if (strcmp(key, "channel") == 0) {
      channel = atoi(value);
      return true;
    }
    if (strcmp(key, "packets") == 0) {
      packets = atoi(value);
      return true;
    }
    return false;
  }
  
  const char* getParameter(const char* key) override {
    if (strcmp(key, "bssid") == 0) return targetBSSID;
    if (strcmp(key, "channel") == 0) {
      static char buf[4];
      itoa(channel, buf, 10);
      return buf;
    }
    return nullptr;
  }
  
  bool start() override {
    Attack::start();
    char msg[128];
    snprintf(msg, 127, "Deauth on %s, channel %u, %u packets",
      targetBSSID, channel, packets);
    Logger::getInstance().logAttackStart(getName(), msg);
    return true;
  }
};

// Utilisation
void configureAndRun() {
  ParametrizedDeauth deauth;
  deauth.setParameter("bssid", "AA:BB:CC:DD:EE:FF");
  deauth.setParameter("channel", "6");
  deauth.setParameter("packets", "50");
  
  deauth.begin();
  deauth.start();
  // ...
}
```

### Exemple 3: Test d'Attaque

```cpp
class DeauthTestSuite : public TestSuite {
public:
  DeauthTestSuite() : TestSuite("Deauth Tests") {
    addTest("Parameter Setting", testParameterSetting);
    addTest("Attack Execution", testExecution);
    addTest("Result Generation", testResultGeneration);
  }
  
private:
  static bool testParameterSetting() {
    ParametrizedDeauth deauth;
    bool result = deauth.setParameter("bssid", "AA:BB:CC:DD:EE:FF");
    ASSERT_TRUE(result);
    
    const char* bssid = deauth.getParameter("bssid");
    ASSERT_NOT_NULL(bssid);
    
    return true;
  }
  
  static bool testExecution() {
    ParametrizedDeauth deauth;
    deauth.begin();
    ASSERT_TRUE(deauth.start());
    ASSERT_TRUE(deauth.isActive());
    ASSERT_TRUE(deauth.stop());
    ASSERT_FALSE(deauth.isActive());
    
    return true;
  }
  
  static bool testResultGeneration() {
    MockAttack attack("Deauth Mock");
    attack.generateTestResults(20);
    ASSERT_EQUAL(attack.getResultCount(), 20);
    
    return true;
  }
};
```

---

## Checklist de Déploiement

- [ ] Attaque implémente tous les callbacks obligatoires
- [ ] Logging approprié à tous les points critiques
- [ ] Gestion des erreurs complète
- [ ] Nettoyage de ressources dans `cleanup()`
- [ ] Tests unitaires passent
- [ ] Benchmarks acceptables (< 100ms par update)
- [ ] Pas de fuites mémoire détectées
- [ ] Documentation des paramètres
- [ ] Menu intégré avec icône unique
- [ ] Résultats formatés professionnellement

---

**Version:** 2.0.0-beta  
**Date:** 2026-09-27  
**Auteur:** Claude Development Team
