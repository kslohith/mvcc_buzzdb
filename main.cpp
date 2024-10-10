#include <iostream>
#include <chrono>
#include "BuzzDB.h"
#include "QueryParser.h"
#include "QueryExecutor.h"

int main() {
    BuzzDB db;

    // Start the transaction
    auto start = std::chrono::high_resolution_clock::now();

    // Example operations
    // db.insert(1, 10);
    // db.insert(2, 20);
    db.updateTuples(2, 30);
    db.printTuples();

    // Calculate and print the elapsed time
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::micro> elapsed = end - start;
    std::cout << "Elapsed time: " << elapsed.count() << " microseconds" << std::endl;

    return 0;
}