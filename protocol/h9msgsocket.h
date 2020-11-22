/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-11-20.
 *
 * Copyright (C) 2020 Kamil Palkowski. All rights reserved.
 */

#ifndef H9_H9MSGSOCKET_H
#define H9_H9MSGSOCKET_H

#include "config.h"
#include <string>
#include "h9socket.h"
#include "genericmsg.h"


class H9MsgSocket: protected H9Socket {
public:
    explicit H9MsgSocket(int socket);
    H9MsgSocket(std::string hostname, std::string port);

    int get_socket();

    int send(GenericMsg &msg) noexcept;
    int send(GenericMsg &msg, std::uint64_t id) noexcept;
    int recv(GenericMsg &msg, int timeout_in_seconds = 0) noexcept;
    int recv_complete_msg(GenericMsg &msg) noexcept;

    void close() noexcept;
    void shutdown_read() noexcept;

    std::uint64_t get_next_id() noexcept;
    std::string get_remote_address() noexcept;
    std::string get_remote_port() noexcept;
};


#endif //H9_H9MSGSOCKET_H
