/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-11-19.
 *
 * Copyright (C) 2020 Kamil Palkowski. All rights reserved.
 */

#ifndef H9_TCPSERVER_H
#define H9_TCPSERVER_H

#include "config.h"
#include <mutex>
#include <list>
#include <sys/socket.h>
#include <netinet/in.h>
#include "common/logger.h"
#include "dctx.h"
#include "executoradapter.h"
#include "tcpclientthread.h"
#if (defined(__APPLE__) && defined(__MACH__))
#include "kqueue.h"
#elif defined(__linux__)
#include "epoll.h"
#endif


class TCPServer {
private:
    std::uint16_t server_port;
    int sockfd;

#if (defined(__unix__) && defined(BSD)) || (defined(__APPLE__) && defined(__MACH__))
    KQueue event;
#elif defined(__linux__)
    Epoll event;
#endif

    Executor *executor;

    std::mutex tcpclientthread_list_mtx;
    std::list<TCPClientThread*> tcpclientthread_list;

    void listen();
    void *get_in_addr(struct sockaddr *sa);
    in_port_t get_in_port(struct sockaddr *sa);

    friend void ExecutorAdapter::cleanup_connection();
    void cleanup_tcpclientthread(TCPClientThread* client);
public:
    explicit TCPServer(Executor *executor) noexcept;
    TCPServer(const TCPServer &a) = delete;
    ~TCPServer();
    void load_config(DCtx *ctx);
    void run();

    int connected_clients_count();
};


#endif //H9_TCPSERVER_H
