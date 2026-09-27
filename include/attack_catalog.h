#ifndef ATTACK_CATALOG_H
#define ATTACK_CATALOG_H

#include "attack_framework.h"
#include "wifi_attacks.h"
#include "ble_attacks.h"
#include "rf_attacks.h"
#include "nfc_attacks.h"
#include "dos_attacks.h"
#include "advanced_attacks.h"

// ============= ATTACK CATEGORIES =============
enum class AttackCategory {
  WIFI = 0,
  BLE = 1,
  RF = 2,
  NFC = 3,
  DOS = 4,
  ADVANCED = 5,
  TOTAL_CATEGORIES = 6
};

// ============= ATTACK CATALOG =============
class AttackCatalog {
public:
  static AttackCatalog& getInstance() {
    static AttackCatalog instance;
    return instance;
  }

  // Création d'attaques par catégorie
  Attack* createWiFiAttack(uint8_t index);
  Attack* createBLEAttack(uint8_t index);
  Attack* createRFAttack(uint8_t index);
  Attack* createNFCAttack(uint8_t index);
  Attack* createDOSAttack(uint8_t index);
  Attack* createAdvancedAttack(uint8_t index);

  // Interface unifiée
  Attack* createAttack(AttackCategory category, uint8_t attackIndex);
  Attack* createAttackByName(const char* name);

  // Information catalogue
  const char* getCategoryName(AttackCategory category);
  uint8_t getAttackCountByCategory(AttackCategory category);
  const char* getAttackName(AttackCategory category, uint8_t index);
  const char* getAttackDescription(AttackCategory category, uint8_t index);

  // Lister toutes les attaques
  void printCatalog();
  void printCategory(AttackCategory category);

private:
  AttackCatalog();

  // Tables de descriptions
  static const char* WIFI_ATTACKS[];
  static const char* BLE_ATTACKS[];
  static const char* RF_ATTACKS[];
  static const char* NFC_ATTACKS[];
  static const char* DOS_ATTACKS[];
  static const char* ADVANCED_ATTACKS[];

  static const uint8_t WIFI_COUNT = 7;
  static const uint8_t BLE_COUNT = 7;
  static const uint8_t RF_COUNT = 8;
  static const uint8_t NFC_COUNT = 7;
  static const uint8_t DOS_COUNT = 6;
  static const uint8_t ADVANCED_COUNT = 8;
};

#endif // ATTACK_CATALOG_H
