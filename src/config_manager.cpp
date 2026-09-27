#include "config_manager.h"
#include "logging_system.h"

// ============= CONFIGURATION MANAGER IMPLEMENTATION =============

ConfigManager::ConfigManager() {
  initializeDefaults();
}

void ConfigManager::initializeDefaults() {
  // Initialize system config
  sysConfig = SystemConfig();
  strncpy(sysConfig.deviceName, "ESP32-S3-Security", 31);

  // Initialize default user profile
  currentProfile = UserProfile();
  strncpy(currentProfile.profileName, "Default", 31);

  // Load from NVS if available
  loadFromNVS();
}

bool ConfigManager::loadSystemConfig() {
  Logger::getInstance().info("Config", "Loading system configuration");
  return loadFromNVS();
}

bool ConfigManager::saveSystemConfig() {
  Logger::getInstance().info("Config", "Saving system configuration");
  return saveToNVS();
}

bool ConfigManager::createUserProfile(const UserProfile& profile) {
  if (!validateProfile(profile)) {
    Logger::getInstance().error("Config", "Invalid profile");
    return false;
  }

  // Check if profile already exists
  for (auto* p : profiles) {
    if (strcmp(p->profileName, profile.profileName) == 0) {
      Logger::getInstance().warn("Config", "Profile already exists");
      return false;
    }
  }

  if (profiles.size() >= MAX_PROFILES) {
    Logger::getInstance().error("Config", "Max profiles reached");
    return false;
  }

  UserProfile* newProfile = new UserProfile(profile);
  profiles.push_back(newProfile);

  char msg[128];
  snprintf(msg, 127, "Profile created: %s", profile.profileName);
  Logger::getInstance().info("Config", msg);

  return saveToNVS();
}

bool ConfigManager::loadUserProfile(const char* profileName) {
  for (auto* p : profiles) {
    if (strcmp(p->profileName, profileName) == 0) {
      currentProfile = *p;

      char msg[128];
      snprintf(msg, 127, "Profile loaded: %s", profileName);
      Logger::getInstance().info("Config", msg);
      return true;
    }
  }

  Logger::getInstance().warn("Config", "Profile not found");
  return false;
}

bool ConfigManager::deleteUserProfile(const char* profileName) {
  for (uint16_t i = 0; i < profiles.size(); i++) {
    if (strcmp(profiles[i]->profileName, profileName) == 0) {
      delete profiles[i];
      profiles.erase(profiles.begin() + i);

      char msg[128];
      snprintf(msg, 127, "Profile deleted: %s", profileName);
      Logger::getInstance().info("Config", msg);
      return saveToNVS();
    }
  }

  return false;
}

uint16_t ConfigManager::getAllProfiles(UserProfile* output, uint16_t maxCount) {
  uint16_t count = (profiles.size() < maxCount) ? profiles.size() : maxCount;
  for (uint16_t i = 0; i < count; i++) {
    output[i] = *profiles[i];
  }
  return count;
}

bool ConfigManager::createPreset(const AttackPreset& preset) {
  if (!validatePreset(preset)) {
    Logger::getInstance().error("Config", "Invalid preset");
    return false;
  }

  // Check if preset already exists
  for (auto* p : presets) {
    if (strcmp(p->name, preset.name) == 0) {
      Logger::getInstance().warn("Config", "Preset already exists");
      return false;
    }
  }

  if (presets.size() >= MAX_PRESETS) {
    Logger::getInstance().error("Config", "Max presets reached");
    return false;
  }

  AttackPreset* newPreset = new AttackPreset(preset);
  newPreset->timestamp = millis();
  presets.push_back(newPreset);

  char msg[128];
  snprintf(msg, 127, "Preset created: %s", preset.name);
  Logger::getInstance().info("Config", msg);

  return saveToNVS();
}

bool ConfigManager::loadPreset(const char* presetName) {
  AttackPreset* preset = getPreset(presetName);
  if (!preset) return false;

  char msg[128];
  snprintf(msg, 127, "Preset loaded: %s", presetName);
  Logger::getInstance().info("Config", msg);
  return true;
}

bool ConfigManager::deletePreset(const char* presetName) {
  for (uint16_t i = 0; i < presets.size(); i++) {
    if (strcmp(presets[i]->name, presetName) == 0) {
      delete presets[i];
      presets.erase(presets.begin() + i);

      char msg[128];
      snprintf(msg, 127, "Preset deleted: %s", presetName);
      Logger::getInstance().info("Config", msg);
      return saveToNVS();
    }
  }
  return false;
}

bool ConfigManager::updatePreset(const AttackPreset& preset) {
  for (auto* p : presets) {
    if (strcmp(p->name, preset.name) == 0) {
      *p = preset;

      char msg[128];
      snprintf(msg, 127, "Preset updated: %s", preset.name);
      Logger::getInstance().info("Config", msg);
      return saveToNVS();
    }
  }
  return false;
}

AttackPreset* ConfigManager::getPreset(const char* presetName) {
  for (auto* p : presets) {
    if (strcmp(p->name, presetName) == 0) {
      return p;
    }
  }
  return nullptr;
}

uint16_t ConfigManager::getAllPresets(AttackPreset* output, uint16_t maxCount) {
  uint16_t count = (presets.size() < maxCount) ? presets.size() : maxCount;
  for (uint16_t i = 0; i < count; i++) {
    output[i] = *presets[i];
  }
  return count;
}

bool ConfigManager::setParameter(const char* key, const char* value) {
  // Check if parameter already exists
  for (auto* entry : parameters) {
    if (strcmp(entry->key, key) == 0) {
      strncpy(entry->value, value, 255);
      return true;
    }
  }

  // Create new parameter
  if (parameters.size() >= MAX_PARAMETERS) {
    Logger::getInstance().warn("Config", "Max parameters reached");
    return false;
  }

  ConfigEntry* entry = new ConfigEntry();
  strncpy(entry->key, key, 63);
  strncpy(entry->value, value, 255);
  parameters.push_back(entry);

  return true;
}

const char* ConfigManager::getParameter(const char* key) {
  for (auto* entry : parameters) {
    if (strcmp(entry->key, key) == 0) {
      return entry->value;
    }
  }
  return nullptr;
}

bool ConfigManager::removeParameter(const char* key) {
  for (uint16_t i = 0; i < parameters.size(); i++) {
    if (strcmp(parameters[i]->key, key) == 0) {
      delete parameters[i];
      parameters.erase(parameters.begin() + i);
      return true;
    }
  }
  return false;
}

bool ConfigManager::parsePresetParameters(const char* paramStr, char** outKeys,
                                          char** outValues, uint16_t maxCount,
                                          uint16_t& count) {
  if (!paramStr || !outKeys || !outValues) return false;

  count = 0;
  char temp[256];
  strncpy(temp, paramStr, 255);

  char* token = strtok(temp, ",");
  while (token && count < maxCount) {
    // Parse key=value
    char* equals = strchr(token, '=');
    if (equals) {
      *equals = '\0';
      outKeys[count] = token;
      outValues[count] = equals + 1;
      count++;
    }
    token = strtok(nullptr, ",");
  }

  return count > 0;
}

bool ConfigManager::exportConfig(char* output, uint16_t maxLen) {
  if (!output) return false;

  uint16_t written = 0;
  written += snprintf(output + written, maxLen - written,
    "{\"version\":%u,\"device\":\"%s\",\"profile\":\"%s\",\"presets\":[",
    sysConfig.version, sysConfig.deviceName, currentProfile.profileName);

  for (uint16_t i = 0; i < presets.size(); i++) {
    if (i > 0) written += snprintf(output + written, maxLen - written, ",");
    written += snprintf(output + written, maxLen - written,
      "{\"name\":\"%s\",\"attack\":\"%s\",\"profile\":%u}",
      presets[i]->name, presets[i]->attackName, (uint8_t)presets[i]->profile);
  }

  written += snprintf(output + written, maxLen - written, "]}");

  return written < maxLen;
}

bool ConfigManager::importConfig(const char* configStr) {
  if (!configStr) return false;

  Logger::getInstance().info("Config", "Importing configuration");
  // TODO: Implement JSON parsing for import
  return false;
}

bool ConfigManager::validatePreset(const AttackPreset& preset) {
  if (strlen(preset.name) == 0) return false;
  if (strlen(preset.attackName) == 0) return false;
  return true;
}

bool ConfigManager::validateProfile(const UserProfile& profile) {
  if (strlen(profile.profileName) == 0) return false;
  if (profile.brightnessLevel > 100) return false;
  return true;
}

bool ConfigManager::validateConfig(const SystemConfig& config) {
  if (config.maxConcurrentAttacks == 0) return false;
  return true;
}

bool ConfigManager::saveState(const char* stateKey, const char* stateData) {
  // TODO: Implement NVS state persistence
  Logger::getInstance().info("Config", "State saved");
  return true;
}

const char* ConfigManager::loadState(const char* stateKey) {
  // TODO: Implement NVS state loading
  return nullptr;
}

bool ConfigManager::deleteState(const char* stateKey) {
  // TODO: Implement NVS state deletion
  return true;
}

void ConfigManager::resetToDefaults() {
  sysConfig = SystemConfig();
  currentProfile = UserProfile();

  for (auto* p : presets) delete p;
  presets.clear();

  for (auto* p : profiles) delete p;
  profiles.clear();

  for (auto* p : parameters) delete p;
  parameters.clear();

  Logger::getInstance().info("Config", "Reset to defaults");
}

void ConfigManager::applyDefaults() {
  initializeDefaults();
}

bool ConfigManager::loadFromNVS() {
  // TODO: Implement NVS loading
  // This would load presets, profiles, and system config from ESP32 NVS
  return true;
}

bool ConfigManager::saveToNVS() {
  // TODO: Implement NVS saving
  // This would save presets, profiles, and system config to ESP32 NVS
  return true;
}
