/*
 * H9 project
 *
 * Created by crowx on 2023-09-06.
 *
 * Copyright (C) 2023 Kamil Palkowski. All rights reserved.
 */

#pragma once

#include "config.h"

#include <atomic>
#include <list>
#include <map>
#include <queue>
#include <spdlog/spdlog.h>
#include <thread>

#include "bus_driver.h"
#include "frameobserver.h"
#include "framesubject.h"
#include "h9framecomparator.h"
#include "metrics_collector.h"
#if (defined(__unix__) && defined(BSD)) || defined(__APPLE__) && defined(__MACH__)
#include "kqueue.h"
#elif defined(__linux__)
#include "epoll.h"
#endif

class Bus: public FrameSubject {
  private:
#if (defined(__unix__) && defined(BSD)) || (defined(__APPLE__) && defined(__MACH__))
    using IOEventQueue = KQueue;
#else
    using IOEventQueue = Epoll;
#endif

    struct BusFrameLess {
        bool operator()(const BusFrame* x, const BusFrame* y) const {
            return x->priority() > y->priority(); // HIGH = 0, LOW = 1
        }
    };

    std::shared_ptr<spdlog::logger> logger;
    std::shared_ptr<spdlog::logger> frames_logger;
    std::shared_ptr<spdlog::logger> frames_recv_file_logger;
    std::shared_ptr<spdlog::logger> frames_sent_file_logger;

    std::atomic_bool run;
    std::map<int, BusDriver*> bus;

    IOEventQueue event_notificator;
    std::thread recv_thread_desc;

    std::priority_queue<BusFrame*, std::vector<BusFrame*>, BusFrameLess> send_queue;
    std::mutex send_queue_mtx;

    std::list<BusFrame*> send_orphans;
    std::mutex send_orphans_mtx;

    MetricsCollector::counter_t& sent_frames_counter;
    MetricsCollector::counter_t& received_frames_counter;

    constexpr static int number_of_frame_types = 1 << H9frame::H9FRAME_TYPE_BIT_LENGTH;
    MetricsCollector::counter_t* sent_frames_counter_by_type[number_of_frame_types];
    MetricsCollector::counter_t* received_frames_counter_by_type[number_of_frame_types];
    void recv_thread();

  public:
    Bus();
    Bus(const Bus&) = delete;
    Bus& operator=(const Bus&) = delete;
    ~Bus();

    void add_driver(BusDriver* bus_driver);
    void activate();

    /// @return number
    int send_frame(ExtH9Frame frame);
    int send_frame_noblock(ExtH9Frame frame);
};
