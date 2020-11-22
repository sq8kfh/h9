/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-11-22.
 *
 * Copyright (C) 2020 Kamil Palkowski. All rights reserved.
 */

#ifndef H9_KQUEUE_H
#define H9_KQUEUE_H

#include "config.h"
#include <sys/event.h>


class KQueue {
private:
    constexpr static int event_number = 2;
    int kq;
    struct kevent tevent[event_number];
public:
    KQueue();
    ~KQueue();

    void attach_read_event(int fd);
    int wait();

    void trigger_async_event();

    bool is_socket_ready(int event_num, int fd);
    bool is_async_event(int event_num);
};


#endif //H9_KQUEUE_H
