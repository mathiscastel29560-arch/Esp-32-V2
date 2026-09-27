# Exemples d'Utilisation des 43 Attaques

## Table des Matières

1. [Exemples Basiques](#exemples-basiques)
2. [Workflows Avancés](#workflows-avancés)
3. [Intégration avec le Système](#intégration-avec-le-système)
4. [Bonnes Pratiques](#bonnes-pratiques)

---

## Exemples Basiques

### Lancer une Attaque WiFi Simple

```cpp
#include "attack_catalog.h"
#include "logging_system.h"

void example_wifi_scan() {
  // Créer instance d'attaque
  Attack* scan = AttackCatalog::getInstance()
    .createAttackByName("WiFi Network Scan");
  
  if (!scan) {
    Serial.println("Erreur: Attaque non trouvée");
    return;
  }

  // Cycle de vie
  scan->begin();
  scan->start();

  // Boucle mise à jour (émule loop Arduino)
  while (scan->isRunning()) {
    scan->update();
    delay(50); // Mise à jour toutes les 50ms
  }

  scan->stop();

  // Afficher résultats
  Serial.printf("Trouvé %u réseaux\n", scan->getResultCount());
  for (uint16_t i = 0; i < scan->getResultCount(); i++) {
    const AttackResult* result = scan->getResult(i);
    Serial.printf("  [%u] %s (RSSI: %d dBm)\n", 
                  i + 1, result->data, result->rssi);
  }

  delete scan;
}
```

### Lancer une Attaque BLE avec Paramètres

```cpp
void example_ble_disconnect() {
  Attack* attack = AttackCatalog::getInstance()
    .createAttackByName("BLE Disconnect");

  // Configurer paramètres
  attack->setParameter("address", "00:11:22:33:44:55");
  attack->setParameter("duration", "5000");

  // Exécuter
  attack->begin();
  attack->start();

  uint32_t start = millis();
  while (attack->isRunning() && (millis() - start) < 6000) {
    attack->update();
    delay(50);
  }

  attack->stop();

  Serial.println("Attaque BLE terminée");
  delete attack;
}
```

### Afficher le Catalogue

```cpp
void example_print_catalog() {
  // Afficher toutes les attaques
  AttackCatalog::getInstance().printCatalog();
  
  Serial.println("\n=== Détail WiFi ===");
  AttackCatalog::getInstance()
    .printCategory(AttackCategory::WIFI);
    
  Serial.println("\n=== Détail BLE ===");
  AttackCatalog::getInstance()
    .printCategory(AttackCategory::BLE);
}
```

### Itérer sur une Catégorie

```cpp
void example_enumerate_category() {
  AttackCatalog& catalog = AttackCatalog::getInstance();
  AttackCategory category = AttackCategory::WIFI;
  
  uint8_t count = catalog.getAttackCountByCategory(category);
  Serial.printf("Catégorie: %s (%u attaques)\n", 
                catalog.getCategoryName(category), count);

  for (uint8_t i = 0; i < count; i++) {
    const char* name = catalog.getAttackName(category, i);
    Serial.printf("  [%u] %s\n", i + 1, name);

    // Créer et tester brièvement
    Attack* attack = catalog.createAttack(category, i);
    if (attack) {
      attack->begin();
      Serial.printf("      ✓ Initialisée avec succès\n");
      delete attack;
    }
  }
}
```

---

## Workflows Avancés

### Workflow: Scan Complet Toutes Bandes

```cpp
class MultiFrequencyScan : public AttackWorkflow {
public:
  MultiFrequencyScan() : AttackWorkflow("Multi-Fréquence") {}

  void setupAttacks() {
    AttackCatalog& catalog = AttackCatalog::getInstance();

    // Bande 2.4GHz
    addStep(catalog.createAttackByName("WiFi Network Scan"), 15000, true);
    addStep(catalog.createAttackByName("BLE Scanner"), 15000, true);
    addStep(catalog.createAttackByName("NRF24 Scanner"), 10000, true);

    // SubGhz
    addStep(catalog.createAttackByName("CC1101 Scanner"), 15000, true);

    // IoT
    addStep(catalog.createAttackByName("Zigbee Sniffer"), 20000, true);
    addStep(catalog.createAttackByName("Smart Home Scanner"), 30000, true);
  }

  void logResults() {
    Serial.println("\n========= RÉSULTATS SCAN ==========");
    
    for (uint16_t i = 0; i < MAX_STEPS && i < steps.size(); i++) {
      if (!steps[i]->attack) continue;

      const char* name = steps[i]->attack->getName();
      uint16_t count = steps[i]->attack->getResultCount();

      Serial.printf("%s: %u résultats\n", name, count);
      
      for (uint16_t j = 0; j < count && j < 5; j++) {
        const AttackResult* result = steps[i]->attack->getResult(j);
        Serial.printf("  - %s\n", result->data);
      }
    }
  }
};

void example_multi_frequency_scan() {
  MultiFrequencyScan scan;
  scan.setupAttacks();
  scan.start();

  // Boucle de mise à jour
  uint32_t last_update = millis();
  while (scan.isRunning()) {
    scan.update();
    
    // Afficher progrès
    if ((millis() - last_update) > 5000) {
      uint8_t progress = scan.getProgress();
      Serial.printf("Progrès: %u%%\n", progress);
      last_update = millis();
    }

    delay(50);
  }

  scan.logResults();
}
```

### Workflow: Test de Pénétration WiFi

```cpp
void example_wifi_penetration_test() {
  AttackWorkflow pentest("WiFi Pentest");
  AttackCatalog& catalog = AttackCatalog::getInstance();

  Serial.println("🔍 Phase 1: Reconnaissance");
  Attack* scan = catalog.createAttackByName("WiFi Network Scan");
  pentest.addStep(scan, 15000, true);

  Serial.println("🎯 Phase 2: Capture Handshake");
  Attack* capture = catalog.createAttackByName("WiFi Handshake Capture");
  pentest.addStep(capture, 60000, true);

  Serial.println("💣 Phase 3: Attaque Deauth");
  Attack* deauth = catalog.createAttackByName("WiFi Deauth");
  deauth->setParameter("channel", "6");
  pentest.addStep(deauth, 5000, true);

  Serial.println("🚀 Démarrage pentest");
  pentest.start();

  while (pentest.isRunning()) {
    pentest.update();
    
    Attack* current = pentest.getCurrentAttack();
    if (current) {
      Serial.printf("En cours: %s\n", current->getName());
    }

    delay(100);
  }

  Serial.println("✅ Pentest terminé");
}
```

### Workflow: Chaîne IoT/Maison Intelligente

```cpp
void example_smart_home_chain() {
  AttackWorkflow smart_home("Smart Home Audit");
  AttackCatalog& catalog = AttackCatalog::getInstance();

  // Découverte appareils
  smart_home.addStep(
    catalog.createAttackByName("Smart Home Scanner"), 30000, true);

  // Sniffing Zigbee
  smart_home.addStep(
    catalog.createAttackByName("Zigbee Sniffer"), 20000, true);

  // Interception MQTT
  smart_home.addStep(
    catalog.createAttackByName("MQTT Interceptor"), 15000, true);

  // Analyse sécurité
  smart_home.addStep(
    catalog.createAttackByName("Vulnerability Scanner"), 30000, true);

  smart_home.start();

  // Affichage réel-temps
  while (smart_home.isRunning()) {
    smart_home.update();

    static uint32_t last_print = 0;
    if ((millis() - last_print) > 2000) {
      Serial.printf("Progrès: %u%%\n", smart_home.getProgress());
      last_print = millis();
    }

    delay(50);
  }
}
```

---

## Intégration avec le Système

### Intégration Menu Principal

```cpp
// Dans menu.cpp
void displayAttackMenu() {
  AttackCatalog& catalog = AttackCatalog::getInstance();

  while (true) {
    clearScreen();
    Serial.println("========== ATTAQUES ==========");

    // Afficher catégories
    for (int i = 0; i < 6; i++) {
      AttackCategory cat = (AttackCategory)i;
      uint8_t count = catalog.getAttackCountByCategory(cat);
      Serial.printf("[%d] %s (%u attaques)\n", 
                    i + 1, catalog.getCategoryName(cat), count);
    }

    Serial.println("[0] Retour");
    Serial.print("Sélection: ");

    int choice = readInput();

    if (choice == 0) break;
    if (choice >= 1 && choice <= 6) {
      displayCategoryMenu((AttackCategory)(choice - 1));
    }
  }
}

void displayCategoryMenu(AttackCategory category) {
  AttackCatalog& catalog = AttackCatalog::getInstance();

  while (true) {
    clearScreen();
    Serial.printf("========== %s ==========\n", 
                  catalog.getCategoryName(category));

    uint8_t count = catalog.getAttackCountByCategory(category);
    for (uint8_t i = 0; i < count; i++) {
      const char* name = catalog.getAttackName(category, i);
      Serial.printf("[%u] %s\n", i + 1, name);
    }

    Serial.println("[0] Retour");
    Serial.print("Sélection: ");

    int choice = readInput();

    if (choice == 0) break;
    if (choice >= 1 && choice <= count) {
      runAttack(category, choice - 1);
    }
  }
}

void runAttack(AttackCategory category, uint8_t index) {
  Attack* attack = AttackCatalog::getInstance()
    .createAttack(category, index);

  if (!attack) {
    Serial.println("❌ Erreur création attaque");
    delay(2000);
    return;
  }

  Serial.printf("▶️ Démarrage: %s\n", attack->getName());

  attack->begin();
  attack->start();

  uint32_t start = millis();
  while (attack->isRunning()) {
    attack->update();

    // Afficher statut
    uint32_t elapsed = millis() - start;
    Serial.printf("\rÉlapsé: %u ms | Résultats: %u", 
                  elapsed, attack->getResultCount());

    delay(50);
  }

  attack->stop();

  // Afficher résultats
  Serial.println("\n\n========== RÉSULTATS ==========");
  Serial.printf("Attaque: %s\n", attack->getName());
  Serial.printf("Statut: %u\n", (uint8_t)attack->getCurrentStatus());
  Serial.printf("Résultats: %u\n", attack->getResultCount());

  for (uint16_t i = 0; i < attack->getResultCount() && i < 20; i++) {
    const AttackResult* result = attack->getResult(i);
    Serial.printf("  [%u] %s\n", i + 1, result->data);
  }

  Serial.println("\nAppuyez sur [SELECT] pour continuer...");
  while (!buttonSelectPressed()) delay(100);

  delete attack;
}
```

### Intégration avec État Machine UI

```cpp
class AttackExecutionHandler : public UIStateHandler {
private:
  Attack* currentAttack;
  uint32_t startTime;

public:
  void onEnter() override {
    // Créer attaque depuis configuration
    currentAttack = createAttackFromConfig();
    currentAttack->begin();
    currentAttack->start();
    startTime = millis();
  }

  void update() override {
    if (!currentAttack || !currentAttack->isRunning()) {
      // Transition vers résultats
      uiState->setState(UIState::ATTACK_RESULTS);
      return;
    }

    currentAttack->update();
  }

  void render() override {
    // Afficher barre progrès
    uint32_t elapsed = millis() - startTime;
    uint8_t percent = (elapsed * 100) / 10000; // Exemple 10sec

    drawProgressBar(percent);
    drawResultCount(currentAttack->getResultCount());
    drawCurrentStatus(currentAttack->getCurrentStatus());
  }

  void onExit() override {
    if (currentAttack) {
      currentAttack->stop();
      delete currentAttack;
    }
  }
};
```

---

## Bonnes Pratiques

### 1. Gestion Mémoire

```cpp
// ✅ BON: Libération explicite
Attack* attack = catalog.createAttackByName("WiFi Scan");
attack->begin();
attack->start();
// ... utiliser ...
attack->stop();
delete attack; // Libérer mémoire

// ❌ MAUVAIS: Fuite mémoire
Attack* attack = catalog.createAttackByName("WiFi Scan");
// Oubli de delete

// ✅ BON: Utiliser des destructeurs
class AttackWrapper {
  Attack* attack;
public:
  AttackWrapper(const char* name) 
    : attack(catalog.createAttackByName(name)) {}
  
  ~AttackWrapper() {
    if (attack) delete attack;
  }
};
```

### 2. Gestion Erreurs

```cpp
// ✅ BON: Vérifier création attaque
Attack* attack = catalog.createAttackByName("WiFi Scan");
if (!attack) {
  Logger::getInstance().error("Attaque", "Création échouée");
  return false;
}

// ✅ BON: Vérifier démarrage
if (!attack->begin()) {
  Logger::getInstance().error("Attaque", "Initialisation échouée");
  delete attack;
  return false;
}

// ✅ BON: Timeout de sécurité
uint32_t start = millis();
uint32_t max_duration = 60000; // 60 secondes

while (attack->isRunning() && 
       (millis() - start) < max_duration) {
  attack->update();
  delay(50);
}

// Force arrêt si timeout
if (attack->isRunning()) {
  attack->stop();
}
```

### 3. Logging et Monitoring

```cpp
// ✅ BON: Logger activité
Attack* attack = catalog.createAttackByName("WiFi Scan");

Logger& logger = Logger::getInstance();
logger.info("Attaque", "Démarrage WiFi Scan");

attack->begin();
attack->start();

while (attack->isRunning()) {
  attack->update();
  
  static uint32_t last_log = 0;
  if ((millis() - last_log) > 5000) {
    logger.info("Attaque", 
      "Progression: %u résultats", 
      attack->getResultCount());
    last_log = millis();
  }

  delay(50);
}

logger.info("Attaque", "Terminée");
delete attack;
```

### 4. Paramétrage Sécurisé

```cpp
// ✅ BON: Valider paramètres
Attack* attack = catalog.createAttackByName("WiFi Deauth");

// Valider avant de lancer
const char* bssid = getUserInput("BSSID cible");
if (!isValidBSSID(bssid)) {
  logger.error("Attaque", "BSSID invalide");
  delete attack;
  return false;
}

const char* channel_str = getUserInput("Canal (1-13)");
int channel = atoi(channel_str);
if (channel < 1 || channel > 13) {
  logger.error("Attaque", "Canal invalide");
  delete attack;
  return false;
}

// Configurer paramètres validés
attack->setParameter("bssid", bssid);
attack->setParameter("channel", channel_str);

attack->begin();
attack->start();
// ...
```

### 5. Cleanup Ressources

```cpp
// ✅ BON: Nettoyer après attaque
Attack* attack = catalog.createAttackByName("NFC Tag Reader");

attack->begin();
attack->start();

while (attack->isRunning()) {
  attack->update();
  delay(50);
}

attack->stop();

// Nettoyer résultats si nécessaire
if (attack->getResultCount() > 1000) {
  attack->clearResults();
}

delete attack;
```

---

## Flux Complet d'Application

```cpp
void setup() {
  Serial.begin(115200);
  
  // Initialiser systèmes
  Display::getInstance().begin();
  Logger::getInstance().info("System", "Démarrage");
  
  // Vérifier catalogue
  AttackCatalog::getInstance().printCatalog();
}

void loop() {
  // Menu principal
  displayMainMenu();
  
  // Sélection utilisateur
  int option = getUserMenuChoice();
  
  switch (option) {
    case 1: // Attaques WiFi
      displayCategoryMenu(AttackCategory::WIFI);
      break;
    
    case 2: // Workflows
      runPresetWorkflow();
      break;
    
    case 3: // Diagnostic
      runSystemDiagnostics();
      break;
    
    default:
      break;
  }
  
  delay(100);
}
```

---

**Version**: 1.0  
**Date**: 2026-09-27  
**Cible**: ESP32-S3 Phase 0
