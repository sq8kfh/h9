/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-11-30.
 *
 * Copyright (C) 2020 Kamil Palkowski. All rights reserved.
 */

#ifndef H9_SHAREDMUTEX_H
#define H9_SHAREDMUTEX_H

#include "config.h"
#include <atomic>
#include <mutex>

class SharedMutex {
private:
    bool main_lock;
    std::mutex main_lock_mtx;
    std::mutex shared_counter_mtx;
    int shared_counter;
    std::condition_variable lock_all;
    std::condition_variable lock_main;
public:
    SharedMutex();
    void lock();
    void unlock();
    void lock_shared();
    void unlock_shared();
};


#endif //H9_SHAREDMUTEX_H
