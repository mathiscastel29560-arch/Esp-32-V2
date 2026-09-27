#include "dos_attacks.h"
#include "logging_system.h"

// ============= FLOOD ATTACK IMPLEMENTATION =============

bool FloodAttack::begin() {
  Logger::getInstance().info("DoS", "Attaque Flood initialisée");
  return true;
}

bool FloodAttack::start() {
  Attack::start();
  packetsFlooded = 0;
  targetPort = 80; // Par défaut HTTP
  Logger::getInstance().logAttackStart(getName(), "Inondation TCP/UDP en cours");
  return true;
}

void FloodAttack::update() {
  if (!isRunning) return;

  uint32_t elapsed = millis() - startTime;

  // Simulation: envoi rapide de paquets
  if (elapsed % 10 == 0 && packetsFlooded < 5000) {
    packetsFlooded += 50; // 50 paquets tous les 10ms

    if (packetsFlooded % 500 == 0) {
      char desc[64];
      snprintf(desc, 63, "Paquet flood #%u", packetsFlooded / 50);
      AttackResult* result = ResultBuilder::createPacket(
        (uint8_t*)"FLOOD_PKT", 9, desc);
      addResult(result);
    }
  }

  if (elapsed > floodDuration || packetsFlooded >= 5000) {
    setStatus(AttackStatus::SUCCESS);
    isRunning = false;
    char msg[128];
    snprintf(msg, 127, "%u paquets envoyés", packetsFlooded);
    Logger::getInstance().logAttackEnd(getName(), currentStatus);
  }
}

bool FloodAttack::stop() {
  Attack::stop();
  return true;
}

bool FloodAttack::setParameter(const char* key, const char* value) {
  if (strcmp(key, "port") == 0) {
    targetPort = atoi(value);
    return true;
  }
  if (strcmp(key, "duration") == 0) {
    floodDuration = atoi(value);
    return true;
  }
  return false;
}

// ============= AMPLIFICATION ATTACK IMPLEMENTATION =============

bool AmplificationAttack::begin() {
  Logger::getInstance().info("DoS", "Attaque Amplification initialisée");
  return true;
}

bool AmplificationAttack::start() {
  Attack::start();
  packetsAmplified = 0;
  amplificationRatio = 30.0f; // 30x amplification par défaut
  Logger::getInstance().logAttackStart(getName(), "Attaque amplification en cours");
  return true;
}

void AmplificationAttack::update() {
  if (!isRunning) return;

  uint32_t elapsed = millis() - startTime;

  // Simulation: amplification progressive
  if (elapsed % 50 == 0 && packetsAmplified < 10000) {
    packetsAmplified += (uint32_t)(amplificationRatio);

    if (packetsAmplified % 2000 == 0) {
      uint32_t amplified = (uint32_t)(packetsAmplified * amplificationRatio);
      char desc[64];
      snprintf(desc, 63, "Flux amplifié x%.0f", amplificationRatio);
      AttackResult* result = ResultBuilder::createPacket(
        (uint8_t*)"AMPL_PKT", 8, desc);
      addResult(result);
    }
  }

  if (elapsed > attackDuration || packetsAmplified >= 10000) {
    setStatus(AttackStatus::SUCCESS);
    isRunning = false;
    Logger::getInstance().logAttackEnd(getName(), currentStatus);
  }
}

bool AmplificationAttack::stop() {
  Attack::stop();
  return true;
}

// ============= SLOWLORIS ATTACK IMPLEMENTATION =============

bool SlowlorisAttack::begin() {
  Logger::getInstance().info("DoS", "Attaque Slowloris initialisée");
  return true;
}

bool SlowlorisAttack::start() {
  Attack::start();
  connectionsHeld = 0;
  Logger::getInstance().logAttackStart(getName(), "Slowloris: lent drain de ressources");
  return true;
}

void SlowlorisAttack::update() {
  if (!isRunning) return;

  uint32_t elapsed = millis() - startTime;

  // Simulation: maintien de connexions lentes
  if (elapsed % 500 == 0 && connectionsHeld < 100) {
    connectionsHeld++;

    if (connectionsHeld % 20 == 0) {
      char desc[64];
      snprintf(desc, 63, "Connexions lentes: %u", connectionsHeld);
      AttackResult* result = ResultBuilder::createScan(desc, -60);
      addResult(result);
    }
  }

  if (elapsed > attackDuration || connectionsHeld >= 100) {
    setStatus(connectionsHeld > 0 ? AttackStatus::SUCCESS : AttackStatus::FAILED);
    isRunning = false;
    Logger::getInstance().logAttackEnd(getName(), currentStatus);
  }
}

bool SlowlorisAttack::stop() {
  Attack::stop();
  return true;
}

bool SlowlorisAttack::setParameter(const char* key, const char* value) {
  if (strcmp(key, "server") == 0) {
    strncpy(targetServer, value, 63);
    targetServer[63] = '\0';
    return true;
  }
  return false;
}

// ============= DNS AMPLIFICATION IMPLEMENTATION =============

bool DNSAmplification::begin() {
  Logger::getInstance().info("DoS", "Amplification DNS initialisée");
  return true;
}

bool DNSAmplification::start() {
  Attack::start();
  queriesSent = 0;
  Logger::getInstance().logAttackStart(getName(), "Amplification DNS (spoofée)");
  return true;
}

void DNSAmplification::update() {
  if (!isRunning) return;

  uint32_t elapsed = millis() - startTime;

  // Simulation: requêtes DNS amplifiées
  if (elapsed % 100 == 0 && queriesSent < 500) {
    queriesSent++;

    if (queriesSent % 50 == 0) {
      char desc[64];
      snprintf(desc, 63, "Requête DNS #%u", queriesSent);
      AttackResult* result = ResultBuilder::createPacket(
        (uint8_t*)"DNS_REQ", 7, desc);
      addResult(result);
    }
  }

  if (elapsed > amplificationDuration || queriesSent >= 500) {
    setStatus(AttackStatus::SUCCESS);
    isRunning = false;
    Logger::getInstance().logAttackEnd(getName(), currentStatus);
  }
}

bool DNSAmplification::stop() {
  Attack::stop();
  return true;
}

// ============= NTP REFLECTION IMPLEMENTATION =============

bool NTPReflection::begin() {
  Logger::getInstance().info("DoS", "Réflexion NTP initialisée");
  return true;
}

bool NTPReflection::start() {
  Attack::start();
  reflectionPackets = 0;
  Logger::getInstance().logAttackStart(getName(), "Attaque réflexion NTP");
  return true;
}

void NTPReflection::update() {
  if (!isRunning) return;

  uint32_t elapsed = millis() - startTime;

  // Simulation: réflexion de paquets NTP
  if (elapsed % 120 == 0 && reflectionPackets < 600) {
    reflectionPackets++;

    if (reflectionPackets % 100 == 0) {
      char desc[64];
      snprintf(desc, 63, "Paquet NTP réfléchi #%u", reflectionPackets);
      AttackResult* result = ResultBuilder::createPacket(
        (uint8_t*)"NTP_REFL", 8, desc);
      addResult(result);
    }
  }

  if (elapsed > reflectionDuration || reflectionPackets >= 600) {
    setStatus(AttackStatus::SUCCESS);
    isRunning = false;
    Logger::getInstance().logAttackEnd(getName(), currentStatus);
  }
}

bool NTPReflection::stop() {
  Attack::stop();
  return true;
}

// ============= RESOURCE EXHAUSTION IMPLEMENTATION =============

bool ResourceExhaustion::begin() {
  Logger::getInstance().info("DoS", "Épuisement Ressources initialisé");
  return true;
}

bool ResourceExhaustion::start() {
  Attack::start();
  resourcesConsumed = 0;
  Logger::getInstance().logAttackStart(getName(), "Épuisement CPU/Mémoire");
  return true;
}

void ResourceExhaustion::update() {
  if (!isRunning) return;

  uint32_t elapsed = millis() - startTime;

  // Simulation: consommation progressive de ressources
  if (elapsed % 50 == 0 && resourcesConsumed < 100000) {
    resourcesConsumed += 1000;

    if (resourcesConsumed % 10000 == 0) {
      uint16_t percentUsed = (resourcesConsumed / 100000) * 100;
      char desc[64];
      snprintf(desc, 63, "Ressources %u%% consommées", percentUsed);
      AttackResult* result = ResultBuilder::createScan(desc, -40);
      addResult(result);
    }
  }

  if (elapsed > exhaustionDuration || resourcesConsumed >= 100000) {
    setStatus(AttackStatus::SUCCESS);
    isRunning = false;
    Logger::getInstance().logAttackEnd(getName(), currentStatus);
  }
}

bool ResourceExhaustion::stop() {
  Attack::stop();
  return true;
}
