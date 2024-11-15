#ifndef LOCK_MANAGER_H
#define LOCK_MANAGER_H

#include <unordered_map>
#include <memory>
#include "Lock.h"
#include <iostream>

class LockManager {
public:
    /// @brief get the Lock on a particular tuple
    /// @param page_number 
    /// @param slot_id 
    /// @return  
    std::shared_ptr<Lock> getLock(int page_number, int slot_id);
    LockManager() {
        // Initialize the lock_table
        lock_table = std::unordered_map<int, std::shared_ptr<Lock>>();
        std::cout << "LockManager constructed. Bucket count: " << lock_table.bucket_count() << std::endl;
    }

private:
    std::unordered_map<int, std::shared_ptr<Lock>> lock_table;
    std::mutex mtx;
};

#endif