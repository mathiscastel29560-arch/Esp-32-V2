#ifndef CONFIG_OPTIMIZER_H
#define CONFIG_OPTIMIZER_H

#include <Arduino.h>
#include <map>
#include <cstring>

class ConfigOptimizer {
public:
  static ConfigOptimizer& getInstance() {
    static ConfigOptimizer instance;
    return instance;
  }

  void setU32(const char* key, uint32_t value);
  void setU16(const char* key, uint16_t value);
  void setU8(const char* key, uint8_t value);
  void setString(const char* key, const char* value);
  void setBool(const char* key, bool value);

  bool getU32(const char* key, uint32_t& output);
  bool getU16(const char* key, uint16_t& output);
  bool getU8(const char* key, uint8_t& output);
  bool getString(const char* key, char* output, size_t maxLen);
  bool getBool(const char* key, bool& output);

  void printConfig();
  void clearAll();

private:
  ConfigOptimizer() {}

  struct ConfigEntry {
    enum Type { U32, U16, U8, STRING, BOOL } type;
    union {
      uint32_t u32;
      uint16_t u16;
      uint8_t u8;
      bool boolVal;
      char* strVal;
    } data;

    ~ConfigEntry() {
      if (type == STRING && data.strVal) {
        free(data.strVal);
      }
    }
  };

  std::map<const char*, ConfigEntry*> config;
};

#endif
