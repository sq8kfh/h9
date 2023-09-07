/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-11-22.
 *
 * Copyright (C) 2020 Kamil Palkowski. All rights reserved.
 */

#ifndef H9_EPOLL_H
#define H9_EPOLL_H

#include "config.h"
#include <sys/epoll.h>


class Epoll {
private:
    constexpr static int event_number = 2;
    int epoll;
    struct epoll_event tevent[event_number];
    int event_fd;
public:
    Epoll();
    ~Epoll();

    void attach_socket(int fd);
    int wait();

    void trigger_async_event();

    bool is_socket_event(int event_num, int fd);
    bool is_async_event(int event_num);
};


#endif //H9_EPOLL_H
