#ifndef ATTACK_TEMPLATES_H
#define ATTACK_TEMPLATES_H

#include <Arduino.h>
#include <vector>
#include "attack_framework.h"

// ============= TEMPLATE BASE CLASSES =============

// Scanner template - for reconnaissance attacks
class ScannerTemplate : public Attack {
protected:
  uint16_t itemsFound;
  uint32_t scanTimeout;

public:
  ScannerTemplate(const char* name) : Attack(name), itemsFound(0), scanTimeout(10000) {}

  bool start() override {
    Attack::start();
    itemsFound = 0;
    return true;
  }

  void update() override {
    if (isRunning && millis() - startTime > scanTimeout) {
      setStatus(AttackStatus::SUCCESS);
      isRunning = false;
    }
  }
};

// Jammer template - for disruption/jamming
class JammerTemplate : public Attack {
protected:
  uint16_t packetsTransmitted;
  float transmitPower;
  uint32_t jamDuration;

public:
  JammerTemplate(const char* name) : Attack(name), packetsTransmitted(0),
                                      transmitPower(20.0f), jamDuration(10000) {}

  bool start() override {
    Attack::start();
    packetsTransmitted = 0;
    return true;
  }

  void update() override {
    if (isRunning && millis() - startTime > jamDuration) {
      setStatus(AttackStatus::SUCCESS);
      isRunning = false;
    }
  }
};

// Capture template - for data capture/extraction
class CaptureTemplate : public Attack {
protected:
  uint32_t bytesCapured;
  uint32_t captureTimeout;

public:
  CaptureTemplate(const char* name) : Attack(name), bytesCapured(0), captureTimeout(30000) {}

  bool start() override {
    Attack::start();
    bytesCapured = 0;
    return true;
  }

  void update() override {
    if (isRunning && millis() - startTime > captureTimeout) {
      if (bytesCapured > 0) {
        setStatus(AttackStatus::SUCCESS);
      } else {
        setStatus(AttackStatus::FAILED);
      }
      isRunning = false;
    }
  }
};

// Exploit template - for targeted exploitation
class ExploitTemplate : public Attack {
protected:
  const char* targetIdentifier;
  uint16_t attemptCount;
  uint32_t exploitTimeout;
  bool vulnerable;

public:
  ExploitTemplate(const char* name) : Attack(name), targetIdentifier(""),
                                       attemptCount(0), exploitTimeout(15000),
                                       vulnerable(false) {}

  bool start() override {
    Attack::start();
    attemptCount = 0;
    return true;
  }

  void update() override {
    if (isRunning && millis() - startTime > exploitTimeout) {
      if (vulnerable && attemptCount > 0) {
        setStatus(AttackStatus::SUCCESS);
      } else {
        setStatus(AttackStatus::FAILED);
      }
      isRunning = false;
    }
  }
};

// ============= TEMPLATE LIBRARY =============

class AttackTemplateLibrary {
public:
  static AttackTemplateLibrary& getInstance() {
    static AttackTemplateLibrary instance;
    return instance;
  }

  // Template creation helpers
  Attack* createScannerAttack(const char* name, uint32_t timeout = 10000);
  Attack* createJammerAttack(const char* name, float power = 20.0f, uint32_t duration = 10000);
  Attack* createCaptureAttack(const char* name, uint32_t timeout = 30000);
  Attack* createExploitAttack(const char* name, const char* target, uint32_t timeout = 15000);

  // Preset templates
  Attack* createWiFiScanner();
  Attack* createBLEScanner();
  Attack* createRFScanner();

  Attack* createWiFiDeauth(const char* bssid = nullptr);
  Attack* createBLEDisconnect(const char* address = nullptr);

  Attack* createWiFiHandshakeCapture();
  Attack* createBLEAdvertisementCapture();

  uint16_t getTemplateCount() const { return 10; }

private:
  AttackTemplateLibrary() {}
};

// ============= WORKFLOW BUILDER =============

struct WorkflowStep {
  Attack* attack;
  uint32_t duration;
  bool waitForCompletion;

  WorkflowStep() : attack(nullptr), duration(0), waitForCompletion(true) {}
};

class AttackWorkflow {
public:
  AttackWorkflow(const char* name) : workflowName(name), currentStep(0), running(false) {
    memset(workflowName, 0, 64);
    strncpy(workflowName, name, 63);
  }

  void addStep(Attack* attack, uint32_t duration = 0, bool wait = true);
  void removeStep(uint16_t index);

  bool start();
  bool stop();
  bool pause();
  bool resume();

  bool nextStep();
  Attack* getCurrentAttack();
  uint16_t getCurrentStep() const { return currentStep; }
  uint16_t getTotalSteps() const { return steps.size(); }

  bool isRunning() const { return running; }
  uint8_t getProgress() const;

  void printWorkflowInfo();

private:
  char workflowName[64];
  std::vector<WorkflowStep*> steps;
  uint16_t currentStep;
  bool running;
  uint32_t startTime;

  static const uint16_t MAX_STEPS = 20;
};

// ============= PRESET WORKFLOWS =============

class PresetWorkflows {
public:
  static PresetWorkflows& getInstance() {
    static PresetWorkflows instance;
    return instance;
  }

  // WiFi workflows
  AttackWorkflow* createWiFiReconWorkflow();
  AttackWorkflow* createWiFiPenetrationWorkflow();
  AttackWorkflow* createWiFiJammingWorkflow();

  // BLE workflows
  AttackWorkflow* createBLEReconWorkflow();
  AttackWorkflow* createBLEDisruptionWorkflow();

  // Multi-band workflows
  AttackWorkflow* createMultiBandScanWorkflow();
  AttackWorkflow* createCoordinatedAttackWorkflow();

private:
  PresetWorkflows() {}
};

#endif // ATTACK_TEMPLATES_H
