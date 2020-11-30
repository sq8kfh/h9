/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-11-30.
 *
 * Copyright (C) 2020 Kamil Palkowski. All rights reserved.
 */

#include "sharedmutex.h"


SharedMutex::SharedMutex(): main_lock(false), shared_counter(0) {
}

void SharedMutex::lock() {
    main_lock_mtx.lock();
    main_lock = true;

    std::unique_lock<std::mutex> lck(shared_counter_mtx);
    lock_main.wait(lck, [this]{ return this->shared_counter == 0; });
    lck.unlock();
}

void SharedMutex::unlock() {
    main_lock = false;
    main_lock_mtx.unlock();
    lock_all.notify_all();
}

void SharedMutex::lock_shared() {
    std::unique_lock<std::mutex> lck(main_lock_mtx);
    lock_all.wait(lck, [this]{ return !this->main_lock; });
    shared_counter_mtx.lock();
    ++shared_counter;
    shared_counter_mtx.unlock();
    lck.unlock();
}

void SharedMutex::unlock_shared() {
    shared_counter_mtx.lock();
    --shared_counter;
    shared_counter_mtx.unlock();
    lock_main.notify_one();
}
