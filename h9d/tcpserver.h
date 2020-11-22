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
#include <list>
#include "common/logger.h"
#include "dctx.h"
#include "executor.h"
#include "tcpclientthread.h"


class TCPServer {
private:
    std::uint16_t server_port;
    int sockfd;

    Executor *executor;

    std::list<TCPClientThread*> tcpclientthread_list;

    void listen();
    void *get_in_addr(struct sockaddr *sa);
    in_port_t get_in_port(struct sockaddr *sa);
public:
    explicit TCPServer(Executor *executor) noexcept;
    TCPServer(const TCPServer &a) = delete;
    ~TCPServer();
    void load_config(DCtx *ctx);
    void run();
};


#endif //H9_TCPSERVER_H
