#pragma once

#include <string>
#include <vector>

// Advanced features menu system
class AdvancedMenu {
public:
  enum AdvancedTab {
    ADV_SECURITY = 0,      // Encryption, Auth, API Keys
    ADV_MONITORING = 1,    // Alerts, CPU/Memory/Battery
    ADV_AUTOMATION = 2,    // Scheduled audits, workflows
    ADV_EXPORT = 3,        // CSV/JSON exports, filters
    ADV_OTA = 4,           // Delta OTA, firmware updates
    ADV_LANGUAGE = 5,      // EN/FR/ES, Dark mode
    ADV_DATABASE = 6,      // SQLite, backup, retention
    ADV_CLOUD = 7          // Cloud sync, remote config
  };

  static AdvancedMenu& getInstance() {
    static AdvancedMenu instance;
    return instance;
  }

  // Display advanced menu
  void display(AdvancedTab tab);

  // Handle interactions
  void handleSelect(AdvancedTab tab, int itemIndex);
  void handleBack();

  // Display specific tabs
  void displaySecurityTab();
  void displayMonitoringTab();
  void displayAutomationTab();
  void displayExportTab();
  void displayOTATab();
  void displayLanguageTab();
  void displayDatabaseTab();
  void displayCloudTab();

private:
  AdvancedMenu() = default;

  // Helper methods
  void drawHeader(const std::string& title);
  void drawSecurityStatus();
  void drawAlertStatus();
  void drawScheduleStatus();
};

#endif // ADVANCED_MENU_H
