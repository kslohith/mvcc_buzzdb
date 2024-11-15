#include <iostream>
#include <chrono>
#include <thread>
#include "BuzzDB.h"
#include "QueryParser.h"
#include "QueryExecutor.h"
#include "ConcurrencyControl.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <concurrency_control_mode>\n";
        return 1;
    }

    std::string mode = argv[1];

    ConcurrencyControl cc_mode;
    if (mode == "2PL") {
        cc_mode = ConcurrencyControl::MV2PL;
    } else if (mode == "OCC") {
        cc_mode = ConcurrencyControl::MVOCC;
    } else {
        throw std::invalid_argument("Invalid concurrency control mode");
    }

    BuzzDB db(cc_mode);

    // Start the transaction
    auto start = std::chrono::high_resolution_clock::now();


    {
        /// Testing Concurrency
        /// Test 1: Lost Updates

        /// Insert two tuples
        int64_t currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        auto t1 = std::make_unique<Transaction>(currentTime, db.buffer_manager, db.version_manager, db.transaction_manager, db.lock_manager, ConcurrencyControl::MV2PL);
        db.insert(6, 93, t1);
        db.insert(7, 104, t1);
        t1->commit();


        /// Create 2 threads and let them update the same tuple, resultant state should the sum of both operations.
        std::vector<std::thread> threads;
        for(int i = 0; i < 5; i++) {
            threads.push_back(std::thread([&db, i](){
                int64_t currentTime_new = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
                auto t2 = std::make_unique<Transaction>(currentTime_new, db.buffer_manager, db.version_manager, db.transaction_manager, db.lock_manager, ConcurrencyControl::MV2PL);
                db.updateTuples(7, i+1, t2);
                std::this_thread::sleep_for(std::chrono::seconds(2));
                int result = t2->commit();
                while(result == -1) {
                    std::cout << "Transaction failed: Conflict" << t2->transaction_id << std::endl;
                    int64_t time_new = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
                    auto t2_new = std::make_unique<Transaction>(time_new, db.buffer_manager, db.version_manager, db.transaction_manager, db.lock_manager, ConcurrencyControl::MV2PL);
                    db.updateTuples(7, i+1, t2_new);
                    result = t2_new->commit();
                }
            }));
        }

        for(auto& thread : threads) {
            thread.join();
        }

        db.printTuples();

        /// Sleep for 2 seconds
        // std::this_thread::sleep_for(std::chrono::seconds(2));


        /// Test 2: Dirty Reads
        /// create 2 threads, let one thread update the value of a tuple and let the other tuple just read the value and print it.
        // std::vector<std::thread> thread_test2;
        // for(int i = 0; i < 2; i++) {
        //     thread_test2.push_back(std::thread([&db, i](){
        //         if(i == 1){
        //             int64_t currentTime_new = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        //             auto t2 = std::make_unique<Transaction>(currentTime_new, db.buffer_manager, db.version_manager, db.transaction_manager, db.lock_manager, ConcurrencyControl::MV2PL);
        //             db.updateTuples(7, i+1, t2);
        //             std::this_thread::sleep_for(std::chrono::seconds(2));
        //             int result = t2->commit();
        //             while(result == -1) {
        //                 std::cout << "Transaction failed: Conflict" << t2->transaction_id << std::endl;
        //                 int64_t time_new = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        //                 auto t2_new = std::make_unique<Transaction>(time_new, db.buffer_manager, db.version_manager, db.transaction_manager, db.lock_manager, ConcurrencyControl::MV2PL);
        //                 db.updateTuples(7, i+1, t2_new);
        //                 result = t2_new->commit();
        //             }
        //         }
        //         else{
        //             db.printTuples();
        //         }
        //     }));
        // }

        // for(auto& thread : thread_test2) {
        //     thread.join();
        // }

        //db.printTuples();
    }


    // Calculate and print the elapsed time
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::micro> elapsed = end - start;
    std::cout << "Elapsed time: " << elapsed.count() << " microseconds" << std::endl;

    return 0;
}