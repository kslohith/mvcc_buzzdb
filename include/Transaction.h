#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <cstdint>
#include <vector>

class BufferManager;
class Transaction {
public:
    int64_t transaction_id;
    int64_t commit_ts;
    /// @brief Store the metadata / address for the tuples that are being updated by the transaction
    std::vector<std::vector<int64_t>> pending_writes;
    BufferManager& buffer_manager;

    Transaction(int64_t transaction_id, BufferManager& buffer_manager);
    void commit();
};

#endif // TRANSACTION_H