#include "Transaction.h"
#include <iostream>

void Transaction::commit() {
    // get all locks held by the transaction and release it
    for (std::vector<int64_t> lockMetadata : locks_held) {
        lockManager.getLock(lockMetadata[0], lockMetadata[1])->release();
    }
    std::cout <<"Transaction "<<txnId<<" committed"<< "\n";
}