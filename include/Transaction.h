#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <cstdint>
#include <vector>
#include <mutex>

class BufferManager;
class TransactionManager;
class VersionManager;
class Transaction {
public:
    int64_t transaction_id;
    int64_t commit_ts;
    /// @brief Store the metadata / address for the tuples that are being updated by the transaction
    std::vector<std::vector<int64_t>> pending_writes;
    BufferManager& buffer_manager;
    VersionManager& version_manager;
    TransactionManager& transaction_manager;
    static std::mutex commit_mutex;

    Transaction(int64_t transaction_id, BufferManager& buffer_manager, VersionManager& version_manager, TransactionManager& transaction_manager);
    int commit();
};

#endif // TRANSACTION_H