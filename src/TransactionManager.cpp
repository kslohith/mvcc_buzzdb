#include "TransactionManager.h"
#include <iostream>

void TransactionManager::addCommittedTransaction(int64_t commit_ts, std::vector<std::vector<int64_t>> tupleMetadata) {
    std::cout << "Adding committed transaction: " << commit_ts << "\n";
    std::vector<int64_t> tuplesUpdated;
    for (std::vector<int64_t> tuple : tupleMetadata) {
        tuplesUpdated.push_back(tuple[2]);
    }
    commitedTransactions[commit_ts] = tuplesUpdated;
}

std::unordered_map<int64_t, std::vector<int64_t>> TransactionManager::getAllCommittedTransaction() {
    return commitedTransactions;
}
