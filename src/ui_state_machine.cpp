#include "ui_state_machine.h"
#include "logging_system.h"

// ============= UI STATE HANDLERS IMPLEMENTATION =============

void MainMenuHandler::onEnter() {
  Logger::getInstance().info("UIState", "Entering MAIN_MENU");
  selectedItem = 0;
}

void MainMenuHandler::onExit() {
  Logger::getInstance().info("UIState", "Exiting MAIN_MENU");
}

void MainMenuHandler::update() {
  // Update main menu state
}

void MainMenuHandler::handleEvent(UIEvent event) {
  switch (event) {
    case UIEvent::BUTTON_DOWN:
      selectedItem++;
      break;
    case UIEvent::BUTTON_UP:
      if (selectedItem > 0) selectedItem--;
      break;
    case UIEvent::BUTTON_SELECT:
      // Trigger selected action
      break;
    default:
      break;
  }
}

void MainMenuHandler::render() {
  // Render main menu UI
}

void AttackMenuHandler::onEnter() {
  Logger::getInstance().info("UIState", "Entering ATTACK_MENU");
  selectedAttack = 0;
  scrollOffset = 0;
}

void AttackMenuHandler::onExit() {
  Logger::getInstance().info("UIState", "Exiting ATTACK_MENU");
}

void AttackMenuHandler::update() {
  // Update attack menu
}

void AttackMenuHandler::handleEvent(UIEvent event) {
  switch (event) {
    case UIEvent::BUTTON_DOWN:
      selectedAttack++;
      if (selectedAttack - scrollOffset > 3) {
        scrollOffset++;
      }
      break;
    case UIEvent::BUTTON_UP:
      if (selectedAttack > 0) selectedAttack--;
      if (selectedAttack < scrollOffset) {
        scrollOffset = selectedAttack;
      }
      break;
    default:
      break;
  }
}

void AttackMenuHandler::render() {
  // Render attack menu UI
}

void AttackRunningHandler::onEnter() {
  Logger::getInstance().info("UIState", "Entering ATTACK_RUNNING");
  attackStartTime = millis();
  progressPercent = 0;
}

void AttackRunningHandler::onExit() {
  Logger::getInstance().info("UIState", "Exiting ATTACK_RUNNING");
}

void AttackRunningHandler::update() {
  progressPercent = (millis() - attackStartTime) / 100;
  if (progressPercent > 100) progressPercent = 100;
}

void AttackRunningHandler::handleEvent(UIEvent event) {
  if (event == UIEvent::BUTTON_BACK) {
    // Request attack cancellation
  }
}

void AttackRunningHandler::render() {
  // Render progress bar and attack status
}

void AttackResultsHandler::onEnter() {
  Logger::getInstance().info("UIState", "Entering ATTACK_RESULTS");
  selectedResult = 0;
  resultCount = 0; // Should be set from attack
}

void AttackResultsHandler::onExit() {
  Logger::getInstance().info("UIState", "Exiting ATTACK_RESULTS");
}

void AttackResultsHandler::update() {
  // Update results display
}

void AttackResultsHandler::handleEvent(UIEvent event) {
  switch (event) {
    case UIEvent::BUTTON_DOWN:
      if (selectedResult < resultCount - 1) selectedResult++;
      break;
    case UIEvent::BUTTON_UP:
      if (selectedResult > 0) selectedResult--;
      break;
    default:
      break;
  }
}

void AttackResultsHandler::render() {
  // Render results UI
}

// ============= UI STATE MACHINE IMPLEMENTATION =============

UIStateMachine::UIStateMachine()
  : currentState(UIState::STARTUP),
    previousState(UIState::STARTUP),
    currentHandler(nullptr),
    lastUpdateTime(0),
    stateEntryTime(0) {

  Logger::getInstance().info("UIStateMachine", "Initialized");
  setupDefaultTransitions();
}

void UIStateMachine::initialize() {
  setState(UIState::MAIN_MENU);
}

void UIStateMachine::update() {
  uint32_t now = millis();
  if (now - lastUpdateTime < 50) return; // Update every 50ms
  lastUpdateTime = now;

  if (currentHandler) {
    currentHandler->update();
  }
}

void UIStateMachine::render() {
  if (currentHandler) {
    currentHandler->render();
  }
}

void UIStateMachine::setState(UIState newState) {
  if (newState == currentState) return;

  if (currentHandler) {
    currentHandler->onExit();
    delete currentHandler;
  }

  previousState = currentState;
  currentState = newState;
  stateEntryTime = millis();

  currentHandler = createHandler(newState);
  if (currentHandler) {
    currentHandler->onEnter();
  }

  char msg[128];
  snprintf(msg, 127, "State transition: %u -> %u", (uint8_t)previousState, (uint8_t)newState);
  Logger::getInstance().info("UIStateMachine", msg);
}

void UIStateMachine::handleEvent(UIEvent event) {
  UIState nextState;
  if (findTransition(currentState, event, nextState)) {
    setState(nextState);
  }

  if (currentHandler) {
    currentHandler->handleEvent(event);
  }
}

void UIStateMachine::registerTransition(const UITransition& trans) {
  if (transitions.size() >= MAX_TRANSITIONS) return;

  UITransition* t = new UITransition(trans);
  transitions.push_back(t);
}

bool UIStateMachine::findTransition(UIState from, UIEvent event, UIState& to) {
  for (auto* t : transitions) {
    if (t->fromState == from && t->trigger == event) {
      to = t->toState;
      if (t->onExit) t->onExit();
      if (t->onEnter) t->onEnter();
      return true;
    }
  }
  return false;
}

void UIStateMachine::printStateDebug() {
  Serial.println("\n========== UI State Machine Debug ==========");
  Serial.printf("Current State: %u\n", (uint8_t)currentState);
  Serial.printf("Previous State: %u\n", (uint8_t)previousState);
  Serial.printf("Time in State: %u ms\n", millis() - stateEntryTime);
  Serial.printf("Registered Transitions: %u\n", (uint16_t)transitions.size());
  Serial.println("==========================================\n");
}

UIStateHandler* UIStateMachine::createHandler(UIState state) {
  switch (state) {
    case UIState::MAIN_MENU:
      return new MainMenuHandler();
    case UIState::ATTACK_MENU:
      return new AttackMenuHandler();
    case UIState::ATTACK_RUNNING:
      return new AttackRunningHandler();
    case UIState::ATTACK_RESULTS:
      return new AttackResultsHandler();
    default:
      return nullptr;
  }
}

void UIStateMachine::setupDefaultTransitions() {
  // Main menu transitions
  UITransition t1 = {UIState::MAIN_MENU, UIEvent::BUTTON_SELECT, UIState::ATTACK_MENU, nullptr, nullptr};
  registerTransition(t1);

  // Attack menu transitions
  UITransition t2 = {UIState::ATTACK_MENU, UIEvent::BUTTON_SELECT, UIState::ATTACK_CONFIG, nullptr, nullptr};
  registerTransition(t2);

  UITransition t3 = {UIState::ATTACK_MENU, UIEvent::BUTTON_BACK, UIState::MAIN_MENU, nullptr, nullptr};
  registerTransition(t3);

  // Attack running transitions
  UITransition t4 = {UIState::ATTACK_RUNNING, UIEvent::ATTACK_FINISHED, UIState::ATTACK_RESULTS, nullptr, nullptr};
  registerTransition(t4);

  // Results transitions
  UITransition t5 = {UIState::ATTACK_RESULTS, UIEvent::BUTTON_BACK, UIState::MAIN_MENU, nullptr, nullptr};
  registerTransition(t5);
}

// ============= ANIMATION ENGINE IMPLEMENTATION =============

void AnimationEngine::playAnimation(const char* name, uint32_t duration, bool loop) {
  Animation* anim = new Animation();
  anim->name = name;
  anim->duration = duration;
  anim->startTime = millis();
  anim->looping = loop;
  animations.push_back(anim);
}

void AnimationEngine::stopAnimation(const char* name) {
  for (uint16_t i = 0; i < animations.size(); i++) {
    if (strcmp(animations[i]->name, name) == 0) {
      delete animations[i];
      animations.erase(animations.begin() + i);
      return;
    }
  }
}

void AnimationEngine::update() {
  uint32_t now = millis();

  for (uint16_t i = 0; i < animations.size(); i++) {
    Animation* anim = animations[i];
    uint32_t elapsed = now - anim->startTime;

    if (elapsed >= anim->duration) {
      if (anim->looping) {
        anim->startTime = now;
        anim->currentFrame = 0;
      } else {
        delete anim;
        animations.erase(animations.begin() + i);
        i--;
      }
    } else {
      anim->currentFrame = (float)elapsed / anim->duration;
    }
  }
}

float AnimationEngine::getFrameProgress(const char* name) {
  for (auto* anim : animations) {
    if (strcmp(anim->name, name) == 0) {
      return anim->currentFrame;
    }
  }
  return 0.0f;
}

bool AnimationEngine::isAnimating(const char* name) const {
  for (auto* anim : animations) {
    if (strcmp(anim->name, name) == 0) {
      return true;
    }
  }
  return false;
}
