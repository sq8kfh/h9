/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-07-01.
 *
 * Copyright (C) 2019 Kamil Palkowski. All rights reserved.
 */

#ifndef H9_H9SOCKET_H
#define H9_H9SOCKET_H

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
    H9Socket(int socket) noexcept;
    H9Socket(std::string hostname, std::string port) noexcept;
    ~H9Socket() noexcept;
    int connect() noexcept;
    void close();
    int recv(std::string& buf, int timeout_in_seconds = 0) noexcept;
    int send(const std::string& buf) noexcept;
    std::string get_remote_address() noexcept;
    std::string get_remote_port() noexcept;
};


#endif //H9_H9SOCKET_H
