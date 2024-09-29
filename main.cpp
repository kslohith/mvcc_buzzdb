#include <iostream>
#include <chrono>
#include "BuzzDB.h"
#include "QueryParser.h"
#include "QueryExecutor.h"

int main() {
    BuzzDB db;

    // Start the transaction
    auto start = std::chrono::high_resolution_clock::now();
    auto millies = start.time_since_epoch();
    auto txnStartTime = std::chrono::duration_cast<std::chrono::milliseconds>(millies).count();

    // Example operations
    db.insert(6, 93, txnStartTime);
    db.printTuples(txnStartTime);
    db.executeQueries();

    // Calculate and print the elapsed time
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::micro> elapsed = end - start;
    std::cout << "Elapsed time: " << elapsed.count() << " microseconds" << std::endl;

    return 0;
}