#include <iostream>
#include <chrono>
#include <thread>
#include "BuzzDB.h"
#include "QueryParser.h"
#include "QueryExecutor.h"

int main() {
    BuzzDB db;

    // Start the transaction
    auto start = std::chrono::high_resolution_clock::now();

    // Example operations
    // get the current timestamp
    // int64_t currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    // auto t1 = std::make_unique<Transaction>(currentTime, db.buffer_manager);
    // db.insert(6, 93, t1);
    // db.insert(7, 104, t1);
    // t1->commit();

    /// sleep for 3 seconds
    // std::this_thread::sleep_for(std::chrono::seconds(2));

    // // create multiple threads to simulate multiple transactions and update the tuples
    // std::vector<std::thread> threads;
    // for(int i = 0; i < 5; i++) {
    //     threads.push_back(std::thread([&db, i](){
    //         int64_t currentTime_new = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    //         auto t2 = std::make_unique<Transaction>(currentTime_new, db.buffer_manager);
    //         db.updateTuples(7, 100, t2);
    //         t2->commit();
    //     }));
    // }

    // for(auto& thread : threads) {
    //     thread.join();
    // }

    db.printTuples();

    // int64_t currentTime_new = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    // auto t2 = std::make_unique<Transaction>(currentTime_new);
    // db.insert(8, 106, t2);
    // db.updateTuples(7, 100, t2);


    // Calculate and print the elapsed time
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::micro> elapsed = end - start;
    std::cout << "Elapsed time: " << elapsed.count() << " microseconds" << std::endl;

    return 0;
}