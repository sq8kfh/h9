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
#include "protocol/identificationmsg.h"


class H9MsgSocket: protected H9Socket {
    std::uint64_t next_msg_id;
public:
    explicit H9MsgSocket(int socket) noexcept;
    H9MsgSocket(std::string hostname, std::string port) noexcept;

    int get_socket() noexcept;

    using H9Socket::connect;
    int authentication(const std::string& entity) noexcept;

    int send(GenericMsg &msg) noexcept;
    int send(GenericMsg &msg, std::uint64_t id) noexcept;
    int recv(GenericMsg &msg, int timeout_in_seconds = 0) noexcept;
    int recv_complete_msg(GenericMsg &msg) noexcept;

    using H9Socket::close;
    void shutdown_read() noexcept;

    std::uint64_t get_next_id() noexcept;
    using H9Socket::get_remote_address;
    using H9Socket::get_remote_port;
};


#endif //H9_H9MSGSOCKET_H
