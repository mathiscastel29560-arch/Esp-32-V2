#ifndef MODULE_REGISTRY_H
#define MODULE_REGISTRY_H

#include <Arduino.h>
#include <vector>
#include "attack_framework.h"

// ============= MODULE TYPES =============

enum class ModuleType {
  ATTACK = 0,
  DRIVER = 1,
  SENSOR = 2,
  UTILITY = 3,
  UI = 4
};

struct ModuleInfo {
  const char* name;
  const char* version;
  ModuleType type;
  const char* author;
  const char* description;
  uint16_t requiredMemory;  // KB
  bool enabled;
  bool loaded;
  uint32_t loadTime;

  ModuleInfo() : name(""), version("1.0.0"), type(ModuleType::ATTACK),
                 author(""), description(""), requiredMemory(0),
                 enabled(true), loaded(false), loadTime(0) {}
};

struct ModuleDependency {
  const char* moduleName;
  const char* requiredVersion;
  bool satisfied;

  ModuleDependency() : moduleName(""), requiredVersion(""), satisfied(false) {}
};

// ============= DYNAMIC MODULE LOADER =============

typedef Attack* (*AttackFactory)();
typedef void (*ModuleInitFunc)();
typedef void (*ModuleCleanupFunc)();

struct LoadedModule {
  ModuleInfo info;
  ModuleInitFunc init;
  ModuleCleanupFunc cleanup;
  AttackFactory factory;
  std::vector<ModuleDependency*> dependencies;
};

class ModuleRegistry {
public:
  static ModuleRegistry& getInstance() {
    static ModuleRegistry instance;
    return instance;
  }

  // Module registration
  bool registerModule(const ModuleInfo& info, ModuleInitFunc init,
                     ModuleCleanupFunc cleanup, AttackFactory factory = nullptr);
  bool unregisterModule(const char* moduleName);

  // Module loading/unloading
  bool loadModule(const char* moduleName);
  bool unloadModule(const char* moduleName);
  bool reloadModule(const char* moduleName);

  // Module queries
  ModuleInfo* getModuleInfo(const char* moduleName);
  uint16_t getAllModules(ModuleInfo* output, uint16_t maxCount);
  uint16_t getLoadedModules(ModuleInfo* output, uint16_t maxCount);

  // Attack creation from module
  Attack* createAttackFromModule(const char* moduleName);

  // Dependency management
  bool addDependency(const char* moduleName, const char* dependsOn, const char* version);
  bool checkDependencies(const char* moduleName);
  bool resolveDependencies();

  // Status
  uint16_t getModuleCount() const { return modules.size(); }
  uint16_t getLoadedCount() const;
  uint32_t getTotalMemoryUsage() const;

  bool isModuleLoaded(const char* moduleName) const;
  bool canLoadModule(const char* moduleName) const;

  void printModuleInfo();
  void printDependencyTree();

private:
  ModuleRegistry();

  std::vector<LoadedModule*> modules;
  static const uint16_t MAX_MODULES = 50;
  static const uint32_t MAX_MEMORY = 2048; // KB
};

// ============= MODULE FACTORY =============

class ModuleFactory {
public:
  static ModuleFactory& getInstance() {
    static ModuleFactory instance;
    return instance;
  }

  // Register built-in modules at startup
  void registerBuiltinModules();

  // Attack creation helpers
  Attack* createAttack(const char* attackName);
  Attack* createAttackByCategory(const char* category);

  uint16_t getAttackCount() const;
  uint16_t getAttacksByCategory(const char* category, const char** output, uint16_t maxCount);

private:
  ModuleFactory() {}
};

// ============= MODULE VALIDATOR =============

class ModuleValidator {
public:
  static ModuleValidator& getInstance() {
    static ModuleValidator instance;
    return instance;
  }

  bool validateModule(const ModuleInfo& info);
  bool validateMemory(const ModuleInfo& info);
  bool validateDependencies(const ModuleInfo& info);
  bool validateVersion(const char* version);

  const char* getLastError() { return lastError; }

private:
  ModuleValidator() : lastError("") {}

  char lastError[256];
};

#endif // MODULE_REGISTRY_H
