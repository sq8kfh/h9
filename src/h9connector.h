/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-05-16.
 *
 * Copyright (C) 2019-2023 Kamil Palkowski. All rights reserved.
 */

#pragma once

#include "config.h"

#include <jsonrpcpp/jsonrpcpp.hpp>
#include <nlohmann/json.hpp>

#include "h9msgsocket.h"

class H9Connector {
  private:
    H9MsgSocket h9socket;
    int next_msg_id;
  public:
    H9Connector(std::string hostname, std::string port) noexcept;
    ~H9Connector() noexcept;

    std::string hostname() const noexcept;

    /*
     * throw: std::system_error
     */
    void connect(std::string entity);
    void close() noexcept;
    void shutdown_read() noexcept;

    jsonrpcpp::entity_ptr recv();

    void send(jsonrpcpp::entity_ptr msg);

    int get_next_id() noexcept;
};
