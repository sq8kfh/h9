/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-05-16.
 *
 * Copyright (C) 2019-2023 Kamil Palkowski. All rights reserved.
 */

#include "h9connector.h"

#include <system_error>
#include <unistd.h>

H9Connector::H9Connector(std::string hostname, std::string port) noexcept:
    h9socket(std::move(hostname), std::move(port)),
    next_msg_id(1) {
}

H9Connector::~H9Connector() noexcept {
    h9socket.close();
}

std::string H9Connector::hostname() const noexcept {
    return h9socket.get_remote_address();
}

void H9Connector::connect(std::string entity) {
    if (h9socket.connect() < 0) {
        throw std::system_error(errno, std::system_category(), __FILE__ + std::string(":") + std::to_string(__LINE__));
    }
    int res = h9socket.authentication(entity);
    if (res != 1) {
        throw std::runtime_error("Authentication fail");
    }
}

void H9Connector::close() noexcept {
    h9socket.close();
}

void H9Connector::shutdown_read() noexcept {
    h9socket.shutdown_read();
}

jsonrpcpp::entity_ptr H9Connector::recv() {
    nlohmann::json json;
    int res = h9socket.recv_complete_msg(json);
    if (res < 0) {
        throw std::system_error(errno, std::system_category(), __FILE__ + std::string(":") + std::to_string(__LINE__));
    }
    else if (res == 0) {
        throw std::runtime_error("Connection closed");
    }

    if (json.is_discarded()) {
        throw std::runtime_error("Invalid JSON");
    }

    jsonrpcpp::Parser parser;
    jsonrpcpp::entity_ptr msg = parser.parse_json(json);

    return std::move(msg);
}

void H9Connector::send(jsonrpcpp::entity_ptr msg) {
    if (h9socket.send(msg->to_json()) <= 0) {
        throw std::system_error(errno, std::system_category(), __FILE__ + std::string(":") + std::to_string(__LINE__));
    }
}

int H9Connector::get_next_id() noexcept {
    ++next_msg_id; // number 1 reserved for authenticate call
    return next_msg_id;
}
