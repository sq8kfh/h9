/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-05-10.
 *
 * Copyright (C) 2019-2020 Kamil Palkowski. All rights reserved.
 */

#include "socketmgr.h"

#include <algorithm>
#include <system_error>
#include <string>
#include <iostream>
#include <cassert>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "common/logger.h"

SocketMgr::SocketMgr() noexcept {
    FD_ZERO(&event_socket_set);
    socket_max = 0;
}

void SocketMgr::register_socket(Socket *socket) noexcept {
    assert(socket->get_socket());

    if (socket->get_socket() != 0) {
        FD_SET(socket->get_socket(), &event_socket_set);
        socket_set.insert(socket);
        if (socket->get_socket() > socket_max) {
            socket_max = socket->get_socket();
        }
    }
}

void SocketMgr::unregister_socket(Socket *socket) noexcept {
    //assert(socket->get_socket());

    if (socket->get_socket() != 0) {
        FD_CLR(socket->get_socket(), &event_socket_set);
        socket_set.erase(socket);
        if (socket->get_socket() >= socket_max) {
            socket_max = 0;
            std::for_each(socket_set.begin(), socket_set.end(), [this](Socket* s) {
                if (s->get_socket() > socket_max) {
                    socket_max = s->get_socket();
                }
            });
            socket_max = socket->get_socket();
        }
    }
}

void SocketMgr::select_loop(std::function<void(void)> after_select_callback, std::function<void(void)> cron_func) {
    while (!socket_set.empty()) {
        h9_log_debug2("Select next loop (size of socket set: %d)", socket_set.size());
        fd_set rfds;
        rfds = event_socket_set;

        struct timeval tv;
        /*h9d_select_event_t tmp;
        fd_set rfds;

        time_t t = time_trigger_period + last_time - time(NULL);
        tv.tv_sec = t > 0 ? t : 0;*/
        tv.tv_sec = 15;
        tv.tv_usec = 0;

        int retval = select(socket_max+1, &rfds, nullptr, nullptr, &tv);

        if (retval == -1) {
            if (errno == EINTR) { //received a signal (e.g. SIGHUP for log rotate)
                h9_log_warn("Interrupted system call during a 'select'");
                continue;
            }
            throw std::system_error(errno, std::generic_category(), __FILE__ + std::string(":") + std::to_string(__LINE__));
        }
        else if (retval) {
            //for (auto it = socket_set.begin(); it != socket_set.end(); ++it) {
            for (auto it : socket_set) {
                //auto it_local = it;
                //++it;
                if (FD_ISSET(it->get_socket(), &rfds)) {
                    try {
                        it->on_select();
                        //it_local->second->on_select();
                    }
                    catch (SocketMgr::Socket::CloseSocketException& e) {
                        //h9_log_info("Close socket in select loop (socket: %d)", it_local->first);
                        //it_local->second->on_close();
                        h9_log_info("Close socket in select loop (socket: %d)", it->get_socket());
                        it->on_close();
                    }
                }
            }
        }
        else if (cron_func) {
            cron_func();
        }
        if (after_select_callback) {
            after_select_callback();
        }
    }
}
