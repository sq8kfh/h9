#include <utility>

/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-05-14.
 *
 * Copyright (C) 2019-2020 Kamil Palkowski. All rights reserved.
 */

#include "tcpserver.h"

#include <system_error>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "common/logger.h"

TcpServer::TcpServer(TNewConnectionCallback new_connection_callback, std::uint16_t port):
        new_connection_callback(std::move(new_connection_callback)) {

    int sockfd = 0;
    int yes=1;        // for setsockopt() SO_REUSEADDR, below
    int rv;

    struct addrinfo hints, *ai, *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if ((rv = getaddrinfo(NULL, std::to_string(port).c_str(), &hints, &ai)) != 0) {
        fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
        throw std::system_error(errno, std::generic_category(), std::string(__FILE__) + std::string(":") + std::to_string(__LINE__));
    }

    for(p = ai; p != NULL; p = p->ai_next) {
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sockfd< 0) {
            continue;
        }

        // lose the pesky "address already in use" error message
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) < 0) {
            throw std::system_error(errno, std::generic_category(), __FILE__ + std::string(":") + std::to_string(__LINE__));
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) < 0) {
            ::close(sockfd);
            continue;
        }

        break;
    }

    // if we got here, it means we didn't get bound
    if (p == NULL) {
        fprintf(stderr, "selectserver: failed to bind\n");
        throw std::system_error(errno, std::generic_category(), std::string(__FILE__) + std::string(":") + std::to_string(__LINE__));
    }

    freeaddrinfo(ai); // all done with this

    // listen
    if (listen(sockfd, 10) == -1) {
        throw std::system_error(errno, std::generic_category(), std::string(__FILE__) + std::string(":") + std::to_string(__LINE__));
    }

    set_socket(sockfd, true);
}

static void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

static in_port_t get_in_port(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return ((struct sockaddr_in*)sa)->sin_port;
    }
    return ((struct sockaddr_in6*)sa)->sin6_port;
}

void TcpServer::on_select() {
    int newfd;
    struct sockaddr_storage remoteaddr;
    socklen_t addrlen;
    char remoteIP[INET6_ADDRSTRLEN];

    addrlen = sizeof(remoteaddr);
    newfd = accept(get_socket(), (struct sockaddr *)&remoteaddr, &addrlen);

    if (newfd == -1) {
        throw std::system_error(errno, std::generic_category(), std::string(__FILE__) + std::string(":") + std::to_string(__LINE__));
    } else {
        new_connection_callback(
                newfd,
                std::string(inet_ntop(remoteaddr.ss_family, get_in_addr((struct sockaddr *) &remoteaddr), remoteIP, INET6_ADDRSTRLEN)),
                ntohs(get_in_port((struct sockaddr *) &remoteaddr)));
    }
}

void TcpServer::on_close() noexcept {
    ::close(get_socket());
    disconnected();
}
