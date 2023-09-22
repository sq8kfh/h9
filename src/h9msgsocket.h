/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-11-20.
 *
 * Copyright (C) 2020-2023 Kamil Palkowski. All rights reserved.
 */

#pragma once

#include "config.h"
#include <string>
#include "h9socket.h"
#include <nlohmann/json.hpp>


class H9MsgSocket: protected H9Socket {
    //TODO: zmienic nazwe na json_socket, ogarnac bledy, szczegolnie z parsowaniem jsona
    //TODO: H9MsgSocket i authentication moga zrzucac wyjatki, moze przerobic to na retval? albo dedykowany typ bo moga am byc wyjatki z parsera jsona (authentication)
public:
    explicit H9MsgSocket(int socket);
    H9MsgSocket(std::string hostname, std::string port) noexcept;

    H9MsgSocket(const H9MsgSocket&) = delete;
    H9MsgSocket(H9MsgSocket&&) = delete;
    H9MsgSocket &operator=(const H9MsgSocket&) = delete;

    int get_socket() noexcept;

    using H9Socket::connect;
    int authentication(const std::string& entity);

    int send(const nlohmann::json &json) noexcept;

    /// @param[out] json Recv json object, if incorrect json.is_discarded() return true
    /// @param[in] timeout_in_seconds If >0 set recv timeout
    /// @retval -1 an error occurred and the global variable 'errno' is set to indicate the error.
    /// @retval 0 an connection closed
    /// @retval 1 on successful
    int recv(nlohmann::json &json, int timeout_in_seconds = 0) noexcept;

    /// @param[out] json Recv json object, if incorrect json.is_discarded() return true
    /// @retval -1 an error occurred and the global variable 'errno' is set to indicate the error.
    /// @retval 0 an connection closed
    /// @retval 1 on successful
    int recv_complete_msg(nlohmann::json &json) noexcept;

    using H9Socket::close;
    void shutdown_read() noexcept;

    using H9Socket::get_remote_address;
    using H9Socket::get_remote_port;
};
