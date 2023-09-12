/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-11-20.
 *
 * Copyright (C) 2020 Kamil Palkowski. All rights reserved.
 */

#include "h9msgsocket.h"
#include <system_error>
#include <sys/errno.h>
#include <sys/socket.h>


H9MsgSocket::H9MsgSocket(int socket): H9Socket(socket), next_msg_id(0)  {
    if (connect() < 0) //this should never happen if yes something is wrong with socket
        throw std::system_error(errno, std::generic_category(), __FILE__ + std::string(":") + std::to_string(__LINE__));
}

H9MsgSocket::H9MsgSocket(std::string hostname, std::string port) noexcept: H9Socket(std::move(hostname), std::move(port)), next_msg_id(0)  {
}

int H9MsgSocket::get_socket() noexcept {
    return _socket;
}

int H9MsgSocket::authentication(const std::string& entity) noexcept {
    //IdentificationMsg ident_msg(entity);
//    if (send(ident_msg) <= 0) {
//        return -1;
//    }
    //return 0; //authentication fail
    return 1; //if ok
}

int H9MsgSocket::send(const nlohmann::json& json) noexcept {
    return H9Socket::send(json.dump());
}

int H9MsgSocket::recv(nlohmann::json &json, int timeout_in_seconds) noexcept {
    std::string raw_str;
    int res = H9Socket::recv(raw_str, timeout_in_seconds);
    if (res > 0) {
        json = std::move(nlohmann::json::parse(std::move(raw_str), nullptr, false));
    }
    return res;
}

int H9MsgSocket::recv_complete_msg(nlohmann::json &json) noexcept {
    while (true) {
        int res = recv(json, 0);
        if (res < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
            continue;
        }
        else {
            return res;
        }
    }
}

void H9MsgSocket::shutdown_read() noexcept {
    shutdown(_socket, SHUT_RD);
}

std::uint64_t H9MsgSocket::get_next_id() noexcept {
    if (next_msg_id == 0) next_msg_id = 1;
    else ++next_msg_id;
    //TODO: randomize id ??
    //std::hash<std::uint64_t> hasher;
    //return hasher(next);
    return next_msg_id;
}
