#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <cstdint>
#include <vector>
#include <mutex>
#include "ConcurrencyControl.h"

class BufferManager;
class TransactionManager;
class VersionManager;
class LockManager;
class Transaction {
public:
    int64_t transaction_id;
    int64_t commit_ts;
    /// @brief Store the metadata / address for the tuples that are being updated by the transaction
    std::vector<std::vector<int64_t>> pending_writes;
    std::vector<std::vector<int64_t>> pending_reads;
    BufferManager& buffer_manager;
    VersionManager& version_manager;
    TransactionManager& transaction_manager;
    LockManager& lock_manager;
    static std::mutex commit_mutex;
    ConcurrencyControl cc_mode;

    Transaction(int64_t transaction_id, BufferManager& buffer_manager, VersionManager& version_manager, TransactionManager& transaction_manager, LockManager& lock_manager, ConcurrencyControl cc_mode);
    int commit();
    int commitMVOCC();
    int commitMV2PL();
    void getLockOnTuple(int page_number, int slot_id);
    void releaseLockOnTuple(int page_number, int slot_id);
};

#endif // TRANSACTION_H