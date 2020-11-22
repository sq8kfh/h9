/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-11-20.
 *
 * Copyright (C) 2020 Kamil Palkowski. All rights reserved.
 */

#include "h9msgsocket.h"
#include <functional>
#include <system_error>
#include <sys/errno.h>
#include <sys/socket.h>


H9MsgSocket::H9MsgSocket(int socket): H9Socket(socket)  {
    if (connect() < 0)
        throw std::system_error(errno, std::generic_category(), __FILE__ + std::string(":") + std::to_string(__LINE__));
}

H9MsgSocket::H9MsgSocket(std::string hostname, std::string port): H9Socket(std::move(hostname), std::move(port))  {
    if (connect() < 0)
        throw std::system_error(errno, std::generic_category(), __FILE__ + std::string(":") + std::to_string(__LINE__));
}

int H9MsgSocket::get_socket() {
    return _socket;
}

int H9MsgSocket::send(GenericMsg &msg) noexcept {
    return send(msg, get_next_id());
}

int H9MsgSocket::send(GenericMsg &msg, std::uint64_t id) noexcept {
    msg.set_id(id);
    std::string raw_msg = msg.serialize();
    return H9Socket::send(raw_msg);
}

int H9MsgSocket::recv(GenericMsg &msg, int timeout_in_seconds) noexcept {
    std::string raw_msg;
    int res = H9Socket::recv(raw_msg, timeout_in_seconds);
    if (res > 0) {
        msg = std::move(GenericMsg(raw_msg));
    }
    return res;
}

int H9MsgSocket::recv_complete_msg(GenericMsg &msg) noexcept {
    while (true) {
        int res = recv(msg, 0);
        if (res < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
            continue;
        }
        else {
            return res;
        }
    }
}

void H9MsgSocket::close() noexcept {
    H9Socket::close();
}

void H9MsgSocket::shutdown_read() noexcept {
    shutdown(_socket, SHUT_RD);
}

std::uint64_t H9MsgSocket::get_next_id() noexcept {
    static std::uint64_t next = 0;
    if (next == 0) next = 1;
    else ++next;
    //TODO: randomize id ??
    //std::hash<std::uint64_t> hasher;
    //return hasher(next);
    return next;
}

std::string H9MsgSocket::get_remote_address() noexcept {
    return _hostname;
}

std::string H9MsgSocket::get_remote_port() noexcept {
    return _port;
}
