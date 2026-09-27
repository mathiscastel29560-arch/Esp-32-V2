#ifndef DIAGNOSTICS_H
#define DIAGNOSTICS_H

#include <Arduino.h>

class SystemDiagnostics {
public:
  static SystemDiagnostics& getInstance() {
    static SystemDiagnostics instance;
    return instance;
  }

  void runFullDiagnostics() {
    Serial.println("\n============= SYSTEM DIAGNOSTICS =============");

    printMemoryStatus();
    printCPUStatus();
    printUptimeStatus();
    printFeatureStatus();

    Serial.println("=============================================\n");
  }

  void printMemoryStatus() {
    Serial.println("--- Memory Status ---");
    uint32_t freeHeap = ESP.getFreeHeap();
    uint32_t heapSize = ESP.getHeapSize();
    uint32_t heapUsed = heapSize - freeHeap;
    float heapPercent = (float)heapUsed / heapSize * 100.0f;

    Serial.printf("Heap: %lu bytes used, %lu bytes free (%.1f%% used)\n",
      heapUsed, freeHeap, heapPercent);

    uint32_t freePsram = ESP.getFreePsram();
    uint32_t psramSize = ESP.getPsramSize();
    uint32_t psramUsed = psramSize - freePsram;
    float psramPercent = (float)psramUsed / psramSize * 100.0f;

    Serial.printf("PSRAM: %lu bytes used, %lu bytes free (%.1f%% used)\n",
      psramUsed, freePsram, psramPercent);
  }

  void printCPUStatus() {
    Serial.println("--- CPU Status ---");
    Serial.printf("Frequency: %u MHz\n", getCpuFreqMhz());
    Serial.printf("Cores: %u\n", ESP.getChipCores());
    Serial.printf("Revision: %u\n", ESP.getChipRevision());
  }

  void printUptimeStatus() {
    Serial.println("--- Uptime Status ---");
    uint32_t uptimeSeconds = millis() / 1000;
    uint16_t days = uptimeSeconds / 86400;
    uint16_t hours = (uptimeSeconds % 86400) / 3600;
    uint16_t minutes = (uptimeSeconds % 3600) / 60;
    uint16_t seconds = uptimeSeconds % 60;

    Serial.printf("Uptime: %ud %uh %um %us\n", days, hours, minutes, seconds);
  }

  void printFeatureStatus() {
    Serial.println("--- Feature Status ---");
    Serial.printf("WiFi Support: %s\n", isWiFiCapable() ? "YES" : "NO");
    Serial.printf("BLE Support: %s\n", isBLECapable() ? "YES" : "NO");
    Serial.printf("PSRAM: %s\n", ESP.getPsramSize() > 0 ? "YES" : "NO");
  }

  void dumpStackTrace() {
    Serial.println("\n=== STACK TRACE ===");
    Serial.println("(Not implemented on this platform)");
  }

private:
  SystemDiagnostics() {}

  uint16_t getCpuFreqMhz() {
    return esp_clk_cpu_freq() / 1000000;
  }

  bool isWiFiCapable() {
    return true; // ESP32-S3 always has WiFi
  }

  bool isBLECapable() {
    return true; // ESP32-S3 always has BLE
  }
};

#endif
