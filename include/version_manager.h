#ifndef VERSION_MANAGER_H
#define VERSION_MANAGER_H

#include <Arduino.h>

#define VERSION_MAJOR 2
#define VERSION_MINOR 1
#define VERSION_PATCH 0

class SemanticVersion {
public:
  SemanticVersion(uint16_t maj = 0, uint16_t min = 0, uint16_t pat = 0)
    : major(maj), minor(min), patch(pat) {}

  static SemanticVersion currentVersion() {
    return SemanticVersion(VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
  }

  bool isCompatibleWith(const SemanticVersion& other) const {
    return major == other.major;
  }

  int compare(const SemanticVersion& other) const {
    if (major != other.major) return major < other.major ? -1 : 1;
    if (minor != other.minor) return minor < other.minor ? -1 : 1;
    if (patch != other.patch) return patch < other.patch ? -1 : 1;
    return 0;
  }

  void print() {
    Serial.printf("%u.%u.%u\n", major, minor, patch);
  }

  char* toString(char* buffer, size_t bufLen) {
    snprintf(buffer, bufLen, "%u.%u.%u", major, minor, patch);
    return buffer;
  }

private:
  uint16_t major;
  uint16_t minor;
  uint16_t patch;
};

class VersionManager {
public:
  static VersionManager& getInstance() {
    static VersionManager instance;
    return instance;
  }

  void registerMigration(const SemanticVersion& from, const SemanticVersion& to,
                        std::function<bool()> migration) {
    if (!migrations) {
      migrations = new std::vector<MigrationEntry*>();
    }
    migrations->push_back(new MigrationEntry{from, to, migration});
  }

  bool migrate(const SemanticVersion& currentVer) {
    SemanticVersion target = SemanticVersion::currentVersion();

    if (currentVer.compare(target) >= 0) {
      return true;
    }

    if (!migrations) return false;

    for (auto* entry : *migrations) {
      if (entry->from.compare(currentVer) == 0 &&
          entry->to.compare(target) == 0) {
        return entry->migrate();
      }
    }

    return false;
  }

  void printVersionInfo() {
    Serial.println("\n=== VERSION INFO ===");
    Serial.print("Current: ");
    SemanticVersion::currentVersion().print();
    Serial.println("====================\n");
  }

private:
  VersionManager() : migrations(nullptr) {}

  struct MigrationEntry {
    SemanticVersion from;
    SemanticVersion to;
    std::function<bool()> migrate;
  };

  std::vector<MigrationEntry*>* migrations;
};

#endif
