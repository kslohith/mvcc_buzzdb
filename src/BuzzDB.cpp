#include "BuzzDB.h"
#include "QueryExecutor.h"

BuzzDB::BuzzDB() {
    // Storage Manager automatically created
}

void BuzzDB::insert(int key, int value, std::unique_ptr<Transaction> &t) {
    tuple_insertion_attempt_counter += 1;

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
    ScanOperator scanOp(buffer_manager);
    scanOp.open();
    const auto& tuple = scanOp.getCurrentTuple();
    std::cout << tuple->tuple_id << " " << tuple->page_number << " " << tuple->slot_number << " " << tuple->creation_ts << std::endl;
    printTuple(tuple->fields);
    while (scanOp.next()) {
        const auto& tuple = scanOp.getCurrentTuple();
        std::cout << tuple->tuple_id << " " << tuple->page_number << " " << tuple->slot_number << " " << tuple->creation_ts << std::endl;
        printTuple(tuple->fields);
    }
    scanOp.close();
}

void BuzzDB::deleteTuples(int index) {
    DeleteOperator delOp(buffer_manager, 0, index); 
   
    if (!delOp.next()) {
        std::cerr << "Failed to delete tuple." << std::endl;
    }
}

// void BuzzDB::updateTuples(int key, int value, Transaction &txn) {
//     /* Search for the existing tuple with the given key and get its page and slot number, if it does not have any lock associated with it, then acquire lock and then call update method of the tuple */
//     ScanOperator scanOp(buffer_manager);
//     scanOp.open();
//     while(scanOp.next()) {
//         std::unique_ptr<Tuple> &currentTuple = scanOp.getCurrentTuple();
//         if( currentTuple->fields[0]->asInt() == key ) {
//             std::cout << "Tuple found" << currentTuple->pageNumber << currentTuple->slotId << "\n";
//             std::shared_ptr<Lock> currentLock = lock_manager.getLock(currentTuple->pageNumber, currentTuple->slotId);
//             currentLock->acquire();
//             txn.locks_held.push_back({currentTuple->pageNumber, currentTuple->slotId});
//             /* update the value of the tuple with the passed value */
//             currentTuple->fields[1] = std::make_unique<Field>(value);
//             auto &page = buffer_manager.getPage(currentTuple->pageNumber);
//             page->updateTuple(currentTuple->pageNumber, currentTuple->slotId, std::move(currentTuple), lock_manager);
//             std::cout << "Tuple updated successfully" << "\n";
//             //Strict 2pl Locking protocol, locks will be released whenn the transaction commits
//             //currentLock->release();
//             break;
//         }
//     }
// }

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