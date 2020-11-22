/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-11-19.
 *
 * Copyright (C) 2020 Kamil Palkowski. All rights reserved.
 */

#include "tcpserver.h"
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "executoradapter.h"


void TCPServer::listen() {
    int yes=1;        // for setsockopt() SO_REUSEADDR, below
    int rv;

    struct addrinfo hints, *ai, *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if ((rv = getaddrinfo(NULL, std::to_string(server_port).c_str(), &hints, &ai)) != 0) {
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
    if (::listen(sockfd, 10) == -1) {
        throw std::system_error(errno, std::generic_category(), std::string(__FILE__) + std::string(":") + std::to_string(__LINE__));
    }
}

void *TCPServer::get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

in_port_t TCPServer::get_in_port(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return ((struct sockaddr_in*)sa)->sin_port;
    }
    return ((struct sockaddr_in6*)sa)->sin6_port;
}

TCPServer::TCPServer(Executor *executor) noexcept: executor(executor) {

}

TCPServer::~TCPServer() {
    for (TCPClientThread *it : tcpclientthread_list) {
        delete it;
    }
    tcpclientthread_list.clear();
}

void TCPServer::load_config(DCtx *ctx) {
    server_port = ctx->cfg_server_port();
}

void TCPServer::run() {
    listen();

    while(true) {
        int newfd;
        struct sockaddr_storage remoteaddr;
        socklen_t addrlen;
        char remoteIP[INET6_ADDRSTRLEN];

        addrlen = sizeof(remoteaddr);
        newfd = accept(sockfd, (struct sockaddr *)&remoteaddr, &addrlen);

        if (newfd == -1) {
            throw std::system_error(errno, std::generic_category(), std::string(__FILE__) + std::string(":") + std::to_string(__LINE__));
        } else {
            h9_log_info("New connection %s:%d", std::string(inet_ntop(remoteaddr.ss_family, get_in_addr((struct sockaddr *) &remoteaddr), remoteIP, INET6_ADDRSTRLEN)).c_str(), ntohs(get_in_port((struct sockaddr *) &remoteaddr)));

            tcpclientthread_list.push_back(new TCPClientThread(newfd, ExecutorAdapter(executor)));
        }
    }
}
