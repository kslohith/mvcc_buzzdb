#include <cstdint>

class Transaction {
public:

    int64_t transaction_id;
    int64_t commit_ts;

    Transaction(int64_t transaction_id);
};