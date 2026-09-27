#ifndef DOS_ATTACKS_H
#define DOS_ATTACKS_H

#include "attack_framework.h"

// ============= FLOOD ATTACK (TCP/UDP) =============
class FloodAttack : public Attack {
public:
  FloodAttack() : Attack("Flood Attack"), packetsFlooded(0),
                  floodDuration(10000) {}

  bool begin() override;
  bool start() override;
  void update() override;
  bool stop() override;

  bool setParameter(const char* key, const char* value) override;

private:
  uint32_t packetsFlooded;
  uint32_t floodDuration;
  uint16_t targetPort;
};

// ============= AMPLIFICATION ATTACK =============
class AmplificationAttack : public Attack {
public:
  AmplificationAttack() : Attack("Amplification Attack"),
                          amplificationRatio(0), packetsAmplified(0),
                          attackDuration(12000) {}

  bool begin() override;
  bool start() override;
  void update() override;
  bool stop() override;

private:
  float amplificationRatio;
  uint32_t packetsAmplified;
  uint32_t attackDuration;
};

// ============= SLOWLORIS ATTACK =============
class SlowlorisAttack : public Attack {
public:
  SlowlorisAttack() : Attack("Slowloris Attack"), connectionsHeld(0),
                      attackDuration(30000) {}

  bool begin() override;
  bool start() override;
  void update() override;
  bool stop() override;

  bool setParameter(const char* key, const char* value) override;

private:
  uint16_t connectionsHeld;
  uint32_t attackDuration;
  char targetServer[64];
};

// ============= DNS AMPLIFICATION =============
class DNSAmplification : public Attack {
public:
  DNSAmplification() : Attack("DNS Amplification"), queriesSent(0),
                       amplificationDuration(15000) {}

  bool begin() override;
  bool start() override;
  void update() override;
  bool stop() override;

private:
  uint16_t queriesSent;
  uint32_t amplificationDuration;
};

// ============= NTP REFLECTION =============
class NTPReflection : public Attack {
public:
  NTPReflection() : Attack("NTP Reflection"), reflectionPackets(0),
                    reflectionDuration(12000) {}

  bool begin() override;
  bool start() override;
  void update() override;
  bool stop() override;

private:
  uint16_t reflectionPackets;
  uint32_t reflectionDuration;
};

// ============= RESOURCE EXHAUSTION =============
class ResourceExhaustion : public Attack {
public:
  ResourceExhaustion() : Attack("Resource Exhaustion"),
                         resourcesConsumed(0), exhaustionDuration(20000) {}

  bool begin() override;
  bool start() override;
  void update() override;
  bool stop() override;

private:
  uint32_t resourcesConsumed;
  uint32_t exhaustionDuration;
};

#endif // DOS_ATTACKS_H
