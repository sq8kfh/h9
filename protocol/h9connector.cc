/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-05-16.
 *
 * Copyright (C) 2019 Kamil Palkowski. All rights reserved.
 */

#include "h9connector.h"

#include <system_error>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "common/logger.h"


std::uint32_t H9Connector::recv_header() {
    static size_t recv_bytes = 0;
    std::uint32_t recv_header = 0;

    do {
        ssize_t nbyte = ::recv(sockfd, reinterpret_cast<char *>(&recv_header) + recv_bytes,
                               sizeof(recv_header) - recv_bytes, 0);
        if (nbyte <= 0) {
            if (nbyte == 0) {
                h9_log_debug("close connection during recv header");
                //return;
            }
            throw std::system_error(errno, std::generic_category(),
                                    __FILE__ + std::string(":") + std::to_string(__LINE__));
        }
        recv_bytes += nbyte;
    } while (recv_bytes != sizeof(recv_header));

    recv_bytes = 0;
    return ntohl(recv_header);
}

std::string H9Connector::recv_data(std::uint32_t data_to_read) {
    size_t buf_len = 4096;
    char* buf = new char[buf_len];
    std::string recv_data_buf;
    do {
        size_t bytes_to_recv = data_to_read - recv_data_buf.size();

        ssize_t nbyte = ::recv(sockfd, buf, buf_len < bytes_to_recv ? buf_len : bytes_to_recv, 0);
        if (nbyte <= 0) {
            if (nbyte == 0) {
                h9_log_debug("close connection during recv data");
            }
            throw std::system_error(errno, std::generic_category(),
                                    __FILE__ + std::string(":") + std::to_string(__LINE__));
        }

        recv_data_buf.append(buf, nbyte);
    } while (recv_data_buf.size() != data_to_read);

    return std::move(recv_data_buf);
}

H9Connector::H9Connector(std::string hostname, std::string port) {
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(hostname.c_str(), port.c_str(), &hints, &servinfo)) != 0) {
        throw std::system_error(errno, std::generic_category(),
                                __FILE__ + std::string(":") + std::to_string(__LINE__));
        //h9_log_err("xmlsocket: getaddrinfo: %s", gai_strerror(rv));
    }

    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            //h9_log_warn("xmlsocket: socket %s", strerror(errno));
            continue;
        }
        if (::connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            //h9_log_warn("xmlsocket: connect %s", strerror(errno));
            continue;
        }
        break;
    }

    //if (p == NULL) {
    //    h9_log_err("xmlsocket: failed to connect");
    //    return NULL;
    //}
    //
    //inet_ntop(p->ai_family, get_in_sockaddr(p->ai_addr), s, sizeof s);
    //h9_log_info("xmlsocket: connecting to %s", s);

    freeaddrinfo(servinfo);
}

H9Connector::~H9Connector() {
    ::close(sockfd);
}

int H9Connector::connect() {
    return 0;
}

GenericMsg H9Connector::recv() {
    std::uint32_t data_size = recv_header();
    std::string data = recv_data(data_size);

    return GenericMsg(data);
}

void H9Connector::send(const GenericMsg &msg) {
    std::string raw_msg = msg.serialize();
    std::uint32_t header = htonl(raw_msg.size());

    ssize_t nbyte = ::send(sockfd, &header, sizeof(header), 0);
    if (nbyte <= 0) {
        /*if (nbyte == 0) {
            return;
        }*/
        throw std::system_error(errno, std::generic_category(),
                                __FILE__ + std::string(":") + std::to_string(__LINE__));
    }

    nbyte = ::send(sockfd, raw_msg.c_str(), raw_msg.size(), 0);
    if (nbyte <= 0) {
        /*if (nbyte == 0) {
            return;
        }*/
        throw std::system_error(errno, std::generic_category(),
                                __FILE__ + std::string(":") + std::to_string(__LINE__));
    }
}
