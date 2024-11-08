#include "Lock.h"

void Lock::acquire() {
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, [this]() { return !is_locked; });
    is_locked = true;
}

void Lock::release() {
    std::lock_guard<std::mutex> lock(mtx);
    is_locked = false;
    cv.notify_one();
}