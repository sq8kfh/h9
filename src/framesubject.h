/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-11-23.
 *
 * Copyright (C) 2020-2023 Kamil Palkowski. All rights reserved.
 */

#ifndef H9_FRAMESUBJECT_H
#define H9_FRAMESUBJECT_H

#include "config.h"
#include <map>
#include <mutex>
#include <list>
#include "frameobserver.h"
#include "h9framecomparator.h"


class FrameSubject {
private:
    std::recursive_mutex frame_observers_mtx;
    std::map<H9FrameComparator, std::list<FrameObserver*>> frame_observers;

    friend FrameObserver::FrameObserver(FrameSubject *subject, H9FrameComparator comparator);
    friend FrameObserver::~FrameObserver();

    void attach_frame_observer(FrameObserver *observer, H9FrameComparator comparator);
    void detach_frame_observer(FrameObserver *observer);
protected:
    void notify_frame_observer(const ExtH9Frame &frame);
};


#endif //H9_FRAMESUBJECT_H
