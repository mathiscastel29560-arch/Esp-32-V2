#include "attack_templates.h"
#include "logging_system.h"

// ============= ATTACK TEMPLATE LIBRARY IMPLEMENTATION =============

Attack* AttackTemplateLibrary::createScannerAttack(const char* name, uint32_t timeout) {
  ScannerTemplate* scanner = new ScannerTemplate(name);
  scanner->scanTimeout = timeout;
  return scanner;
}

Attack* AttackTemplateLibrary::createJammerAttack(const char* name, float power, uint32_t duration) {
  JammerTemplate* jammer = new JammerTemplate(name);
  jammer->transmitPower = power;
  jammer->jamDuration = duration;
  return jammer;
}

Attack* AttackTemplateLibrary::createCaptureAttack(const char* name, uint32_t timeout) {
  CaptureTemplate* capture = new CaptureTemplate(name);
  capture->captureTimeout = timeout;
  return capture;
}

Attack* AttackTemplateLibrary::createExploitAttack(const char* name, const char* target, uint32_t timeout) {
  ExploitTemplate* exploit = new ExploitTemplate(name);
  exploit->targetIdentifier = target;
  exploit->exploitTimeout = timeout;
  return exploit;
}

Attack* AttackTemplateLibrary::createWiFiScanner() {
  return createScannerAttack("WiFi Scanner", 10000);
}

Attack* AttackTemplateLibrary::createBLEScanner() {
  return createScannerAttack("BLE Scanner", 15000);
}

Attack* AttackTemplateLibrary::createRFScanner() {
  return createScannerAttack("RF Scanner", 20000);
}

Attack* AttackTemplateLibrary::createWiFiDeauth(const char* bssid) {
  JammerTemplate* deauth = new JammerTemplate("WiFi Deauth");
  deauth->transmitPower = 20.0f;
  deauth->jamDuration = 5000;
  return deauth;
}

Attack* AttackTemplateLibrary::createBLEDisconnect(const char* address) {
  ExploitTemplate* exploit = new ExploitTemplate("BLE Disconnect");
  if (address) exploit->targetIdentifier = address;
  exploit->exploitTimeout = 3000;
  return exploit;
}

Attack* AttackTemplateLibrary::createWiFiHandshakeCapture() {
  return createCaptureAttack("WiFi Handshake Capture", 30000);
}

Attack* AttackTemplateLibrary::createBLEAdvertisementCapture() {
  return createCaptureAttack("BLE Advertisement Capture", 20000);
}

// ============= ATTACK WORKFLOW IMPLEMENTATION =============

void AttackWorkflow::addStep(Attack* attack, uint32_t duration, bool wait) {
  if (steps.size() >= MAX_STEPS) return;

  WorkflowStep* step = new WorkflowStep();
  step->attack = attack;
  step->duration = duration;
  step->waitForCompletion = wait;
  steps.push_back(step);

  char msg[128];
  snprintf(msg, 127, "Step added to %s", workflowName);
  Logger::getInstance().info("Workflow", msg);
}

void AttackWorkflow::removeStep(uint16_t index) {
  if (index < steps.size()) {
    delete steps[index];
    steps.erase(steps.begin() + index);
  }
}

bool AttackWorkflow::start() {
  if (steps.empty()) return false;

  running = true;
  currentStep = 0;
  startTime = millis();

  if (steps[0]->attack) {
    steps[0]->attack->begin();
    steps[0]->attack->start();
  }

  char msg[128];
  snprintf(msg, 127, "Workflow started: %s", workflowName);
  Logger::getInstance().info("Workflow", msg);

  return true;
}

bool AttackWorkflow::stop() {
  running = false;

  if (currentStep < steps.size() && steps[currentStep]->attack) {
    steps[currentStep]->attack->stop();
  }

  Logger::getInstance().info("Workflow", "Workflow stopped");
  return true;
}

bool AttackWorkflow::pause() {
  if (currentStep < steps.size() && steps[currentStep]->attack) {
    // TODO: Implement pause
    return true;
  }
  return false;
}

bool AttackWorkflow::resume() {
  if (currentStep < steps.size() && steps[currentStep]->attack) {
    // TODO: Implement resume
    return true;
  }
  return false;
}

bool AttackWorkflow::nextStep() {
  if (!running) return false;

  // Stop current step
  if (currentStep < steps.size() && steps[currentStep]->attack) {
    steps[currentStep]->attack->stop();
  }

  // Move to next step
  currentStep++;
  if (currentStep >= steps.size()) {
    running = false;

    char msg[128];
    snprintf(msg, 127, "Workflow completed: %s", workflowName);
    Logger::getInstance().info("Workflow", msg);
    return false;
  }

  // Start next step
  if (steps[currentStep]->attack) {
    steps[currentStep]->attack->begin();
    steps[currentStep]->attack->start();
  }

  return true;
}

Attack* AttackWorkflow::getCurrentAttack() {
  if (currentStep < steps.size()) {
    return steps[currentStep]->attack;
  }
  return nullptr;
}

uint8_t AttackWorkflow::getProgress() const {
  if (steps.empty()) return 0;
  return (uint8_t)((currentStep * 100) / steps.size());
}

void AttackWorkflow::printWorkflowInfo() {
  Serial.println("\n========== Workflow Info ==========");
  Serial.printf("Name: %s\n", workflowName);
  Serial.printf("Status: %s\n", running ? "RUNNING" : "STOPPED");
  Serial.printf("Current Step: %u/%u\n", currentStep + 1, (uint16_t)steps.size());
  Serial.printf("Progress: %u%%\n", getProgress());
  Serial.println("==================================\n");
}

// ============= PRESET WORKFLOWS IMPLEMENTATION =============

AttackWorkflow* PresetWorkflows::createWiFiReconWorkflow() {
  AttackWorkflow* wf = new AttackWorkflow("WiFi Recon");

  Attack* scan = AttackTemplateLibrary::getInstance().createWiFiScanner();
  wf->addStep(scan, 10000, true);

  Attack* handshake = AttackTemplateLibrary::getInstance().createWiFiHandshakeCapture();
  wf->addStep(handshake, 30000, true);

  return wf;
}

AttackWorkflow* PresetWorkflows::createWiFiPenetrationWorkflow() {
  AttackWorkflow* wf = new AttackWorkflow("WiFi Penetration");

  Attack* scan = AttackTemplateLibrary::getInstance().createWiFiScanner();
  wf->addStep(scan, 10000, true);

  Attack* capture = AttackTemplateLibrary::getInstance().createWiFiHandshakeCapture();
  wf->addStep(capture, 30000, true);

  Attack* deauth = AttackTemplateLibrary::getInstance().createWiFiDeauth();
  wf->addStep(deauth, 5000, true);

  return wf;
}

AttackWorkflow* PresetWorkflows::createWiFiJammingWorkflow() {
  AttackWorkflow* wf = new AttackWorkflow("WiFi Jamming");

  Attack* scan = AttackTemplateLibrary::getInstance().createWiFiScanner();
  wf->addStep(scan, 10000, true);

  Attack* jam = AttackTemplateLibrary::getInstance().createWiFiDeauth();
  wf->addStep(jam, 10000, true);

  return wf;
}

AttackWorkflow* PresetWorkflows::createBLEReconWorkflow() {
  AttackWorkflow* wf = new AttackWorkflow("BLE Recon");

  Attack* scan = AttackTemplateLibrary::getInstance().createBLEScanner();
  wf->addStep(scan, 15000, true);

  Attack* capture = AttackTemplateLibrary::getInstance().createBLEAdvertisementCapture();
  wf->addStep(capture, 20000, true);

  return wf;
}

AttackWorkflow* PresetWorkflows::createBLEDisruptionWorkflow() {
  AttackWorkflow* wf = new AttackWorkflow("BLE Disruption");

  Attack* scan = AttackTemplateLibrary::getInstance().createBLEScanner();
  wf->addStep(scan, 15000, true);

  Attack* disconnect = AttackTemplateLibrary::getInstance().createBLEDisconnect();
  wf->addStep(disconnect, 5000, true);

  return wf;
}

AttackWorkflow* PresetWorkflows::createMultiBandScanWorkflow() {
  AttackWorkflow* wf = new AttackWorkflow("Multi-Band Scan");

  Attack* wifiScan = AttackTemplateLibrary::getInstance().createWiFiScanner();
  wf->addStep(wifiScan, 10000, true);

  Attack* bleScan = AttackTemplateLibrary::getInstance().createBLEScanner();
  wf->addStep(bleScan, 15000, true);

  Attack* rfScan = AttackTemplateLibrary::getInstance().createRFScanner();
  wf->addStep(rfScan, 20000, true);

  return wf;
}

AttackWorkflow* PresetWorkflows::createCoordinatedAttackWorkflow() {
  AttackWorkflow* wf = new AttackWorkflow("Coordinated Attack");

  Attack* wifiDeauth = AttackTemplateLibrary::getInstance().createWiFiDeauth();
  wf->addStep(wifiDeauth, 5000, true);

  Attack* bleDisconnect = AttackTemplateLibrary::getInstance().createBLEDisconnect();
  wf->addStep(bleDisconnect, 5000, true);

  return wf;
}
