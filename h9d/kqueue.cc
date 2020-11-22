/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-11-22.
 *
 * Copyright (C) 2020 Kamil Palkowski. All rights reserved.
 */

#include "kqueue.h"
#include <string>
#include <stdexcept>
#include <system_error>
#include <unistd.h>


KQueue::KQueue(): kq(-1) {
    kq = kqueue();
    if (kq	== -1) {
        throw std::system_error(errno, std::generic_category(), __FILE__ + std::string(":") + std::to_string(__LINE__));
    }

    struct kevent event;
    EV_SET(&event, 1, EVFILT_USER, EV_ADD | EV_CLEAR, 0, 0, NULL);
    int ret = kevent(kq, &event, 1, NULL, 0, NULL);
    if (ret == -1) {
        throw std::system_error(errno, std::generic_category(), __FILE__ + std::string(":") + std::to_string(__LINE__));
    }
    if (event.flags & EV_ERROR) {
        throw std::runtime_error(
                __FILE__ + std::string(":") + std::to_string(__LINE__) + std::string(" Event error: ") +
                std::string(strerror(event.data)));
    }
}

KQueue::~KQueue() {
    if (kq > -1 ) {
        close(kq);
    }
}

void KQueue::attach_read_event(int fd) {
    struct kevent event;
    EV_SET(&event, fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
    int ret = kevent(kq, &event, 1, NULL, 0, NULL);
    if (ret == -1) {
        throw std::system_error(errno, std::generic_category(), __FILE__ + std::string(":") + std::to_string(__LINE__));
    }
    if (event.flags & EV_ERROR) {
        throw std::runtime_error(
                __FILE__ + std::string(":") + std::to_string(__LINE__) + std::string(" Event error: ") +
                std::string(strerror(event.data)));
    }
}

int KQueue::wait() {
    int ret = kevent(kq, NULL, 0, tevent, event_number, NULL);
    if	(ret ==	-1) {
        throw std::system_error(errno, std::generic_category(), __FILE__ + std::string(":") + std::to_string(__LINE__));
    }

    return ret;
}

void KQueue::trigger_async_event() {
    struct kevent event;
    EV_SET(&event, 1, EVFILT_USER, 0, NOTE_TRIGGER, 0, NULL);
    int ret = kevent(kq, &event, 1, NULL, 0, NULL);
    if (ret == -1) {
        throw std::system_error(errno, std::generic_category(), __FILE__ + std::string(":") + std::to_string(__LINE__));
    }
    if (event.flags & EV_ERROR) {
        throw std::runtime_error(
                __FILE__ + std::string(":") + std::to_string(__LINE__) + std::string(" Event error: ") +
                std::string(strerror(event.data)));
    }
}

bool KQueue::is_socket_ready(int event_num, int fd) {
    if (fd == (int)tevent[event_num].ident) return true;
    return false;
}

bool KQueue::is_async_event(int event_num) {
    return tevent[event_num].filter == EVFILT_USER;
}
