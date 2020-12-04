/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-11-22.
 *
 * Copyright (C) 2020 Kamil Palkowski. All rights reserved.
 */

#include "epoll.h"


Epoll::Epoll(): epoll(-1), eventfd(-1) {
    epoll = epoll_create1(0);
    if (epoll == -1) {
        throw std::system_error(errno, std::generic_category(), __FILE__ + std::string(":") + std::to_string(__LINE__));
    }

    eventfd = eventfd(0, EFD_NONBLOCK);
    struct epoll_event event = {0};
    event.data.fd = eventfd;
    event.events = EPOLLIN | EPOLLET;
    if (epoll_ctl(epoll, EPOLL_CTL_ADD, eventfd, &event) == -1) {
        throw std::system_error(errno, std::generic_category(), __FILE__ + std::string(":") + std::to_string(__LINE__));
    }
}

Epoll::~Epoll() {
    if (epoll > -1 ) {
        close(epoll);
    }
    if (eventfd > -1 ) {
        close(eventfd);
    }
}

void Epoll::attach_socket(int fd) {
    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = fd;

    if (epoll_ctl(epoll, EPOLL_CTL_ADD, fd, &event) {
        throw std::system_error(errno, std::generic_category(), __FILE__ + std::string(":") + std::to_string(__LINE__));
    }
}

int Epoll::wait() {
    int ret = epoll_wait(epoll, tevent, event_number, -1);
    if	(ret ==	-1) {
        throw std::system_error(errno, std::generic_category(), __FILE__ + std::string(":") + std::to_string(__LINE__));
    }
    return ret;
}

void Epoll::trigger_async_event() {
    if (eventfd_write(eventfd, 1) < 0) {
        throw std::system_error(errno, std::generic_category(), __FILE__ + std::string(":") + std::to_string(__LINE__));
    }
}

bool Epoll::is_socket_event(int event_num, int fd) {
    if (fd == (int)tevent[event_num].data.fd) return true;
    return false;
}

bool Epoll::is_async_event(int event_num) {
    if (eventfd == (int)tevent[event_num].data.fd) return true;
    return false;
}
