#ifndef ATTACK_ORCHESTRATOR_H
#define ATTACK_ORCHESTRATOR_H

#include <Arduino.h>
#include <vector>
#include "attack_framework.h"
#include "config_manager.h"

// ============= ATTACK EXECUTION CONTEXT =============

enum class AttackPriority {
  LOW = 0,
  NORMAL = 1,
  HIGH = 2,
  CRITICAL = 3
};

struct ExecutionContext {
  Attack* attack;
  AttackPriority priority;
  uint32_t startTime;
  uint32_t maxDuration;
  bool paused;
  uint16_t resourcesMask;      // Bitmask of required resources
  uint32_t memoryAllocated;

  ExecutionContext() : attack(nullptr), priority(AttackPriority::NORMAL),
                       startTime(0), maxDuration(0), paused(false),
                       resourcesMask(0), memoryAllocated(0) {}
};

struct AttackSchedule {
  const char* attackName;
  uint32_t startDelay;        // ms before starting
  uint32_t duration;          // Max execution time
  AttackPriority priority;

  AttackSchedule() : attackName(""), startDelay(0), duration(0),
                     priority(AttackPriority::NORMAL) {}
};

struct OrchestrationStats {
  uint16_t totalExecuted;
  uint16_t successfulCount;
  uint16_t failedCount;
  uint32_t totalDuration;
  uint32_t memoryPeakUsage;
  uint16_t conflictCount;

  OrchestrationStats() : totalExecuted(0), successfulCount(0), failedCount(0),
                         totalDuration(0), memoryPeakUsage(0), conflictCount(0) {}
};

// ============= RESOURCE MANAGEMENT =============

enum class ResourceType {
  GPIO = 0x01,
  I2C = 0x02,
  SPI = 0x04,
  UART = 0x08,
  WiFi = 0x10,
  BLE = 0x20,
  RF_433 = 0x40,
  RF_2400 = 0x80,
  RF_868 = 0x100,
  NFC = 0x200,
  DISPLAY = 0x400,
  PSRAM = 0x800
};

class ResourceManager {
public:
  static ResourceManager& getInstance() {
    static ResourceManager instance;
    return instance;
  }

  bool allocateResource(ResourceType resource, Attack* requester);
  bool releaseResource(ResourceType resource, Attack* requester);
  bool isResourceAvailable(ResourceType resource);
  uint16_t getResourceMask(const char* attackName);

  void printResourceStatus();

private:
  ResourceManager();

  struct ResourceAllocation {
    ResourceType resource;
    Attack* owner;
    uint32_t allocationTime;
  };

  std::vector<ResourceAllocation*> allocations;
  static const uint16_t MAX_ALLOCATIONS = 20;

  bool checkConflict(ResourceType r1, ResourceType r2);
};

// ============= ATTACK ORCHESTRATOR =============

class AttackOrchestrator {
public:
  static AttackOrchestrator& getInstance() {
    static AttackOrchestrator instance;
    return instance;
  }

  // Lifecycle
  void begin();
  void update();
  void stop();

  // Attack management
  bool registerAttack(Attack* attack);
  bool unregisterAttack(Attack* attack);
  bool startAttack(Attack* attack, uint32_t maxDuration = 0);
  bool pauseAttack(Attack* attack);
  bool resumeAttack(Attack* attack);
  bool stopAttack(Attack* attack);

  // Concurrent execution
  bool startMultipleAttacks(Attack** attacks, uint16_t count,
                           AttackPriority* priorities = nullptr);
  uint16_t getActiveAttackCount() const { return activeAttacks.size(); }
  Attack* getActiveAttack(uint16_t index);

  // Scheduling
  bool scheduleAttack(const AttackSchedule& schedule);
  uint16_t getScheduledCount() const { return scheduledAttacks.size(); }

  // Priority management
  bool setPriority(Attack* attack, AttackPriority priority);
  AttackPriority getPriority(Attack* attack);

  // Resource coordination
  bool checkResourceAvailability(Attack* attack);
  bool allocateAttackResources(Attack* attack);
  void releaseAttackResources(Attack* attack);

  // Results aggregation
  uint32_t getTotalResultCount();
  AttackResult* getAggregatedResult(uint16_t index);
  void clearAggregatedResults();

  // Statistics
  OrchestrationStats* getStats() { return &stats; }
  void printOrchestrationReport();

  // Conflict detection
  bool detectConflicts(Attack* a1, Attack* a2);
  uint16_t detectAllConflicts();

private:
  AttackOrchestrator();

  std::vector<ExecutionContext*> activeAttacks;
  std::vector<Attack*> registeredAttacks;
  std::vector<AttackSchedule*> scheduledAttacks;
  std::vector<AttackResult*> aggregatedResults;

  OrchestrationStats stats;
  uint32_t lastUpdateTime;

  static const uint16_t MAX_CONCURRENT = 4;
  static const uint16_t MAX_SCHEDULED = 10;
  static const uint16_t MAX_AGGREGATED = 1000;

  ExecutionContext* getContext(Attack* attack);
  void updateScheduledAttacks();
  void handleTimeoutAttacks();
  void aggregateResults();
};

// ============= ATTACK SEQUENCE (Multi-step) =============

class AttackSequence {
public:
  AttackSequence(const char* name) : sequenceName(name), currentStep(0),
                                     running(false) {
    memset(sequenceName, 0, 64);
    strncpy(sequenceName, name, 63);
  }

  void addStep(Attack* attack, uint32_t duration, AttackPriority priority = AttackPriority::NORMAL);
  void removeStep(uint16_t index);
  uint16_t getStepCount() const { return steps.size(); }

  bool start();
  bool stop();
  bool pauseStep();
  bool resumeStep();
  bool nextStep();

  Attack* getCurrentAttack();
  uint16_t getCurrentStep() const { return currentStep; }
  bool isRunning() const { return running; }

  uint32_t getSequenceDuration() const;
  uint32_t getElapsedTime() const;

private:
  char sequenceName[64];
  std::vector<ExecutionContext*> steps;
  uint16_t currentStep;
  bool running;
  uint32_t startTime;

  static const uint16_t MAX_STEPS = 20;
};

#endif // ATTACK_ORCHESTRATOR_H
