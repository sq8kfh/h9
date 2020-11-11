/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-11-11.
 *
 * Copyright (C) 2020 Kamil Palkowski. All rights reserved.
 */

#ifndef H9_BUSOBSERVER_H
#define H9_BUSOBSERVER_H

#include "config.h"
#include "bus/h9framecomparator.h"

class Bus;

class BusObserver {
protected:
    Bus* const h9bus;
public:
    BusObserver(Bus *bus, H9FrameComparator comparator);
    ~BusObserver();
    virtual void on_frame_recv(H9frame frame) = 0;
};


#endif //H9_BUSOBSERVER_H
