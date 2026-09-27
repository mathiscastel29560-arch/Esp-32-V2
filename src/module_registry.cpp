#include "module_registry.h"
#include "logging_system.h"

// ============= MODULE REGISTRY IMPLEMENTATION =============

bool ModuleRegistry::registerModule(const ModuleInfo& info, ModuleInitFunc init,
                                   ModuleCleanupFunc cleanup, AttackFactory factory) {
  // Validate module
  ModuleValidator::getInstance().validateModule(info);

  // Check if already registered
  for (auto* m : modules) {
    if (strcmp(m->info.name, info.name) == 0) {
      Logger::getInstance().warn("ModuleRegistry", "Module already registered");
      return false;
    }
  }

  if (modules.size() >= MAX_MODULES) {
    Logger::getInstance().error("ModuleRegistry", "Max modules reached");
    return false;
  }

  LoadedModule* module = new LoadedModule();
  module->info = info;
  module->init = init;
  module->cleanup = cleanup;
  module->factory = factory;

  modules.push_back(module);

  char msg[128];
  snprintf(msg, 127, "Module registered: %s v%s", info.name, info.version);
  Logger::getInstance().info("ModuleRegistry", msg);

  return true;
}

bool ModuleRegistry::unregisterModule(const char* moduleName) {
  for (uint16_t i = 0; i < modules.size(); i++) {
    if (strcmp(modules[i]->info.name, moduleName) == 0) {
      if (modules[i]->info.loaded) {
        unloadModule(moduleName);
      }
      delete modules[i];
      modules.erase(modules.begin() + i);

      char msg[128];
      snprintf(msg, 127, "Module unregistered: %s", moduleName);
      Logger::getInstance().info("ModuleRegistry", msg);
      return true;
    }
  }
  return false;
}

bool ModuleRegistry::loadModule(const char* moduleName) {
  LoadedModule* module = nullptr;
  for (auto* m : modules) {
    if (strcmp(m->info.name, moduleName) == 0) {
      module = m;
      break;
    }
  }

  if (!module) return false;
  if (module->info.loaded) return true;

  // Check memory availability
  if (getTotalMemoryUsage() + module->info.requiredMemory > MAX_MEMORY) {
    Logger::getInstance().error("ModuleRegistry", "Insufficient memory");
    return false;
  }

  // Check dependencies
  if (!checkDependencies(moduleName)) {
    Logger::getInstance().error("ModuleRegistry", "Dependency check failed");
    return false;
  }

  // Initialize module
  if (module->init) {
    module->init();
  }

  module->info.loaded = true;
  module->info.loadTime = millis();

  char msg[128];
  snprintf(msg, 127, "Module loaded: %s", moduleName);
  Logger::getInstance().info("ModuleRegistry", msg);

  return true;
}

bool ModuleRegistry::unloadModule(const char* moduleName) {
  for (auto* m : modules) {
    if (strcmp(m->info.name, moduleName) == 0) {
      if (!m->info.loaded) return true;

      // Cleanup module
      if (m->cleanup) {
        m->cleanup();
      }

      m->info.loaded = false;

      char msg[128];
      snprintf(msg, 127, "Module unloaded: %s", moduleName);
      Logger::getInstance().info("ModuleRegistry", msg);
      return true;
    }
  }
  return false;
}

bool ModuleRegistry::reloadModule(const char* moduleName) {
  return unloadModule(moduleName) && loadModule(moduleName);
}

ModuleInfo* ModuleRegistry::getModuleInfo(const char* moduleName) {
  for (auto* m : modules) {
    if (strcmp(m->info.name, moduleName) == 0) {
      return &m->info;
    }
  }
  return nullptr;
}

uint16_t ModuleRegistry::getAllModules(ModuleInfo* output, uint16_t maxCount) {
  uint16_t count = (modules.size() < maxCount) ? modules.size() : maxCount;
  for (uint16_t i = 0; i < count; i++) {
    output[i] = modules[i]->info;
  }
  return count;
}

uint16_t ModuleRegistry::getLoadedModules(ModuleInfo* output, uint16_t maxCount) {
  uint16_t count = 0;
  for (auto* m : modules) {
    if (m->info.loaded && count < maxCount) {
      output[count++] = m->info;
    }
  }
  return count;
}

Attack* ModuleRegistry::createAttackFromModule(const char* moduleName) {
  for (auto* m : modules) {
    if (strcmp(m->info.name, moduleName) == 0) {
      if (!m->info.loaded) {
        if (!loadModule(moduleName)) return nullptr;
      }

      if (m->factory) {
        return m->factory();
      }
    }
  }
  return nullptr;
}

bool ModuleRegistry::addDependency(const char* moduleName, const char* dependsOn, const char* version) {
  LoadedModule* module = nullptr;
  for (auto* m : modules) {
    if (strcmp(m->info.name, moduleName) == 0) {
      module = m;
      break;
    }
  }

  if (!module) return false;

  ModuleDependency* dep = new ModuleDependency();
  dep->moduleName = dependsOn;
  dep->requiredVersion = version;
  module->dependencies.push_back(dep);

  return true;
}

bool ModuleRegistry::checkDependencies(const char* moduleName) {
  LoadedModule* module = nullptr;
  for (auto* m : modules) {
    if (strcmp(m->info.name, moduleName) == 0) {
      module = m;
      break;
    }
  }

  if (!module) return false;

  for (auto* dep : module->dependencies) {
    bool found = false;
    for (auto* m : modules) {
      if (strcmp(m->info.name, dep->moduleName) == 0 && m->info.loaded) {
        found = true;
        dep->satisfied = true;
        break;
      }
    }
    if (!found) return false;
  }

  return true;
}

bool ModuleRegistry::resolveDependencies() {
  // Topological sort to load modules in dependency order
  // TODO: Implement dependency resolution
  return true;
}

uint16_t ModuleRegistry::getLoadedCount() const {
  uint16_t count = 0;
  for (auto* m : modules) {
    if (m->info.loaded) count++;
  }
  return count;
}

uint32_t ModuleRegistry::getTotalMemoryUsage() const {
  uint32_t total = 0;
  for (auto* m : modules) {
    if (m->info.loaded) {
      total += m->info.requiredMemory;
    }
  }
  return total;
}

bool ModuleRegistry::isModuleLoaded(const char* moduleName) const {
  for (auto* m : modules) {
    if (strcmp(m->info.name, moduleName) == 0) {
      return m->info.loaded;
    }
  }
  return false;
}

bool ModuleRegistry::canLoadModule(const char* moduleName) const {
  ModuleInfo* info = getModuleInfo(moduleName);
  if (!info) return false;

  return (getTotalMemoryUsage() + info->requiredMemory) <= MAX_MEMORY;
}

void ModuleRegistry::printModuleInfo() {
  Serial.println("\n========== Module Registry Info ==========");
  Serial.printf("Total Modules: %u\n", getModuleCount());
  Serial.printf("Loaded: %u\n", getLoadedCount());
  Serial.printf("Memory Used: %u KB\n", getTotalMemoryUsage());

  Serial.println("\nModules:");
  ModuleInfo info[MAX_MODULES];
  uint16_t count = getAllModules(info, MAX_MODULES);
  for (uint16_t i = 0; i < count; i++) {
    Serial.printf("  %s v%s - %s\n", info[i].name, info[i].version,
                  info[i].loaded ? "LOADED" : "Not loaded");
  }
  Serial.println("=========================================\n");
}

void ModuleRegistry::printDependencyTree() {
  Serial.println("\n========== Dependency Tree ==========");
  for (auto* m : modules) {
    Serial.printf("%s v%s\n", m->info.name, m->info.version);
    for (auto* dep : m->dependencies) {
      Serial.printf("  -> %s (%s)\n", dep->moduleName,
                    dep->satisfied ? "✓" : "✗");
    }
  }
  Serial.println("====================================\n");
}

// ============= MODULE FACTORY IMPLEMENTATION =============

void ModuleFactory::registerBuiltinModules() {
  Logger::getInstance().info("ModuleFactory", "Registering built-in modules");
  // TODO: Register all built-in attack modules
}

Attack* ModuleFactory::createAttack(const char* attackName) {
  return ModuleRegistry::getInstance().createAttackFromModule(attackName);
}

Attack* ModuleFactory::createAttackByCategory(const char* category) {
  // TODO: Create attack by category
  return nullptr;
}

uint16_t ModuleFactory::getAttackCount() const {
  return ModuleRegistry::getInstance().getModuleCount();
}

uint16_t ModuleFactory::getAttacksByCategory(const char* category, const char** output, uint16_t maxCount) {
  // TODO: Get attacks by category
  return 0;
}

// ============= MODULE VALIDATOR IMPLEMENTATION =============

bool ModuleValidator::validateModule(const ModuleInfo& info) {
  if (strlen(info.name) == 0) {
    strcpy(lastError, "Module name is empty");
    return false;
  }

  if (strlen(info.version) == 0) {
    strcpy(lastError, "Module version is empty");
    return false;
  }

  if (info.requiredMemory == 0) {
    strcpy(lastError, "Module requires memory specification");
    return false;
  }

  return true;
}

bool ModuleValidator::validateMemory(const ModuleInfo& info) {
  if (info.requiredMemory > MAX_MEMORY) {
    strcpy(lastError, "Module memory requirement exceeds max");
    return false;
  }
  return true;
}

bool ModuleValidator::validateDependencies(const ModuleInfo& info) {
  // TODO: Validate dependencies
  return true;
}

bool ModuleValidator::validateVersion(const char* version) {
  // Simple version validation: X.Y.Z format
  int dots = 0;
  for (int i = 0; version[i] != '\0'; i++) {
    if (version[i] == '.') dots++;
    else if (!isdigit(version[i])) return false;
  }
  return dots == 2;
}
