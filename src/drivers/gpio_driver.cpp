#include "drivers.h"
#include "config.h"

// ============= GPIO DRIVER IMPLEMENTATION =============

bool Drivers::GPIO::begin() {
  // TODO: Initialize buttons as inputs with pull-up
  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);
  pinMode(BTN_SELECT, INPUT_PULLUP);
  pinMode(BTN_BACK, INPUT_PULLUP);

  // TODO: Initialize buzzer as output
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  // TODO: Initialize ADC for battery reading
  analogSetPinAttenuation(BATTERY_ADC_PIN, ADC_11db);

  return true;
}

int Drivers::GPIO::readButton(int btn) {
  // TODO: Read button state (debounce needed)
  return digitalRead(btn);
}

void Drivers::GPIO::buzzOn(int duration_ms) {
  // TODO: Activate buzzer for duration
  digitalWrite(BUZZER_PIN, HIGH);
  delay(duration_ms);
  digitalWrite(BUZZER_PIN, LOW);
}

int Drivers::GPIO::readBatteryPercent() {
  // TODO: Read battery ADC and convert to percentage
  int raw = analogRead(BATTERY_ADC_PIN);
  int voltage = (raw * 3300) / 4095;

  if (voltage >= BATTERY_MAX_MV) return 100;
  if (voltage <= BATTERY_MIN_MV) return 0;

  int percent = ((voltage - BATTERY_MIN_MV) * 100) / (BATTERY_MAX_MV - BATTERY_MIN_MV);
  return constrain(percent, 0, 100);
}

int Drivers::GPIO::readBatteryVoltage() {
  // TODO: Read battery voltage in mV
  int raw = analogRead(BATTERY_ADC_PIN);
  return (raw * 3300) / 4095;
}
