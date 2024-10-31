#include "Transaction.h"
#include "Tuple.h"
#include "SlottedPage.h"
#include "BufferManager.h"
#include "TransactionManager.h"
#include "VersionManager.h"
#include <cmath>

std::mutex Transaction::commit_mutex;

Transaction::Transaction(int64_t transaction_id, BufferManager& buffer_manager, VersionManager& version_manager, TransactionManager& transaction_manager) : transaction_id(transaction_id), buffer_manager(buffer_manager), version_manager(version_manager), transaction_manager(transaction_manager) {
    commit_ts = 9223372036854775807;
}

int Transaction::commit() {
    std::lock_guard<std::mutex> lock(commit_mutex);
    std::cout << "Committing transaction: " << transaction_id << std::endl;
    /// First check if any update made in this transaction conflicts with concurrent transactions
    for(auto& tupleMetadata : pending_writes) {
        /// ToDo: Check for conflicts and abort if needed.
        auto tupleId = tupleMetadata[2];
        auto commitedTransactions = transaction_manager.getAllCommittedTransaction();
        for(auto& commitedTransaction : commitedTransactions) {
            std::cout << "Checking for conflict between: " << transaction_id << " and " << commitedTransaction.first << std::endl;
            if(commitedTransaction.first >= transaction_id) {
                for(auto& commitedTupleId : commitedTransaction.second) {
                    if(commitedTupleId == tupleId) {
                        /// Abort and restart the Transaction 
                        std::cout << "Transaction aborted: " << transaction_id << " due to conflict with transaction: " << commitedTransaction.first << std::endl;
                        return -1;
                    }
                }
            }
        }
    }
    /// Now we are sure that there are no conflicts, so update the verison manager and make necessary tuple changes and flish to disk
    for(auto& tupleMetadata : pending_writes) {
        /// ToDo: Check for conflicts and abort if needed.
        /// ToDo: get a write lock on the page
        auto pageNumber = tupleMetadata[0];
        auto slotNumber = tupleMetadata[1];
        auto tupleId = tupleMetadata[2];
        /// ToDo: update the tuple metadata
        auto& currentPage = buffer_manager.getPage(pageNumber);
        char* page_buffer = currentPage->page_data.get();
        Slot* slot_array = reinterpret_cast<Slot*>(page_buffer);
        const char* tuple_data = page_buffer + slot_array[slotNumber].offset;
        std::istringstream iss(std::string(tuple_data, slot_array[slotNumber].length));
        std::unique_ptr<Tuple> currentTuple = Tuple::deserialize(iss);
        int64_t currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        currentTuple->creation_ts = currentTime;
        /// flush the tuple to disk
        auto serializedTupleFinal = currentTuple->serialize();
        std::memcpy(page_buffer + slot_array[slotNumber].offset, serializedTupleFinal.c_str(), currentTuple->getSize());
        buffer_manager.flushPage(pageNumber);
        /// Add the current version of tuple to the version manager
        version_manager.addOrUpdateTuple(tupleId, {pageNumber, (int64_t)slotNumber});
        std::cout << "Transaction commited: " << transaction_id << " for tuple: " << tupleId << std::endl;
        /// ToDo: release the lock on the page
    }
    /// Write the transaction to the transaction manager
    auto commit_ts = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    transaction_manager.addCommittedTransaction(commit_ts, pending_writes);
    return 1;
}