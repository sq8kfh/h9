/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-11-23.
 *
 * Copyright (C) 2020-2023 Kamil Palkowski. All rights reserved.
 */

#include "framesubject.h"

#include "frameobserver.h"

void FrameSubject::attach_frame_observer(FrameObserver* observer, H9FrameComparator comparator) {
    frame_observers_mtx.lock();
    frame_observers[comparator].push_back(observer);
    frame_observers_mtx.unlock();
}

void FrameSubject::detach_frame_observer(FrameObserver* observer) {
    frame_observers_mtx.lock();
    for (auto it = frame_observers.begin(); it != frame_observers.end();) {
        it->second.remove(observer);
        if (it->second.empty()) {
            it = frame_observers.erase(it);
        }
        else {
            ++it;
        }
    }
    frame_observers_mtx.unlock();
}

void FrameSubject::notify_frame_observer(const ExtH9Frame& frame) {
    frame_observers_mtx.lock();
    for (auto const& o : frame_observers) {
        if (o.first == frame) {
            for (auto const& observer : o.second) {
                observer->on_frame_recv(frame);
            }
        }
    }
    frame_observers_mtx.unlock();
}
