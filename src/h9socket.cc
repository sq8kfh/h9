/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-07-01.
 *
 * Copyright (C) 2019-2023 Kamil Palkowski. All rights reserved.
 */

#include "h9socket.h"

#include <system_error>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>


H9Socket::H9Socket() noexcept {
    _socket = -1;
    bytes_to_recv = sizeof(header_buf);
    recv_bytes = 0;

    data_buf_len = 4096;
    data_buf = (char*)malloc((data_buf_len + 1) * sizeof(char));
}

H9Socket::H9Socket(int socket) noexcept: H9Socket() {
    _socket = socket;
    _hostname.erase();
    _port.erase();
}

H9Socket::H9Socket(std::string hostname, std::string port) noexcept: H9Socket() {
    _hostname = std::move(hostname);
    _port = std::move(port);
}

H9Socket::~H9Socket() noexcept {
    ::close(_socket);
}

int H9Socket::connect() noexcept {
    int ret = 0;
    if(_socket > -1) {
        struct sockaddr_storage addr;
        char ipstr[INET6_ADDRSTRLEN];
        int port;

        socklen_t len = sizeof(addr);
        ret = getpeername(_socket, (struct sockaddr*)&addr, &len);
        if (ret < 0) {
            return ret;
        }

        if (addr.ss_family == AF_INET) {
            struct sockaddr_in *s = (struct sockaddr_in *)&addr;
            port = ntohs(s->sin_port);
            inet_ntop(AF_INET, &s->sin_addr, ipstr, sizeof(ipstr));
        } else { // AF_INET6
            struct sockaddr_in6 *s = (struct sockaddr_in6 *)&addr;
            port = ntohs(s->sin6_port);
            inet_ntop(AF_INET6, &s->sin6_addr, ipstr, sizeof(ipstr));
        }

        _hostname = ipstr;
        _port = std::to_string(port);
    }
    else {
        struct addrinfo hints, *servinfo, *p;

        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;

        ret = getaddrinfo(_hostname.c_str(), _port.c_str(), &hints, &servinfo);
        if (ret != 0) {
            //SPDLOG_DEBUG("H9Connector: getaddrinfo: {}.", gai_strerror(ret));
            return -1;
        }

        // loop through all the results and connect to the first we can
        for(p = servinfo; p != nullptr; p = p->ai_next) {
            if ((_socket = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
                continue;
            }
            if (::connect(_socket, p->ai_addr, p->ai_addrlen) == -1) {
                ::close(_socket);
                continue;
            }
            break;
        }

        if (p == nullptr) {
            //SPDLOG_ERROR("connect to '{}' port {}: {}.", _hostname.c_str(), _port.c_str(), strerror(errno));
            return -1;
        }
        freeaddrinfo(servinfo);
    }

#if defined(__APPLE__)
    int set = 1;
    ret = setsockopt(_socket, SOL_SOCKET, SO_NOSIGPIPE, (void *)&set, sizeof(int));
    if (ret < 0) {
        return ret;
    }
#endif
    return 0;
}

void H9Socket::close() noexcept {
    if (_socket > -1) ::close(_socket);
    _socket = -1;
}

int H9Socket::recv(std::string& buf, int timeout_in_seconds) noexcept {
    if (timeout_in_seconds) {
        struct timeval tv;
        tv.tv_sec = timeout_in_seconds;
        tv.tv_usec = 0;
        if (setsockopt(_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv) < 0) {
            return -1;
        }
    }

    int recv_flags = 0;

    if (recv_bytes < 4) { //header
        ssize_t nbyte = ::recv(_socket, reinterpret_cast<char *>(&header_buf) + recv_bytes,
                sizeof(header_buf) - recv_bytes, recv_flags);
        recv_flags |= MSG_DONTWAIT;

        if (nbyte <= 0) {
//            if (nbyte == 0) {
//                SPDLOG_DEBUG("close connection during recv header.");
//            }
            return nbyte;
        }
        recv_bytes += nbyte;

        if (recv_bytes == sizeof(header_buf)) {
            ssize_t tmp = ntohl(header_buf);
            if (tmp > data_buf_len) {
                data_buf_len = tmp;
                data_buf = (char*)realloc(data_buf, (data_buf_len + 1) * sizeof(char));
            }
            bytes_to_recv += tmp;
        }
    }

    if (recv_bytes >= 4) { //data
        size_t tmp_to_recv = bytes_to_recv - recv_bytes;

        ssize_t nbyte = ::recv(_socket, &data_buf[recv_bytes - sizeof(header_buf)], tmp_to_recv, recv_flags);
        if (nbyte <= 0) {
//            if (nbyte == 0) {
//                SPDLOG_DEBUG("close connection during recv data.");
//            }
            return nbyte;
        }
        recv_bytes += nbyte;
    }

    if (recv_bytes < bytes_to_recv) {
        errno = EAGAIN; //emulate nonblocking operation
        return -1;
    }

    data_buf[recv_bytes - sizeof(header_buf)] = '\0';
    buf = data_buf;
    bytes_to_recv = sizeof(header_buf);
    recv_bytes = 0;
    return 1;
}

int H9Socket::send(const std::string& buf) noexcept {
    std::uint32_t header = htonl(buf.size());

    int send_flags = 0;
#if defined(__linux__)
    send_flags |= MSG_NOSIGNAL;
#endif

    ssize_t nbyte = ::send(_socket, &header, sizeof(header), send_flags);
    if (nbyte <= 0) {
        return nbyte;
    }

    const char* char_buf = buf.c_str();
    size_t buf_size = buf.size();
    size_t sent_byte = 0;

    while (sent_byte < buf_size) {
        nbyte = ::send(_socket, &char_buf[sent_byte], buf_size - sent_byte, send_flags);
        if (nbyte <= 0) {
            return nbyte;
        }
        sent_byte += nbyte;
    }
    return 1;
}

std::string H9Socket::get_remote_address() const noexcept {
    return _hostname;
}

std::string H9Socket::get_remote_port() const noexcept {
    return _port;
}
