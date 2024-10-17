#include "Transaction.h"
#include <cmath>

Transaction::Transaction(int64_t transaction_id) : transaction_id(transaction_id) {
    commit_ts = INFINITY;
}