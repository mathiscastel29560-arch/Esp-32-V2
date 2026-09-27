#ifndef UI_STATE_MACHINE_H
#define UI_STATE_MACHINE_H

#include <Arduino.h>
#include <vector>

// ============= UI STATE DEFINITIONS =============

enum class UIState {
  STARTUP = 0,           // Boot animation
  MAIN_MENU = 1,         // Main menu
  ATTACK_MENU = 2,       // Attack selection
  ATTACK_CONFIG = 3,     // Configure attack
  ATTACK_RUNNING = 4,    // Attack in progress
  ATTACK_RESULTS = 5,    // Results display
  SETTINGS_MENU = 6,     // Settings
  DIAGNOSTICS = 7,       // Diagnostic mode
  HELP_SCREEN = 8,       // Help/Info
  ERROR_SCREEN = 9,      // Error display
  LOW_POWER_MODE = 10    // Low power display
};

enum class UIEvent {
  BUTTON_UP = 0,
  BUTTON_DOWN = 1,
  BUTTON_SELECT = 2,
  BUTTON_BACK = 3,
  TOUCH_INPUT = 4,
  TIMER_EXPIRED = 5,
  ATTACK_STARTED = 6,
  ATTACK_FINISHED = 7,
  ERROR_OCCURRED = 8,
  BATTERY_LOW = 9
};

struct UITransition {
  UIState fromState;
  UIEvent trigger;
  UIState toState;
  void (*onEnter)(void);
  void (*onExit)(void);
};

// ============= STATE HANDLERS =============

class UIStateHandler {
public:
  virtual ~UIStateHandler() {}

  virtual void onEnter() = 0;
  virtual void onExit() = 0;
  virtual void update() = 0;
  virtual void handleEvent(UIEvent event) = 0;
  virtual void render() = 0;
};

class MainMenuHandler : public UIStateHandler {
public:
  void onEnter() override;
  void onExit() override;
  void update() override;
  void handleEvent(UIEvent event) override;
  void render() override;

private:
  uint16_t selectedItem;
};

class AttackMenuHandler : public UIStateHandler {
public:
  void onEnter() override;
  void onExit() override;
  void update() override;
  void handleEvent(UIEvent event) override;
  void render() override;

private:
  uint16_t selectedAttack;
  uint16_t scrollOffset;
};

class AttackRunningHandler : public UIStateHandler {
public:
  void onEnter() override;
  void onExit() override;
  void update() override;
  void handleEvent(UIEvent event) override;
  void render() override;

private:
  uint32_t attackStartTime;
  uint8_t progressPercent;
};

class AttackResultsHandler : public UIStateHandler {
public:
  void onEnter() override;
  void onExit() override;
  void update() override;
  void handleEvent(UIEvent event) override;
  void render() override;

private:
  uint16_t selectedResult;
  uint16_t resultCount;
};

// ============= FSM CORE =============

class UIStateMachine {
public:
  static UIStateMachine& getInstance() {
    static UIStateMachine instance;
    return instance;
  }

  void initialize();
  void update();
  void render();

  void setState(UIState newState);
  UIState getState() const { return currentState; }
  UIState getPreviousState() const { return previousState; }

  void handleEvent(UIEvent event);

  void registerTransition(const UITransition& trans);
  bool findTransition(UIState from, UIEvent event, UIState& to);

  void printStateDebug();

private:
  UIStateMachine();

  UIState currentState;
  UIState previousState;
  UIStateHandler* currentHandler;

  std::vector<UITransition*> transitions;
  std::vector<UIStateHandler*> handlers;

  uint32_t lastUpdateTime;
  uint32_t stateEntryTime;

  static const uint16_t MAX_TRANSITIONS = 50;

  UIStateHandler* createHandler(UIState state);
  void setupDefaultTransitions();
};

// ============= ANIMATION ENGINE =============

struct Animation {
  const char* name;
  uint32_t duration;
  uint32_t startTime;
  bool looping;
  float currentFrame;

  Animation() : name(""), duration(0), startTime(0), looping(false), currentFrame(0) {}
};

class AnimationEngine {
public:
  static AnimationEngine& getInstance() {
    static AnimationEngine instance;
    return instance;
  }

  void playAnimation(const char* name, uint32_t duration, bool loop = false);
  void stopAnimation(const char* name);
  void update();

  float getFrameProgress(const char* name);
  bool isAnimating(const char* name) const;

private:
  AnimationEngine() {}

  std::vector<Animation*> animations;
};

#endif // UI_STATE_MACHINE_H
