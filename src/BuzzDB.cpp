#include "BuzzDB.h"
#include "QueryExecutor.h"

BuzzDB::BuzzDB(): buffer_manager(version_manager) {
    // Storage Manager automatically created
}

void BuzzDB::insert(int key, int value, std::unique_ptr<Transaction> &t) {

    // Create a new tuple with the given key and value
    auto newTuple = std::make_unique<Tuple>(t->transaction_id, t->commit_ts);
    auto key_field = std::make_unique<Field>(key);
    auto value_field = std::make_unique<Field>(value);
    newTuple->addField(std::move(key_field));
    newTuple->addField(std::move(value_field));

    InsertOperator insertOp(buffer_manager);
    insertOp.setTupleToInsert(std::move(newTuple));
    bool status = insertOp.next();
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
        currentTuple->print();
    }
}

void BuzzDB::deleteTuples(int index) {
    DeleteOperator delOp(buffer_manager, 0, index); 
   
    if (!delOp.next()) {
        std::cerr << "Failed to delete tuple." << std::endl;
    }
}

void BuzzDB::updateTuples(int key, int value, std::unique_ptr<Transaction> &t) {
    /* Check the version manager for the latest version of the tuple and return it's metadata */
    if(version_manager.getLatestVersion(key).empty()) {
        std::cerr << "Tuple not found" << "\n";
        return;
    }
    auto tupleMetadata = version_manager.getLatestVersion(key);
    auto pageNumber = tupleMetadata[0];
    auto slotNumber = tupleMetadata[1];
    /* Use the scan operator to get the tuple */
    ScanOperator scanOp(buffer_manager);
    scanOp.open();
    auto& currentPage = buffer_manager.getPage(pageNumber);
    char* page_buffer = currentPage->page_data.get();
    Slot* slot_array = reinterpret_cast<Slot*>(page_buffer);
    const char* tuple_data = page_buffer + slot_array[slotNumber].offset;
    std::istringstream iss(std::string(tuple_data, slot_array[slotNumber].length));
    std::unique_ptr<Tuple> currentTuple = Tuple::deserialize(iss);

    std::cout<< "Current Tuple: " << currentTuple->creation_ts << " " << currentTuple->expiration_ts << " " << currentTuple->tuple_id << " " << t->transaction_id << "\n";
   
    if(t->transaction_id >= currentTuple->creation_ts && t->transaction_id <= currentTuple->expiration_ts && t->transaction_id > currentTuple->tuple_id) {
        /// write is feasible, create a new version of the tuple
        auto newTuple = std::make_unique<Tuple>(t->transaction_id, t->commit_ts);
        currentTuple->expiration_ts = t->transaction_id;
        /// To Do: Flush the current tuple to disk
        newTuple->prev_page_number = currentTuple->page_number;
        newTuple->prev_slot_number = currentTuple->slot_number;
        std::cout << "Inserting new Tuple in the Database" << "\n";
        auto key_field = std::make_unique<Field>(key);
        auto value_field = std::make_unique<Field>(value);
        newTuple->addField(std::move(key_field));
        newTuple->addField(std::move(value_field));

        InsertOperator insertOp(buffer_manager);
        insertOp.setTupleToInsert(std::move(newTuple));
        bool status = insertOp.next();
        std::cout << "Is new version of tuples inserted: " << status << "\n";
    }
    else {
        std::cerr << "Transaction not feasible! Aborting !" << "\n";
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