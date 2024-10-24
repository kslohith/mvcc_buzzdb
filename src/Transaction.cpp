#include "Transaction.h"
#include "Tuple.h"
#include "SlottedPage.h"
#include "BufferManager.h"
#include <cmath>


Transaction::Transaction(int64_t transaction_id, BufferManager& buffer_manager) : transaction_id(transaction_id), buffer_manager(buffer_manager) {
    commit_ts = 9223372036854775807;
}

void Transaction::commit() {
    std::cout << "Committing transaction: " << transaction_id << std::endl;
    for(auto& tupleMetadata : pending_writes) {
        std::cout << "Updating tuple with metadata: " << tupleMetadata[0] << " " << tupleMetadata[1] << std::endl;
        /// Update the tuple with the new metadata
        /// ToDo: get a write lock on the page
        auto pageNumber = tupleMetadata[0];
        auto slotNumber = tupleMetadata[1];
        auto& currentPage = buffer_manager.getPage(pageNumber);
        char* page_buffer = currentPage->page_data.get();
        Slot* slot_array = reinterpret_cast<Slot*>(page_buffer);
        const char* tuple_data = page_buffer + slot_array[slotNumber].offset;
        std::istringstream iss(std::string(tuple_data, slot_array[slotNumber].length));
        std::unique_ptr<Tuple> currentTuple = Tuple::deserialize(iss);
        currentTuple->is_visible = true;
        /// Flush the current tuple to disk
        auto serializedTuple = currentTuple->serialize();
        std::memcpy(page_buffer + slot_array[slotNumber].offset, serializedTuple.c_str(), serializedTuple.size());
        buffer_manager.flushPage(pageNumber);
        /// ToDo: release the lock on the page
    }
}