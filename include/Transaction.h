#include <cstdint>
#include <chrono>
#include <unordered_set>
#include "LockManager.h"

class Transaction {
public:
    int64_t txnId;
    std::vector<std::vector<int64_t>> locks_held;
    LockManager &lockManager;

    Transaction(LockManager &lockManager) : lockManager(lockManager) {  
        txnId = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    }
   
    void begin();
    void commit();
    void abort();
};