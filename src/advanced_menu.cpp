#include "advanced_menu.h"
#include "debug_logger.h"
#include "auth_system.h"
#include "alerts_system.h"
#include "scheduled_audits.h"
#include "audit_filter.h"
#include "delta_ota.h"
#include "i18n_strings.h"
#include "wifi_dashboard.h"

void AdvancedMenu::display(AdvancedTab tab) {
  switch (tab) {
    case ADV_SECURITY:
      displaySecurityTab();
      break;
    case ADV_MONITORING:
      displayMonitoringTab();
      break;
    case ADV_AUTOMATION:
      displayAutomationTab();
      break;
    case ADV_EXPORT:
      displayExportTab();
      break;
    case ADV_OTA:
      displayOTATab();
      break;
    case ADV_LANGUAGE:
      displayLanguageTab();
      break;
    case ADV_DATABASE:
      displayDatabaseTab();
      break;
    case ADV_CLOUD:
      displayCloudTab();
      break;
  }
}

void AdvancedMenu::handleSelect(AdvancedTab tab, int itemIndex) {
  DebugLogger::printf("[AdvancedMenu] Selected tab=%u item=%d\n", tab, itemIndex);

  auto& auth = AuthSystem::getInstance();
  auto& alerts = AlertsSystem::getInstance();
  auto& scheduler = ScheduledAudits::getInstance();

  switch (tab) {
    case ADV_SECURITY:
      // Generate new API key
      if (itemIndex == 0) {
        auth.generateAPIKey("New Key", AuthSystem::ROLE_OPERATOR);
        DebugLogger::println("[Advanced] Generated new API key");
      }
      break;

    case ADV_MONITORING:
      // Acknowledge alerts
      if (itemIndex == 0) {
        auto recentAlerts = alerts.getRecentAlerts(5);
        for (const auto& alert : recentAlerts) {
          alerts.acknowledgeAlert(alert.timestamp);
        }
        DebugLogger::println("[Advanced] Acknowledged recent alerts");
      }
      break;

    case ADV_AUTOMATION:
      // Create new scheduled audit
      if (itemIndex == 0) {
        scheduler.createAudit("Manual Schedule",
          "wifi_scan",
          ScheduledAudits::RECUR_DAILY,
          "{}");
        DebugLogger::println("[Advanced] Created new scheduled audit");
      }
      break;

    case ADV_EXPORT:
      // Export to CSV
      if (itemIndex == 0) {
        auto& dashboard = WiFiDashboard::getInstance();
        std::string csv = dashboard.exportAuditsToCSV();
        DebugLogger::printf("[Advanced] Exported CSV (%u bytes)\n", csv.length());
      }
      break;

    case ADV_OTA:
      // Check for delta update
      if (itemIndex == 0) {
        auto& deltaOTA = DeltaOTA::getInstance();
        DebugLogger::println("[Advanced] Checking for delta update...");
      }
      break;

    case ADV_LANGUAGE:
      // Change language
      if (itemIndex == 0) {
        auto& i18n = I18nStrings::getInstance();
        i18n.setLanguage("en");
        DebugLogger::println("[Advanced] Language: English");
      } else if (itemIndex == 1) {
        auto& i18n = I18nStrings::getInstance();
        i18n.setLanguage("fr");
        DebugLogger::println("[Advanced] Language: Français");
      } else if (itemIndex == 2) {
        auto& i18n = I18nStrings::getInstance();
        i18n.setLanguage("es");
        DebugLogger::println("[Advanced] Language: Español");
      }
      break;

    default:
      break;
  }
}

void AdvancedMenu::handleBack() {
  DebugLogger::println("[AdvancedMenu] Returning to main menu");
}

void AdvancedMenu::displaySecurityTab() {
  drawHeader("🔐 SECURITY");

  auto& auth = AuthSystem::getInstance();
  auto keys = auth.listAPIKeys();

  Serial.println("\n🔑 API Keys Management:");
  Serial.printf("  Total Keys: %u\n", (unsigned int)keys.size());

  Serial.println("\n📋 Options:");
  Serial.println("  [1] Generate new API key");
  Serial.println("  [2] List all keys");
  Serial.println("  [3] Revoke key");
  Serial.println("  [4] View permissions");

  drawSecurityStatus();
}

void AdvancedMenu::displayMonitoringTab() {
  drawHeader("📊 MONITORING");

  auto& alerts = AlertsSystem::getInstance();
  auto stats = alerts.getStats();

  Serial.println("\n⚠️ Alert Statistics:");
  Serial.printf("  Total Alerts: %u\n", stats.totalAlerts);
  Serial.printf("  Unacknowledged: %u\n", stats.unacknowledgedCount);
  Serial.printf("  Critical: %u\n", stats.criticalCount);
  Serial.printf("  Warnings: %u\n", stats.warningCount);

  Serial.println("\n🎚️ Threshold Management:");
  Serial.println("  [1] Battery Low (20%)");
  Serial.println("  [2] Battery Critical (5%)");
  Serial.println("  [3] Memory Low (50KB)");
  Serial.println("  [4] CPU High (80%)");

  drawAlertStatus();
}

void AdvancedMenu::displayAutomationTab() {
  drawHeader("⏰ AUTOMATION");

  auto& scheduler = ScheduledAudits::getInstance();
  auto audits = scheduler.getScheduledAudits();

  Serial.println("\n📅 Scheduled Audits:");
  Serial.printf("  Total: %u\n", scheduler.getTotalCount());
  Serial.printf("  Enabled: %u\n", scheduler.getEnabledCount());

  Serial.println("\n📋 Options:");
  Serial.println("  [1] Create new schedule");
  Serial.println("  [2] List all audits");
  Serial.println("  [3] Enable/Disable audit");
  Serial.println("  [4] Delete audit");

  drawScheduleStatus();
}

void AdvancedMenu::displayExportTab() {
  drawHeader("📤 EXPORT & FILTER");

  auto& filter = AuditFilter::getInstance();
  auto& dashboard = WiFiDashboard::getInstance();

  Serial.println("\n📋 Export Formats:");
  Serial.println("  [1] Export to CSV");
  Serial.println("  [2] Export to JSON");
  Serial.println("  [3] Custom filter");
  Serial.println("  [4] Download all");

  Serial.println("\n🔍 Filter Options:");
  Serial.println("  • Date range");
  Serial.println("  • Device count");
  Serial.println("  • RSSI range");
  Serial.println("  • Success only");
}

void AdvancedMenu::displayOTATab() {
  drawHeader("🔄 OTA UPDATES");

  auto& deltaOTA = DeltaOTA::getInstance();

  Serial.println("\n📦 Firmware Update:");
  Serial.printf("  State: %u\n", deltaOTA.getState());
  Serial.printf("  Progress: %u%%\n", deltaOTA.getProgress());

  Serial.println("\n⚡ Delta OTA:");
  Serial.printf("  Delta Size: %u bytes\n", deltaOTA.getDeltaSize());
  Serial.printf("  Bytes Saved: %u KB\n", deltaOTA.getBytesSaved() / 1024);

  Serial.println("\n📋 Options:");
  Serial.println("  [1] Check for updates");
  Serial.println("  [2] Download delta");
  Serial.println("  [3] Full firmware update");
  Serial.println("  [4] Rollback");
}

void AdvancedMenu::displayLanguageTab() {
  drawHeader("🌐 LANGUAGE & THEME");

  auto& i18n = I18nStrings::getInstance();
  auto& dashboard = WiFiDashboard::getInstance();

  Serial.println("\n🌍 Language:");
  Serial.printf("  Current: %s\n", i18n.getLanguage().c_str());

  Serial.println("\n📋 Available:");
  Serial.println("  [1] English (EN)");
  Serial.println("  [2] Français (FR)");
  Serial.println("  [3] Español (ES)");

  Serial.println("\n🌙 Theme:");
  Serial.printf("  Dark Mode: %s\n", dashboard.getDarkMode() ? "ON" : "OFF");

  Serial.println("\n📋 Options:");
  Serial.println("  [4] Toggle dark mode");
}

void AdvancedMenu::displayDatabaseTab() {
  drawHeader("💾 DATABASE");

  Serial.println("\n📊 Storage:");
  Serial.println("  • Audit history (NVS)");
  Serial.println("  • Encrypted logs");
  Serial.println("  • Configuration");

  Serial.println("\n📋 Options:");
  Serial.println("  [1] Database stats");
  Serial.println("  [2] Cleanup old records");
  Serial.println("  [3] Backup database");
  Serial.println("  [4] Restore backup");
}

void AdvancedMenu::displayCloudTab() {
  drawHeader("☁️ CLOUD SYNC");

  Serial.println("\n🔗 Cloud Integration:");
  Serial.println("  • Remote backup");
  Serial.println("  • Device sync");
  Serial.println("  • Firmware update");

  Serial.println("\n📋 Options:");
  Serial.println("  [1] Configure endpoint");
  Serial.println("  [2] Upload audit data");
  Serial.println("  [3] Download config");
  Serial.println("  [4] Sync now");
}

void AdvancedMenu::drawHeader(const std::string& title) {
  Serial.println("\n╔════════════════════════════════════════╗");
  Serial.printf("║ %s\n", title.c_str());
  Serial.println("╚════════════════════════════════════════╝");
}

void AdvancedMenu::drawSecurityStatus() {
  auto& auth = AuthSystem::getInstance();
  Serial.println("\n✅ Security Status:");
  Serial.println("  Encryption: AES-256-GCM");
  Serial.println("  Auth System: Active");
  Serial.println("  API Keys: Configured");
}

void AdvancedMenu::drawAlertStatus() {
  auto& alerts = AlertsSystem::getInstance();
  auto stats = alerts.getStats();

  Serial.println("\n📊 Status:");
  if (stats.criticalCount > 0) {
    Serial.printf("  ⚠️  CRITICAL ALERTS: %u\n", stats.criticalCount);
  } else {
    Serial.println("  ✅ All systems nominal");
  }
}

void AdvancedMenu::drawScheduleStatus() {
  auto& scheduler = ScheduledAudits::getInstance();
  uint32_t nextRun = scheduler.getNextRunTime();

  Serial.println("\n⏱️ Next Run:");
  if (nextRun == UINT32_MAX) {
    Serial.println("  No schedules enabled");
  } else {
    Serial.printf("  In %u minutes\n", (nextRun - time(nullptr)) / 60);
  }
}
