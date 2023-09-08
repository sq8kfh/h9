/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-11-22.
 *
 * Copyright (C) 2020-2023 Kamil Palkowski. All rights reserved.
 */

#ifndef H9_KQUEUE_H
#define H9_KQUEUE_H

#include "config.h"
#include <sys/event.h>


class KQueue {
private:
    constexpr static int event_queue_size = 2;
    int kq;
    struct kevent tevent[event_queue_size];
public:
    constexpr static char notification_mechanism_name[] = "kqueue";

    KQueue();
    ~KQueue();

    void attach_socket(int fd);
    int wait();

    void trigger_async_event();

    bool is_socket_event(int number_of_events, int fd);
    bool is_async_event(int number_of_events);
};


#endif //H9_KQUEUE_H
