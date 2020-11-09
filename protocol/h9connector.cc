/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-05-16.
 *
 * Copyright (C) 2019-2020 Kamil Palkowski. All rights reserved.
 */

#include "h9connector.h"

#include <system_error>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "common/logger.h"


H9Connector::H9Connector(std::string hostname, std::string port) noexcept:
    H9Socket(std::move(hostname), std::move(port)) {
}

H9Connector::~H9Connector() noexcept {
}

int H9Connector::connect() noexcept {
    return H9Socket::connect();
}

std::uint64_t H9Connector::get_next_id(void) {
    //TODO: randomize id
    static std::uint64_t next = 0;
    if (next == 0) next = 1;
    else ++next;
    return next;
}

GenericMsg H9Connector::recv(int timeout_in_seconds) {
    std::string data;

    while (true) {
        if (H9Socket::recv(data, timeout_in_seconds) <= 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) { //incomplete message
                if (timeout_in_seconds) {
                    break;
                }
                continue;
            }
            throw std::system_error(errno, std::generic_category(), __FILE__ + std::string(":") + std::to_string(__LINE__));
        }
        else {
            break;
        }
    }
    return GenericMsg(data);
}

void H9Connector::send(GenericMsg msg, std::uint64_t msg_id) {
    if (msg_id == 0)
        msg.set_id(get_next_id());
    else
        msg.set_id(msg_id);
    std::string raw_msg = msg.serialize();
    if (H9Socket::send(raw_msg) <=0) {
        throw std::system_error(errno, std::generic_category(), __FILE__ + std::string(":") + std::to_string(__LINE__));
    }
}
