/*
 * H9 project
 *
 * Created by crowx on 2023-09-06.
 *
 * Copyright (C) 2023 Kamil Palkowski. All rights reserved.
 */

#ifndef H9_BUS_H
#define H9_BUS_H

#include "config.h"
#include <atomic>
#include <list>
#include <map>
#include <queue>
#include <thread>
#include "bus_driver.h"
#include "h9framecomparator.h"
#include "frameobserver.h"
#include "framesubject.h"


template<class IOEventQueue>
class Bus: public FrameSubject {
private:
    struct BusFrameLess {
        bool operator() (const BusFrame *x, const BusFrame *y) const {
            return x->priority() > y->priority(); //HIGH = 0, LOW = 1
        }
    };

    std::atomic_bool run;
    std::map<int, BusDriver*> bus;

    IOEventQueue event_notificator;
    std::thread recv_thread_desc;

    std::priority_queue<BusFrame*, std::vector<BusFrame*>, BusFrameLess> send_queue;
    std::mutex send_queue_mtx;

    std::list<BusFrame*> send_orphans;
    std::mutex send_orphans_mtx;

    void recv_thread();
public:
    Bus();
    Bus(const Bus&) = delete;
    Bus& operator=(const Bus&) = delete;
    ~Bus();

    void add_driver(BusDriver *bus_driver);
    void activate();

    /// @return number
    int send_frame(ExtH9Frame frame);
    int send_frame_noblock(ExtH9Frame frame);
};


#include "bus.cc"

#endif //H9_BUS_H
