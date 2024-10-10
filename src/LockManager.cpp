#include "LockManager.h"

std::shared_ptr<Lock> LockManager::getLock(int page_number, int slot_id) {
    int key = page_number + slot_id;
    std::lock_guard<std::mutex> lock(mtx);
    if (lock_table.find(key) == lock_table.end()) {
        lock_table[key] = std::make_shared<Lock>();
    }
    return lock_table[key];
}