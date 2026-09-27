#include "config_optimizer.h"

void ConfigOptimizer::setU32(const char* key, uint32_t value) {
  auto it = config.find(key);
  if (it != config.end()) delete it->second;

  ConfigEntry* entry = new ConfigEntry();
  entry->type = ConfigEntry::U32;
  entry->data.u32 = value;
  config[key] = entry;
}

void ConfigOptimizer::setU16(const char* key, uint16_t value) {
  auto it = config.find(key);
  if (it != config.end()) delete it->second;

  ConfigEntry* entry = new ConfigEntry();
  entry->type = ConfigEntry::U16;
  entry->data.u16 = value;
  config[key] = entry;
}

void ConfigOptimizer::setU8(const char* key, uint8_t value) {
  auto it = config.find(key);
  if (it != config.end()) delete it->second;

  ConfigEntry* entry = new ConfigEntry();
  entry->type = ConfigEntry::U8;
  entry->data.u8 = value;
  config[key] = entry;
}

void ConfigOptimizer::setString(const char* key, const char* value) {
  auto it = config.find(key);
  if (it != config.end()) delete it->second;

  ConfigEntry* entry = new ConfigEntry();
  entry->type = ConfigEntry::STRING;
  if (value) {
    entry->data.strVal = (char*)malloc(strlen(value) + 1);
    strcpy(entry->data.strVal, value);
  } else {
    entry->data.strVal = nullptr;
  }
  config[key] = entry;
}

void ConfigOptimizer::setBool(const char* key, bool value) {
  auto it = config.find(key);
  if (it != config.end()) delete it->second;

  ConfigEntry* entry = new ConfigEntry();
  entry->type = ConfigEntry::BOOL;
  entry->data.boolVal = value;
  config[key] = entry;
}

bool ConfigOptimizer::getU32(const char* key, uint32_t& output) {
  auto it = config.find(key);
  if (it != config.end() && it->second->type == ConfigEntry::U32) {
    output = it->second->data.u32;
    return true;
  }
  return false;
}

bool ConfigOptimizer::getU16(const char* key, uint16_t& output) {
  auto it = config.find(key);
  if (it != config.end() && it->second->type == ConfigEntry::U16) {
    output = it->second->data.u16;
    return true;
  }
  return false;
}

bool ConfigOptimizer::getU8(const char* key, uint8_t& output) {
  auto it = config.find(key);
  if (it != config.end() && it->second->type == ConfigEntry::U8) {
    output = it->second->data.u8;
    return true;
  }
  return false;
}

bool ConfigOptimizer::getString(const char* key, char* output, size_t maxLen) {
  auto it = config.find(key);
  if (it != config.end() && it->second->type == ConfigEntry::STRING) {
    if (it->second->data.strVal) {
      strncpy(output, it->second->data.strVal, maxLen - 1);
      output[maxLen - 1] = '\0';
      return true;
    }
  }
  return false;
}

bool ConfigOptimizer::getBool(const char* key, bool& output) {
  auto it = config.find(key);
  if (it != config.end() && it->second->type == ConfigEntry::BOOL) {
    output = it->second->data.boolVal;
    return true;
  }
  return false;
}

void ConfigOptimizer::printConfig() {
  Serial.println("\n=== CONFIG DUMP ===");
  for (auto& pair : config) {
    ConfigEntry* entry = pair.second;
    Serial.printf("%s: ", pair.first);

    switch (entry->type) {
      case ConfigEntry::U32:
        Serial.printf("%lu\n", entry->data.u32);
        break;
      case ConfigEntry::U16:
        Serial.printf("%u\n", entry->data.u16);
        break;
      case ConfigEntry::U8:
        Serial.printf("%u\n", entry->data.u8);
        break;
      case ConfigEntry::STRING:
        Serial.printf("%s\n", entry->data.strVal ? entry->data.strVal : "(null)");
        break;
      case ConfigEntry::BOOL:
        Serial.printf("%s\n", entry->data.boolVal ? "true" : "false");
        break;
    }
  }
  Serial.println("===================\n");
}

void ConfigOptimizer::clearAll() {
  for (auto& pair : config) {
    delete pair.second;
  }
  config.clear();
}
