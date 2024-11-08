// Lock.h
#ifndef LOCK_H
#define LOCK_H

#include <mutex>
#include <condition_variable>

class Lock {
public:
    void acquire();
    void release();

private:
    std::mutex mtx;
    std::condition_variable cv;
    bool is_locked = false;
};

#endif // LOCK_H