/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-11-11.
 *
 * Copyright (C) 2020 Kamil Palkowski. All rights reserved.
 */

#ifndef H9_FRAMEOBSERVER_H
#define H9_FRAMEOBSERVER_H

#include "config.h"
#include "bus/h9framecomparator.h"


class FrameSubject;

class FrameObserver {
public:
    FrameSubject* const subject;
protected:
    friend class FrameSubject;

    FrameObserver(FrameSubject *subject, H9FrameComparator comparator);
    ~FrameObserver();
    virtual void on_frame_recv(H9frame frame) = 0;
};


#endif //H9_FRAMEOBSERVER_H
