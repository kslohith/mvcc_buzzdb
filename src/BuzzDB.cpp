#include "BuzzDB.h"
#include "QueryExecutor.h"

BuzzDB::BuzzDB() {
    // Storage Manager automatically created
}

void BuzzDB::insert(int key, int value) {
    tuple_insertion_attempt_counter += 1;

    // Create a new tuple with the given key and value
    auto newTuple = std::make_unique<Tuple>();
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
    ScanOperator scanOp(buffer_manager);
    scanOp.open();
    const auto& tuple = scanOp.getOutput();
    printTuple(tuple);
    while (scanOp.next()) {
        const auto& tuple = scanOp.getOutput();
        printTuple(tuple);
    }
    scanOp.close();
}

void BuzzDB::deleteTuples(int index) {
    DeleteOperator delOp(buffer_manager, 0, index); 
   
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
        QueryExecutor::prettyPrint(components);
        QueryExecutor::executeQuery(components, buffer_manager);
    }
}