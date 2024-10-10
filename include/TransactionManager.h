#include<unordered_map>
#include "LockManager.h"
#include "Transaction.h"

class TransactionManager {
public:
    LockManager &lockManager;
    TransactionManager(LockManager &lockManager) : lockManager(lockManager) {}
    std::unordered_map<int64_t, std::shared_ptr<Transaction>> transactions;
    std::shared_ptr<Transaction> createTransaction();
};