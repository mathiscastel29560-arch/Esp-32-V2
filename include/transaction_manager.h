#ifndef TRANSACTION_MANAGER_H
#define TRANSACTION_MANAGER_H

#include <Arduino.h>
#include <vector>

enum class TransactionState {
  IDLE = 0,
  ACTIVE = 1,
  COMMITTING = 2,
  COMMITTED = 3,
  ROLLING_BACK = 4,
  ROLLED_BACK = 5,
  FAILED = 6
};

struct Transaction {
  uint32_t id;
  uint32_t startTime;
  TransactionState state;
  uint16_t operationCount;

  Transaction(uint32_t txId)
    : id(txId), startTime(millis()),
      state(TransactionState::ACTIVE), operationCount(0) {}
};

class TransactionManager {
public:
  static TransactionManager& getInstance() {
    static TransactionManager instance;
    return instance;
  }

  uint32_t beginTransaction() {
    uint32_t txId = nextId++;
    Transaction* tx = new Transaction(txId);
    transactions.push_back(tx);
    return txId;
  }

  bool addOperation(uint32_t txId) {
    Transaction* tx = getTransaction(txId);
    if (tx && tx->state == TransactionState::ACTIVE) {
      tx->operationCount++;
      return true;
    }
    return false;
  }

  bool commit(uint32_t txId) {
    Transaction* tx = getTransaction(txId);
    if (tx && tx->state == TransactionState::ACTIVE) {
      tx->state = TransactionState::COMMITTING;
      tx->state = TransactionState::COMMITTED;
      return true;
    }
    return false;
  }

  bool rollback(uint32_t txId) {
    Transaction* tx = getTransaction(txId);
    if (tx && (tx->state == TransactionState::ACTIVE ||
               tx->state == TransactionState::FAILED)) {
      tx->state = TransactionState::ROLLING_BACK;
      tx->state = TransactionState::ROLLED_BACK;
      return true;
    }
    return false;
  }

  Transaction* getTransaction(uint32_t txId) {
    for (auto* tx : transactions) {
      if (tx->id == txId) {
        return tx;
      }
    }
    return nullptr;
  }

  uint16_t getActiveTransactionCount() const {
    uint16_t count = 0;
    for (auto* tx : transactions) {
      if (tx->state == TransactionState::ACTIVE) {
        count++;
      }
    }
    return count;
  }

  void cleanup() {
    auto it = transactions.begin();
    while (it != transactions.end()) {
      if ((*it)->state == TransactionState::COMMITTED ||
          (*it)->state == TransactionState::ROLLED_BACK) {
        delete *it;
        it = transactions.erase(it);
      } else {
        ++it;
      }
    }
  }

  void printTransactions() {
    Serial.println("\n=== TRANSACTIONS ===");
    for (auto* tx : transactions) {
      Serial.printf("TX%lu: %s (%u ops, %lums)\n",
        tx->id, getStateName(tx->state), tx->operationCount,
        millis() - tx->startTime);
    }
    Serial.println("====================\n");
  }

private:
  TransactionManager() : nextId(1) {}

  const char* getStateName(TransactionState state) const {
    switch (state) {
      case TransactionState::IDLE: return "IDLE";
      case TransactionState::ACTIVE: return "ACTIVE";
      case TransactionState::COMMITTING: return "COMMITTING";
      case TransactionState::COMMITTED: return "COMMITTED";
      case TransactionState::ROLLING_BACK: return "ROLLING_BACK";
      case TransactionState::ROLLED_BACK: return "ROLLED_BACK";
      case TransactionState::FAILED: return "FAILED";
    }
    return "UNKNOWN";
  }

  std::vector<Transaction*> transactions;
  uint32_t nextId;
};

#endif
