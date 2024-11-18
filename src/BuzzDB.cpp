#include "BuzzDB.h"
#include "QueryExecutor.h"
#include <thread>

BuzzDB::BuzzDB(ConcurrencyControl cc_mode): buffer_manager(version_manager), cc_mode(cc_mode) {
    // Storage Manager automatically created
}

void BuzzDB::insert(int key, int value, std::unique_ptr<Transaction> &t) {
    // Create a new tuple with the given key and value
    auto newTuple = std::make_unique<Tuple>(t->transaction_id, t->transaction_id);
    auto key_field = std::make_unique<Field>(key);
    auto value_field = std::make_unique<Field>(value);
    newTuple->addField(std::move(key_field));
    newTuple->addField(std::move(value_field));

    InsertOperator insertOp(buffer_manager);
    insertOp.setTupleToInsert(std::move(newTuple));
    bool status = insertOp.addTuple(t);
    std::cout << "Is tuples inserted: " << status << "\n";
}

void BuzzDB::printTuples() {
    std::cout << "Printing tuples" << std::endl;
    // use the versionManager to get the latest version of the tuple and print them
    for(auto& pair : version_manager.getAllLatestVersions()) {
        auto pageNumber = pair[0];
        auto slotNumber = pair[1];
            auto& currentPage = buffer_manager.getPage(pageNumber);
            char* page_buffer = currentPage->page_data.get();
            Slot* slot_array = reinterpret_cast<Slot*>(page_buffer);
            const char* tuple_data = page_buffer + slot_array[slotNumber].offset;
            std::istringstream iss(std::string(tuple_data, slot_array[slotNumber].length));
            std::unique_ptr<Tuple> currentTuple = Tuple::deserialize(iss);
            std::cout << currentTuple->creation_ts << " " << currentTuple->expiration_ts << " " << currentTuple->tuple_id << "\n";
            std::cout << currentTuple->is_visible << "\n";
            currentTuple->print();
    }
}

void BuzzDB::deleteTuples(int index) {
    DeleteOperator delOp(buffer_manager, 0, index); 
   
    if (!delOp.next()) {
        std::cerr << "Failed to delete tuple." << std::endl;
    }
}

void BuzzDB::updateTuples(int key, int deltaValue, std::unique_ptr<Transaction> &t) {
    std::cout << "Updating tuple with key: " << key << " by: " << deltaValue << "\n";
    /// Check if this tuple has already been updated by the transaction
    if(t->pending_writes.size() > 0) {
        for(auto& tupleMetadata : t->pending_writes) {
            if(tupleMetadata[2] == key) {
                std::cout << "Tuple already updated by the transaction !" << "\n";
                auto pageNumber = tupleMetadata[0];
                auto slotNumber = tupleMetadata[1];
                auto oldValue = tupleMetadata[3];
                auto newValue = oldValue + deltaValue;
                auto& currentPage = buffer_manager.getPage(pageNumber);
                char* page_buffer = currentPage->page_data.get();
                Slot* slot_array = reinterpret_cast<Slot*>(page_buffer);
                const char* tuple_data = page_buffer + slot_array[slotNumber].offset;
                std::istringstream iss(std::string(tuple_data, slot_array[slotNumber].length));
                std::unique_ptr<Tuple> currentTuple = Tuple::deserialize(iss);
                currentTuple->fields[1] = std::make_unique<Field>(static_cast<int>(newValue));
                //flush the tuple to disk
                auto serializedTupleFinal = currentTuple->serialize();
                std::memcpy(page_buffer + slot_array[slotNumber].offset, serializedTupleFinal.c_str(), currentTuple->getSize());
                buffer_manager.flushPage(pageNumber);
                tupleMetadata[3] = newValue;
                return;
            }
        }
    }
    /* Check the version manager for the latest version of the tuple and return it's metadata */
    while(true) {
        if(version_manager.getLatestVersion(key).empty()) {
            std::cerr << "Tuple not found" << "\n";
            return;
        }
        auto tupleMetadata = version_manager.getLatestVersion(key);
        auto pageNumber = tupleMetadata[0];
        auto slotNumber = tupleMetadata[1];

        std::cout << "Page Number: " << pageNumber << " Slot Number: " << slotNumber <<  " " << t->transaction_id << "\n";

        if(t->cc_mode == ConcurrencyControl::MV2PL){
            /// acquire read lock on the tuple
            t->getLockOnTuple(pageNumber, slotNumber);
            /// Check if the acquired lock is still valid (the latest version)
            auto newTupleMetadata = version_manager.getLatestVersion(key);
            if (newTupleMetadata[0] != pageNumber || newTupleMetadata[1] != slotNumber) {
                // The version has changed, release the lock and retry
                if (t->cc_mode == ConcurrencyControl::MV2PL) {
                    t->releaseLockOnTuple(pageNumber, slotNumber);
                }
                continue;
            }
        }
        
        while(pageNumber != -1 && slotNumber != -1) {
            auto& currentPage = buffer_manager.getPage(pageNumber);
            char* page_buffer = currentPage->page_data.get();
            Slot* slot_array = reinterpret_cast<Slot*>(page_buffer);
            const char* tuple_data = page_buffer + slot_array[slotNumber].offset;
            std::istringstream iss(std::string(tuple_data, slot_array[slotNumber].length));
            std::unique_ptr<Tuple> currentTuple = Tuple::deserialize(iss);
        
            /// check if currentTuple is visible to the transaction
            if((t->transaction_id >= currentTuple->creation_ts && t->transaction_id <= currentTuple->expiration_ts && t->transaction_id > currentTuple->tuple_id) || t->cc_mode == ConcurrencyControl::MV2PL){
                /// write is feasible, create a new version of the tuple
                auto newTuple = std::make_unique<Tuple>(t->transaction_id, t->transaction_id);
                currentTuple->expiration_ts = t->transaction_id;
                /// To Do: Flush the current tuple to disk
                newTuple->prev_page_number = currentTuple->page_number;
                newTuple->prev_slot_number = currentTuple->slot_number;
                auto currentValue = currentTuple->fields[1].get()->asInt();
                auto key_field = std::make_unique<Field>(key);
                std::cout << "Current Value: " << currentValue << " Delta Value: " << deltaValue << "\n";
                auto value_field = std::make_unique<Field>(currentValue + deltaValue);
                newTuple->addField(std::move(key_field));
                newTuple->addField(std::move(value_field));

                if(t->cc_mode == ConcurrencyControl::MV2PL){
                    /// add the current tuple to the pending reads of the transaction
                    t->pending_reads.push_back({pageNumber, slotNumber});
                }

                InsertOperator insertOp(buffer_manager);
                insertOp.setTupleToInsert(std::move(newTuple));
                bool status = insertOp.addTuple(t);
                break;
            }
            else{
                /// release lock on the current tuple being read
                if(t->cc_mode == ConcurrencyControl::MV2PL){
                    t->releaseLockOnTuple(pageNumber, slotNumber);
                }
                /// get the prev version of the tuple
                pageNumber = currentTuple->prev_page_number; 
                slotNumber = currentTuple->prev_slot_number;
            }
        }
        break;
    }
}

void BuzzDB::executeQueries() {
    std::vector<std::string> test_queries = {
        "SUM{1} GROUP BY {1} WHERE {1} > 2 and {1} < 6"
    };

    for (const auto& query : test_queries) {
        auto components = parseQuery(query);
        QueryExecutor::prettyPrint(components);
        QueryExecutor::executeQuery(components, buffer_manager);
    }
}