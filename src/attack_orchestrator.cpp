#include "attack_orchestrator.h"
#include "logging_system.h"

// ============= RESOURCE MANAGER IMPLEMENTATION =============

ResourceManager::ResourceManager() {}

bool ResourceManager::allocateResource(ResourceType resource, Attack* requester) {
  if (!isResourceAvailable(resource)) {
    Logger::getInstance().warn("Resources", "Resource not available");
    return false;
  }

  if (allocations.size() >= MAX_ALLOCATIONS) {
    Logger::getInstance().error("Resources", "Max allocations reached");
    return false;
  }

  ResourceAllocation* alloc = new ResourceAllocation();
  alloc->resource = resource;
  alloc->owner = requester;
  alloc->allocationTime = millis();
  allocations.push_back(alloc);

  char msg[128];
  snprintf(msg, 127, "Resource %u allocated to %s", (uint16_t)resource, requester->getName());
  Logger::getInstance().info("Resources", msg);

  return true;
}

bool ResourceManager::releaseResource(ResourceType resource, Attack* requester) {
  for (uint16_t i = 0; i < allocations.size(); i++) {
    if (allocations[i]->resource == resource && allocations[i]->owner == requester) {
      delete allocations[i];
      allocations.erase(allocations.begin() + i);

      char msg[128];
      snprintf(msg, 127, "Resource %u released by %s", (uint16_t)resource, requester->getName());
      Logger::getInstance().info("Resources", msg);
      return true;
    }
  }
  return false;
}

bool ResourceManager::isResourceAvailable(ResourceType resource) {
  for (auto* alloc : allocations) {
    if (alloc->resource == resource) {
      return false;
    }
    if (checkConflict(alloc->resource, resource)) {
      return false;
    }
  }
  return true;
}

uint16_t ResourceManager::getResourceMask(const char* attackName) {
  // Return typical resource bitmask for attack type
  if (strstr(attackName, "WiFi")) return (uint16_t)ResourceType::WiFi | (uint16_t)ResourceType::SPI;
  if (strstr(attackName, "BLE")) return (uint16_t)ResourceType::BLE;
  if (strstr(attackName, "NFC")) return (uint16_t)ResourceType::I2C | (uint16_t)ResourceType::NFC;
  if (strstr(attackName, "433")) return (uint16_t)ResourceType::RF_433 | (uint16_t)ResourceType::SPI;
  if (strstr(attackName, "2400")) return (uint16_t)ResourceType::RF_2400 | (uint16_t)ResourceType::SPI;
  return 0;
}

void ResourceManager::printResourceStatus() {
  Serial.println("\n========== Resource Status ==========");
  Serial.printf("Allocations: %u/%u\n", (uint16_t)allocations.size(), MAX_ALLOCATIONS);
  for (auto* alloc : allocations) {
    Serial.printf("  Resource %u owned by %s (%u ms)\n",
      (uint16_t)alloc->resource, alloc->owner->getName(),
      millis() - alloc->allocationTime);
  }
  Serial.println("====================================\n");
}

bool ResourceManager::checkConflict(ResourceType r1, ResourceType r2) {
  // SPI conflicts: RF_433, RF_2400, DISPLAY
  if ((r1 == ResourceType::RF_433 || r1 == ResourceType::RF_2400) &&
      (r2 == ResourceType::RF_433 || r2 == ResourceType::RF_2400)) {
    return true;
  }

  // I2C conflicts
  if ((r1 == ResourceType::I2C || r1 == ResourceType::NFC) &&
      (r2 == ResourceType::I2C || r2 == ResourceType::NFC)) {
    return true;
  }

  return false;
}

// ============= ATTACK ORCHESTRATOR IMPLEMENTATION =============

AttackOrchestrator::AttackOrchestrator() : lastUpdateTime(0) {
  Logger::getInstance().info("Orchestrator", "Initialized");
}

void AttackOrchestrator::begin() {
  Logger::getInstance().info("Orchestrator", "Started");
}

void AttackOrchestrator::update() {
  uint32_t now = millis();
  if (now - lastUpdateTime < 50) return; // Update every 50ms
  lastUpdateTime = now;

  // Update scheduled attacks
  updateScheduledAttacks();

  // Update active attacks
  for (auto* context : activeAttacks) {
    if (context->attack && context->attack->isActive()) {
      context->attack->update();

      // Check timeout
      if (context->maxDuration > 0) {
        if (now - context->startTime > context->maxDuration) {
          stopAttack(context->attack);
        }
      }
    }
  }

  // Handle timeouts
  handleTimeoutAttacks();

  // Aggregate results
  aggregateResults();
}

void AttackOrchestrator::stop() {
  // Stop all active attacks
  for (auto* context : activeAttacks) {
    if (context->attack) {
      context->attack->stop();
      releaseAttackResources(context->attack);
    }
  }

  Logger::getInstance().info("Orchestrator", "Stopped");
}

bool AttackOrchestrator::registerAttack(Attack* attack) {
  if (!attack) return false;

  for (auto* a : registeredAttacks) {
    if (a == attack) {
      Logger::getInstance().warn("Orchestrator", "Attack already registered");
      return false;
    }
  }

  registeredAttacks.push_back(attack);

  char msg[128];
  snprintf(msg, 127, "Attack registered: %s", attack->getName());
  Logger::getInstance().info("Orchestrator", msg);

  return true;
}

bool AttackOrchestrator::unregisterAttack(Attack* attack) {
  for (uint16_t i = 0; i < registeredAttacks.size(); i++) {
    if (registeredAttacks[i] == attack) {
      registeredAttacks.erase(registeredAttacks.begin() + i);

      char msg[128];
      snprintf(msg, 127, "Attack unregistered: %s", attack->getName());
      Logger::getInstance().info("Orchestrator", msg);
      return true;
    }
  }
  return false;
}

bool AttackOrchestrator::startAttack(Attack* attack, uint32_t maxDuration) {
  if (!attack || activeAttacks.size() >= MAX_CONCURRENT) {
    return false;
  }

  // Check resource availability
  if (!checkResourceAvailability(attack)) {
    Logger::getInstance().warn("Orchestrator", "Resources not available");
    return false;
  }

  // Allocate resources
  if (!allocateAttackResources(attack)) {
    Logger::getInstance().error("Orchestrator", "Failed to allocate resources");
    return false;
  }

  // Start attack
  if (!attack->start()) {
    releaseAttackResources(attack);
    return false;
  }

  // Create execution context
  ExecutionContext* context = new ExecutionContext();
  context->attack = attack;
  context->priority = AttackPriority::NORMAL;
  context->startTime = millis();
  context->maxDuration = maxDuration;
  context->memoryAllocated = ESP.getFreeHeap();

  activeAttacks.push_back(context);
  stats.totalExecuted++;

  char msg[128];
  snprintf(msg, 127, "Attack started: %s", attack->getName());
  Logger::getInstance().info("Orchestrator", msg);

  return true;
}

bool AttackOrchestrator::pauseAttack(Attack* attack) {
  ExecutionContext* context = getContext(attack);
  if (!context) return false;

  context->paused = true;

  char msg[128];
  snprintf(msg, 127, "Attack paused: %s", attack->getName());
  Logger::getInstance().info("Orchestrator", msg);

  return true;
}

bool AttackOrchestrator::resumeAttack(Attack* attack) {
  ExecutionContext* context = getContext(attack);
  if (!context) return false;

  context->paused = false;

  char msg[128];
  snprintf(msg, 127, "Attack resumed: %s", attack->getName());
  Logger::getInstance().info("Orchestrator", msg);

  return true;
}

bool AttackOrchestrator::stopAttack(Attack* attack) {
  ExecutionContext* context = getContext(attack);
  if (!context) return false;

  attack->stop();
  releaseAttackResources(attack);

  stats.totalDuration += millis() - context->startTime;

  if (attack->getStatus() == AttackStatus::SUCCESS) {
    stats.successfulCount++;
  } else {
    stats.failedCount++;
  }

  // Remove from active attacks
  for (uint16_t i = 0; i < activeAttacks.size(); i++) {
    if (activeAttacks[i]->attack == attack) {
      delete activeAttacks[i];
      activeAttacks.erase(activeAttacks.begin() + i);
      break;
    }
  }

  char msg[128];
  snprintf(msg, 127, "Attack stopped: %s", attack->getName());
  Logger::getInstance().info("Orchestrator", msg);

  return true;
}

bool AttackOrchestrator::startMultipleAttacks(Attack** attacks, uint16_t count,
                                             AttackPriority* priorities) {
  for (uint16_t i = 0; i < count; i++) {
    AttackPriority priority = priorities ? priorities[i] : AttackPriority::NORMAL;

    if (!startAttack(attacks[i], 0)) {
      Logger::getInstance().error("Orchestrator", "Failed to start attack");
      return false;
    }
  }

  return true;
}

Attack* AttackOrchestrator::getActiveAttack(uint16_t index) {
  if (index < activeAttacks.size()) {
    return activeAttacks[index]->attack;
  }
  return nullptr;
}

bool AttackOrchestrator::scheduleAttack(const AttackSchedule& schedule) {
  if (scheduledAttacks.size() >= MAX_SCHEDULED) {
    return false;
  }

  AttackSchedule* sched = new AttackSchedule(schedule);
  scheduledAttacks.push_back(sched);

  return true;
}

bool AttackOrchestrator::setPriority(Attack* attack, AttackPriority priority) {
  ExecutionContext* context = getContext(attack);
  if (!context) return false;

  context->priority = priority;
  return true;
}

AttackPriority AttackOrchestrator::getPriority(Attack* attack) {
  ExecutionContext* context = getContext(attack);
  if (!context) return AttackPriority::NORMAL;

  return context->priority;
}

bool AttackOrchestrator::checkResourceAvailability(Attack* attack) {
  uint16_t mask = ResourceManager::getInstance().getResourceMask(attack->getName());

  // Check each required resource
  for (int i = 0; i < 12; i++) {
    if (mask & (1 << i)) {
      ResourceType res = (ResourceType)(1 << i);
      if (!ResourceManager::getInstance().isResourceAvailable(res)) {
        return false;
      }
    }
  }

  return true;
}

bool AttackOrchestrator::allocateAttackResources(Attack* attack) {
  uint16_t mask = ResourceManager::getInstance().getResourceMask(attack->getName());

  for (int i = 0; i < 12; i++) {
    if (mask & (1 << i)) {
      ResourceType res = (ResourceType)(1 << i);
      if (!ResourceManager::getInstance().allocateResource(res, attack)) {
        return false;
      }
    }
  }

  return true;
}

void AttackOrchestrator::releaseAttackResources(Attack* attack) {
  uint16_t mask = ResourceManager::getInstance().getResourceMask(attack->getName());

  for (int i = 0; i < 12; i++) {
    if (mask & (1 << i)) {
      ResourceType res = (ResourceType)(1 << i);
      ResourceManager::getInstance().releaseResource(res, attack);
    }
  }
}

uint32_t AttackOrchestrator::getTotalResultCount() {
  uint32_t count = 0;
  for (auto* context : activeAttacks) {
    if (context->attack) {
      count += context->attack->getResultCount();
    }
  }
  return count;
}

AttackResult* AttackOrchestrator::getAggregatedResult(uint16_t index) {
  if (index < aggregatedResults.size()) {
    return aggregatedResults[index];
  }
  return nullptr;
}

void AttackOrchestrator::clearAggregatedResults() {
  for (auto* result : aggregatedResults) {
    delete result;
  }
  aggregatedResults.clear();
}

void AttackOrchestrator::printOrchestrationReport() {
  Serial.println("\n========== Orchestration Report ==========");
  Serial.printf("Active Attacks: %u\n", (uint16_t)activeAttacks.size());
  Serial.printf("Total Executed: %u\n", stats.totalExecuted);
  Serial.printf("Successful: %u, Failed: %u\n", stats.successfulCount, stats.failedCount);
  Serial.printf("Total Duration: %u ms\n", stats.totalDuration);
  Serial.printf("Conflicts Detected: %u\n", stats.conflictCount);
  Serial.println("=========================================\n");
}

bool AttackOrchestrator::detectConflicts(Attack* a1, Attack* a2) {
  uint16_t mask1 = ResourceManager::getInstance().getResourceMask(a1->getName());
  uint16_t mask2 = ResourceManager::getInstance().getResourceMask(a2->getName());

  return (mask1 & mask2) != 0;
}

uint16_t AttackOrchestrator::detectAllConflicts() {
  uint16_t conflictCount = 0;

  for (uint16_t i = 0; i < activeAttacks.size(); i++) {
    for (uint16_t j = i + 1; j < activeAttacks.size(); j++) {
      if (detectConflicts(activeAttacks[i]->attack, activeAttacks[j]->attack)) {
        conflictCount++;
        stats.conflictCount++;
      }
    }
  }

  return conflictCount;
}

ExecutionContext* AttackOrchestrator::getContext(Attack* attack) {
  for (auto* context : activeAttacks) {
    if (context->attack == attack) {
      return context;
    }
  }
  return nullptr;
}

void AttackOrchestrator::updateScheduledAttacks() {
  uint32_t now = millis();

  for (uint16_t i = 0; i < scheduledAttacks.size(); i++) {
    AttackSchedule* sched = scheduledAttacks[i];
    // TODO: Implement scheduled attack execution
  }
}

void AttackOrchestrator::handleTimeoutAttacks() {
  // Already handled in update()
}

void AttackOrchestrator::aggregateResults() {
  clearAggregatedResults();

  for (auto* context : activeAttacks) {
    if (context->attack) {
      for (uint16_t i = 0; i < context->attack->getResultCount(); i++) {
        AttackResult* result = context->attack->getResult(i);
        if (result && aggregatedResults.size() < MAX_AGGREGATED) {
          AttackResult* copy = new AttackResult(*result);
          aggregatedResults.push_back(copy);
        }
      }
    }
  }
}

// ============= ATTACK SEQUENCE IMPLEMENTATION =============

void AttackSequence::addStep(Attack* attack, uint32_t duration, AttackPriority priority) {
  if (steps.size() >= MAX_STEPS) return;

  ExecutionContext* context = new ExecutionContext();
  context->attack = attack;
  context->priority = priority;
  context->maxDuration = duration;
  steps.push_back(context);
}

void AttackSequence::removeStep(uint16_t index) {
  if (index < steps.size()) {
    delete steps[index];
    steps.erase(steps.begin() + index);
  }
}

bool AttackSequence::start() {
  if (steps.empty()) return false;
  running = true;
  startTime = millis();
  return true;
}

bool AttackSequence::stop() {
  running = false;
  if (currentStep < steps.size() && steps[currentStep]->attack) {
    steps[currentStep]->attack->stop();
  }
  return true;
}

bool AttackSequence::pauseStep() {
  if (currentStep < steps.size() && steps[currentStep]->attack) {
    return AttackOrchestrator::getInstance().pauseAttack(steps[currentStep]->attack);
  }
  return false;
}

bool AttackSequence::resumeStep() {
  if (currentStep < steps.size() && steps[currentStep]->attack) {
    return AttackOrchestrator::getInstance().resumeAttack(steps[currentStep]->attack);
  }
  return false;
}

bool AttackSequence::nextStep() {
  if (currentStep < steps.size() && steps[currentStep]->attack) {
    steps[currentStep]->attack->stop();
  }

  currentStep++;
  if (currentStep >= steps.size()) {
    running = false;
    return false;
  }

  return true;
}

Attack* AttackSequence::getCurrentAttack() {
  if (currentStep < steps.size()) {
    return steps[currentStep]->attack;
  }
  return nullptr;
}

uint32_t AttackSequence::getSequenceDuration() const {
  uint32_t total = 0;
  for (auto* step : steps) {
    total += step->maxDuration;
  }
  return total;
}

uint32_t AttackSequence::getElapsedTime() const {
  if (!running) return 0;
  return millis() - startTime;
}
