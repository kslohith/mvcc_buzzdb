#include <iostream>
#include <chrono>
#include "BuzzDB.h"
#include "QueryParser.h"
#include "QueryExecutor.h"
#include <thread>

int main() {
    BuzzDB db;

    // Start the transaction
    auto start = std::chrono::high_resolution_clock::now();

    //  db.insert(1, 10);
    //  db.insert(2, 20);
    //  db.insert(3, 30);

    auto txn1 = db.txn_manager.createTransaction();
    auto txn2 = db.txn_manager.createTransaction();
    std::vector<std::thread> threads;
    threads.emplace_back([&db, txn1](){
        db.updateTuples(2, 35, *txn1);
        db.printTuples();
        txn1->commit();
    });

    threads.emplace_back([&db, txn2](){
        db.updateTuples(2, 40, *txn2);
        db.printTuples();
        txn2->commit();
    });

    for(auto& thread : threads) {
        thread.join();
    }

    // Calculate and print the elapsed time
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::micro> elapsed = end - start;
    std::cout << "Elapsed time: " << elapsed.count() << " microseconds" << std::endl;

    return 0;
}