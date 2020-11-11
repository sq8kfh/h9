/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-11-11.
 *
 * Copyright (C) 2020 Kamil Palkowski. All rights reserved.
 */

#include "busobserver.h"
#include "bus.h"


BusObserver::BusObserver(Bus *bus, H9FrameComparator comparator): h9bus(bus) {
    h9bus->attach_frame_observer(this, comparator);
}

BusObserver::~BusObserver() {
    h9bus->detach_frame_observer(this);
}
