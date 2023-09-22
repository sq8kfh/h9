/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-07-01.
 *
 * Copyright (C) 2019-2023 Kamil Palkowski. All rights reserved.
 */

#pragma once

#include "config.h"

#include <string>

class H9Socket {
  private:
    std::uint32_t header_buf;
    char* data_buf;
    size_t data_buf_len;
    ssize_t recv_bytes;
    ssize_t bytes_to_recv;
    H9Socket() noexcept;

  protected:
    int _socket;
    std::string _hostname;
    std::string _port;

  public:
    explicit H9Socket(int socket) noexcept;
    H9Socket(std::string hostname, std::string port) noexcept;
    H9Socket(const H9Socket& a) = delete;

    ~H9Socket() noexcept;
    /// @retval <-1 an error occurred and the global variable 'errno' is set to indicate the error.
    /// @retval 0 an successful
    int connect() noexcept;
    void close() noexcept;

    /// @param[out] buf
    /// @param[in] timeout_in_seconds If >0 set recv timeout
    /// @retval -1 an error occurred and the global variable 'errno' is set to indicate the error.
    /// @retval 0 an connection closed
    /// @retval 1 on successful
    int recv(std::string& buf, int timeout_in_seconds = 0) noexcept;

    /// @retval -1 an error occurred and the global variable 'errno' is set to indicate the error.
    /// @retval 0 an connection closed
    /// @retval 1 on successful
    int send(const std::string& buf) noexcept;

    std::string get_remote_address() const noexcept;
    std::string get_remote_port() const noexcept;
};
