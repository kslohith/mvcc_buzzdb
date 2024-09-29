#include "BuzzDB.h"

BuzzDB::BuzzDB() : tuple_manager(buffer_manager) {
    // Storage Manager automatically created
}

void BuzzDB::insert(int key, int value, int transaction_id) {
    tuple_insertion_attempt_counter += 1;

    // Create a new tuple with the given key and value
    auto newTuple = std::make_unique<Tuple>(transaction_id);
    auto key_field = std::make_unique<Field>(key);
    auto value_field = std::make_unique<Field>(value);
    newTuple->addField(std::move(key_field));
    newTuple->addField(std::move(value_field));

    InsertOperator insertOp(buffer_manager, tuple_manager);
    insertOp.setTupleToInsert(std::move(newTuple));
    bool status = insertOp.next();
    std::cout << "Is tuples inserted: " << status << "\n";
}

void BuzzDB::printTuples(int key) {
    ScanOperator scanOp(buffer_manager, tuple_manager);
    scanOp.open();
    
    while (scanOp.next()) {
        const auto& tuple = scanOp.getCurrentTuple();
        if (tuple->begin_ts < key && tuple->end_ts > key) {
            printTuple(tuple->fields);
        }
    }
    
    scanOp.close();
}

void BuzzDB::updateTuples(int key, int value) {
    auto newTuple = std::make_unique<Tuple>();
    auto key_field = std::make_unique<Field>(key);
    auto value_field = std::make_unique<Field>(value);
    newTuple->addField(std::move(key_field));
    newTuple->addField(std::move(value_field));

    UpdateOperator updateOp(buffer_manager, tuple_manager);
    updateOp.setTupleToUpdate(std::move(newTuple));
    
    if (!updateOp.next()) {
        std::cerr << "Failed to update tuple." << std::endl;
    }
}

void BuzzDB::deleteTuples(int index) {
    DeleteOperator delOp(buffer_manager, 0, index, tuple_manager); 
   
    if (!delOp.next()) {
        std::cerr << "Failed to delete tuple." << std::endl;
    }
}

void BuzzDB::executeQueries() {
    std::vector<std::string> test_queries = {
        "SUM{1} GROUP BY {1} WHERE {1} > 2 and {1} < 6"
    };

    for (const auto& query : test_queries) {
        auto components = parseQuery(query);
        prettyPrint(components);
        executeQuery(components, buffer_manager);
    }
}