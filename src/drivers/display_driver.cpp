#include "drivers.h"
#include "config.h"

// ============= DISPLAY DRIVER IMPLEMENTATION =============

bool Drivers::Display::begin() {
  // TODO: Initialize TFT_eSPI
  // - Setup SPI pins
  // - Initialize display
  // - Setup touch calibration
  return true;
}

void Drivers::Display::update() {
  // TODO: Update display buffer
}

void Drivers::Display::clear() {
  // TODO: Clear display
}

void Drivers::Display::drawString(int x, int y, const char* text, uint16_t color) {
  // TODO: Draw text at position
}

void Drivers::Display::drawRect(int x, int y, int w, int h, uint16_t color) {
  // TODO: Draw rectangle outline
}

void Drivers::Display::fillRect(int x, int y, int w, int h, uint16_t color) {
  // TODO: Draw filled rectangle
}
