/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-05-10.
 *
 * Copyright (C) 2019 Kamil Palkowski. All rights reserved.
 */

#include "socketmgr.h"

#include <system_error>
#include <string>
#include <iostream>
#include <cassert>

#include "common/logger.h"

SocketMgr::SocketMgr() {
    FD_ZERO(&event_socket_set);
}

void SocketMgr::register_socket(Socket *socket) {
    assert(socket->get_socket());

    if (socket->get_socket() != 0) {
        FD_SET(socket->get_socket(), &event_socket_set);
        socket_map[socket->get_socket()] = socket;
    }
}

void SocketMgr::unregister_socket(Socket *socket) {
    assert(socket->get_socket());

    if (socket->get_socket() != 0) {
        FD_CLR(socket->get_socket(), &event_socket_set);
        socket_map.erase(socket->get_socket());
    }
}

void SocketMgr::select_loop(std::function<void(void)> after_select_callback, std::function<void(void)> cron_func) {
    while (!socket_map.empty()) {
        //h9_log_debug("select loop (socket list size: %d)", socket_map.size());
        fd_set rfds;
        FD_COPY(&event_socket_set, &rfds);

        struct timeval tv;
        /*h9d_select_event_t tmp;
        fd_set rfds;

        time_t t = time_trigger_period + last_time - time(NULL);
        tv.tv_sec = t > 0 ? t : 0;*/
        tv.tv_sec = 15;
        tv.tv_usec = 0;

        int retval = select(socket_map.rbegin()->first+1, &rfds, nullptr, nullptr, &tv);

        if (retval == -1) {
            throw std::system_error(errno, std::generic_category(), __FILE__ + std::string(":") + std::to_string(__LINE__));
        }
        else if (retval) {
            for (auto it = socket_map.begin(); it != socket_map.end();) {
                auto it_local = it;
                ++it;
                if (FD_ISSET(it_local->first, &rfds)) {
                    try {
                        it_local->second->on_select();
                    }
                    catch (SocketMgr::Socket::CloseSocketException& e) {
                        h9_log_info("Close socket in select loop (socket: %d)", it_local->first);
                        it_local->second->close();
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
