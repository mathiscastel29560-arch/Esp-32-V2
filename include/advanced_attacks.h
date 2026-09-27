#ifndef ADVANCED_ATTACKS_H
#define ADVANCED_ATTACKS_H

#include "attack_framework.h"

// ============= PACKET SNIFFER =============
class PacketSniffer : public Attack {
public:
  PacketSniffer() : Attack("Packet Sniffer"), packetsSniffed(0),
                    sniffDuration(25000) {}

  bool begin() override;
  bool start() override;
  void update() override;
  bool stop() override;
  bool cleanup() override;

  bool setParameter(const char* key, const char* value) override;

private:
  uint32_t packetsSniffed;
  uint32_t sniffDuration;
  char filterProtocol[16];
};

// ============= MAN-IN-THE-MIDDLE =============
class ManInTheMiddle : public Attack {
public:
  ManInTheMiddle() : Attack("Man-in-the-Middle"), packetsIntercepted(0),
                     mitDuration(20000) {}

  bool begin() override;
  bool start() override;
  void update() override;
  bool stop() override;

private:
  uint32_t packetsIntercepted;
  uint32_t mitDuration;
};

// ============= CREDENTIAL HARVESTER =============
class CredentialHarvester : public Attack {
public:
  CredentialHarvester() : Attack("Credential Harvester"),
                          credentialsHarvested(0), harvestDuration(30000) {}

  bool begin() override;
  bool start() override;
  void update() override;
  bool stop() override;

private:
  uint16_t credentialsHarvested;
  uint32_t harvestDuration;
};

// ============= SSL STRIPPING =============
class SSLStripping : public Attack {
public:
  SSLStripping() : Attack("SSL Stripping"), connectionsDowngraded(0),
                   stripDuration(15000) {}

  bool begin() override;
  bool start() override;
  void update() override;
  bool stop() override;

private:
  uint16_t connectionsDowngraded;
  uint32_t stripDuration;
};

// ============= DNS SPOOFING =============
class DNSSpoofing : public Attack {
public:
  DNSSpoofing() : Attack("DNS Spoofing"), responsesForged(0),
                  spoofDuration(20000) {}

  bool begin() override;
  bool start() override;
  void update() override;
  bool stop() override;

  bool setParameter(const char* key, const char* value) override;

private:
  uint16_t responsesForged;
  uint32_t spoofDuration;
  char targetDomain[128];
};

// ============= ARP SPOOFING =============
class ARPSpoofing : public Attack {
public:
  ARPSpoofing() : Attack("ARP Spoofing"), poisoningsAttempted(0),
                  poisonDuration(10000) {}

  bool begin() override;
  bool start() override;
  void update() override;
  bool stop() override;

  bool setParameter(const char* key, const char* value) override;

private:
  uint16_t poisoningsAttempted;
  uint32_t poisonDuration;
  char targetIP[16];
};

// ============= VULNERABILITY SCANNER =============
class VulnerabilityScanner : public Attack {
public:
  VulnerabilityScanner() : Attack("Vulnerability Scanner"),
                           vulnerabilitiesFound(0), scanDuration(30000) {}

  bool begin() override;
  bool start() override;
  void update() override;
  bool stop() override;

private:
  uint16_t vulnerabilitiesFound;
  uint32_t scanDuration;
};

// ============= BRUTE FORCE ATTACK =============
class BruteForceAttack : public Attack {
public:
  BruteForceAttack() : Attack("Brute Force"), attemptsPerformed(0),
                       bruteForceDuration(15000) {}

  bool begin() override;
  bool start() override;
  void update() override;
  bool stop() override;

  bool setParameter(const char* key, const char* value) override;

private:
  uint32_t attemptsPerformed;
  uint32_t bruteForceDuration;
  char targetService[32];
};

#endif // ADVANCED_ATTACKS_H
