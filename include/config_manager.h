#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <Arduino.h>
#include <vector>
#include <cstring>
#include "attack_framework.h"

// ============= CONFIGURATION STRUCTURES =============

enum class PowerProfile {
  PERFORMANCE = 0,  // Max CPU, all RF modules active
  BALANCED = 1,     // Medium CPU, selective RF modules
  STEALTH = 2,      // Min CPU, minimal RF emissions
  LOW_POWER = 3     // Critical - only essential functions
};

struct AttackPreset {
  char name[32];              // Preset name (e.g., "WiFi Aggressive")
  char attackName[64];        // Attack class name
  char parameters[256];       // Comma-separated key=value pairs
  PowerProfile profile;       // Power mode for this attack
  uint16_t duration;          // Max duration in seconds (0 = unlimited)
  bool enabled;
  uint32_t timestamp;         // Creation time

  AttackPreset() : profile(PowerProfile::BALANCED), duration(0),
                   enabled(true), timestamp(0) {
    memset(name, 0, 32);
    memset(attackName, 0, 64);
    memset(parameters, 0, 256);
  }
};

struct UserProfile {
  char profileName[32];       // e.g., "Aggressive", "Stealth", "Research"
  PowerProfile defaultProfile;
  uint16_t maxAttackDuration; // Global max duration in seconds
  bool enableLogging;
  bool enableBenchmarking;
  bool vibrationFeedback;
  uint8_t brightnessLevel;    // 0-100

  UserProfile() : defaultProfile(PowerProfile::BALANCED),
                  maxAttackDuration(600), enableLogging(true),
                  enableBenchmarking(true), vibrationFeedback(true),
                  brightnessLevel(80) {
    memset(profileName, 0, 32);
  }
};

struct SystemConfig {
  uint32_t version;           // Config format version
  char deviceName[32];        // Device identifier
  bool autoStartAttacks;      // Resume interrupted attacks on reboot
  bool enableNVSPersistence;  // Save state to NVS
  uint16_t nvsCheckInterval;  // ms between NVS saves
  uint8_t maxConcurrentAttacks;
  bool enableResourceMonitoring;

  SystemConfig() : version(1), autoStartAttacks(false),
                   enableNVSPersistence(true), nvsCheckInterval(30000),
                   maxConcurrentAttacks(3), enableResourceMonitoring(true) {
    memset(deviceName, 0, 32);
  }
};

// ============= CONFIGURATION MANAGER =============

class ConfigManager {
public:
  static ConfigManager& getInstance() {
    static ConfigManager instance;
    return instance;
  }

  // System configuration
  bool loadSystemConfig();
  bool saveSystemConfig();
  SystemConfig* getSystemConfig() { return &sysConfig; }

  // User profiles
  bool createUserProfile(const UserProfile& profile);
  bool loadUserProfile(const char* profileName);
  bool deleteUserProfile(const char* profileName);
  UserProfile* getCurrentProfile() { return &currentProfile; }
  uint16_t getAllProfiles(UserProfile* output, uint16_t maxCount);

  // Attack presets
  bool createPreset(const AttackPreset& preset);
  bool loadPreset(const char* presetName);
  bool deletePreset(const char* presetName);
  bool updatePreset(const AttackPreset& preset);
  AttackPreset* getPreset(const char* presetName);
  uint16_t getAllPresets(AttackPreset* output, uint16_t maxCount);

  // Parameter management
  bool setParameter(const char* key, const char* value);
  const char* getParameter(const char* key);
  bool removeParameter(const char* key);

  // Preset parameter parsing
  bool parsePresetParameters(const char* paramStr, char** outKeys,
                            char** outValues, uint16_t maxCount, uint16_t& count);

  // Configuration export/import
  bool exportConfig(char* output, uint16_t maxLen);
  bool importConfig(const char* configStr);

  // Validation
  bool validatePreset(const AttackPreset& preset);
  bool validateProfile(const UserProfile& profile);
  bool validateConfig(const SystemConfig& config);

  // State persistence
  bool saveState(const char* stateKey, const char* stateData);
  const char* loadState(const char* stateKey);
  bool deleteState(const char* stateKey);

  // Defaults
  void resetToDefaults();
  void applyDefaults();

private:
  ConfigManager();

  SystemConfig sysConfig;
  UserProfile currentProfile;
  std::vector<AttackPreset*> presets;
  std::vector<UserProfile*> profiles;

  struct ConfigEntry {
    char key[64];
    char value[256];
  };
  std::vector<ConfigEntry*> parameters;

  static const uint16_t MAX_PRESETS = 20;
  static const uint16_t MAX_PROFILES = 10;
  static const uint16_t MAX_PARAMETERS = 50;

  bool loadFromNVS();
  bool saveToNVS();
  void initializeDefaults();
};

#endif // CONFIG_MANAGER_H
