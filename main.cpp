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


    {
        /// Testing Concurrency
        /// Test 1: Lost Updates

        /// Insert two tuples
        int64_t currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        auto t1 = std::make_unique<Transaction>(currentTime, db.buffer_manager, db.version_manager, db.transaction_manager);
        db.insert(6, 93, t1);
        db.insert(7, 104, t1);
        t1->commit();

        /// give 2 seconds sleep time
        std::this_thread::sleep_for(std::chrono::seconds(2));

        /// Create 2 threads and let them update the same tuple, resultant state should the sum of both operations.
        std::vector<std::thread> threads;
        for(int i = 0; i < 2; i++) {
            threads.push_back(std::thread([&db, i](){
                int64_t currentTime_new = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
                auto t2 = std::make_unique<Transaction>(currentTime_new, db.buffer_manager, db.version_manager, db.transaction_manager);
                db.updateTuples(7, i+1, t2);
                std::this_thread::sleep_for(std::chrono::seconds(2));
                int result = t2->commit();
                if(result == -1) {
                    std::this_thread::sleep_for(std::chrono::seconds(1));
                    std::cout << "Transaction failed: Conflict" << t2->transaction_id << std::endl;
                    int64_t time_new = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
                    auto t2_new = std::make_unique<Transaction>(time_new, db.buffer_manager, db.version_manager, db.transaction_manager);
                    db.updateTuples(7, i+1, t2_new);
                    if(t2_new->commit() == 1) {
                        std::cout << "Transaction commited: " << t2_new->transaction_id << std::endl;
                    }
                }
            }));
        }

        for(auto& thread : threads) {
            thread.join();
        }

        db.printTuples();
    }

    // Example operations
    // get the current timestamp
    // int64_t currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    // auto t1 = std::make_unique<Transaction>(currentTime, db.buffer_manager);
    // db.insert(6, 93, t1);
    // db.insert(7, 104, t1);

    // t1->commit();

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