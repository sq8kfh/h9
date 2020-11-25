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
    h9socket(std::move(hostname), std::move(port)) {
}

H9Connector::~H9Connector() noexcept {
    h9socket.close();
}

int H9Connector::connect(std::string entity) noexcept {
    h9socket.connect();
    h9socket.authentication(entity);
    return 0;
}

void H9Connector::close() noexcept {
    h9socket.close();
}

void H9Connector::shutdown_read() noexcept {
    h9socket.shutdown_read();
}

GenericMsg H9Connector::recv() {
    GenericMsg msg;
    int res = h9socket.recv_complete_msg(msg);
    if (res <= 0) {
        throw std::system_error(errno, std::generic_category(), __FILE__ + std::string(":") + std::to_string(__LINE__));
    }
    return std::move(msg);
}

std::uint64_t H9Connector::send(GenericMsg msg) {
    if (h9socket.send(msg) <=0) {
        throw std::system_error(errno, std::generic_category(), __FILE__ + std::string(":") + std::to_string(__LINE__));
    }
    return msg.get_id();
}

std::uint64_t H9Connector::send(GenericMsg msg, std::uint64_t id) {
    if (h9socket.send(msg, id) <=0) {
        throw std::system_error(errno, std::generic_category(), __FILE__ + std::string(":") + std::to_string(__LINE__));
    }
    return id;
}

std::uint64_t H9Connector::get_next_id() noexcept {
    return h9socket.get_next_id();
}
