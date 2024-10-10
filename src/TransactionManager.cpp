#include "TransactionManager.h"

std::shared_ptr<Transaction> TransactionManager::createTransaction() {
    auto transaction = std::make_shared<Transaction>(lockManager);
    transactions[transaction->txnId] = transaction;
    return transaction;
}