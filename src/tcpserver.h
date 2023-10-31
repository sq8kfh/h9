/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-11-19.
 *
 * Copyright (C) 2020-2023 Kamil Palkowski. All rights reserved.
 */

#pragma once

#include "config.h"

#include <list>
#include <mutex>
#include <netinet/in.h>
#include <spdlog/spdlog.h>
#include <sys/socket.h>

#include "api.h"
#include "tcpclientthread.h"
#if (defined(__unix__) && defined(BSD)) || (defined(__APPLE__) && defined(__MACH__))
#include "kqueue.h"
#elif defined(__linux__)
#include "epoll.h"
#endif

class TCPServer {
  private:
    std::uint16_t server_port;
    int sockfd;

#if (defined(__unix__) && defined(BSD)) || (defined(__APPLE__) && defined(__MACH__))
    using IOEventQueue = KQueue;
#elif defined(__linux__)
    using IOEventQueue = Epoll;
#endif

    IOEventQueue event;
    std::shared_ptr<spdlog::logger> logger;

    API* api;

    std::mutex tcpclientthread_list_mtx;
    std::list<TCPClientThread*> tcpclientthread_list;

    void listen();
    void* get_in_addr(struct sockaddr* sa);
    in_port_t get_in_port(struct sockaddr* sa);

    // friend void ExecutorAdapter::cleanup_connection();
  public:
    struct ClientInfo {
        std::string idstring;
        std::string entity;
        std::time_t connection_time;
        bool authenticated;
        std::string remote_address;
        std::string remote_port;
        bool frame_subscription;
        bool dev_subscription;
    };

    explicit TCPServer(API* api) noexcept;
    TCPServer(const TCPServer& a) = delete;
    ~TCPServer();
    void set_server_port(std::uint16_t port);
    void run();
    void cleanup_tcpclientthread(TCPClientThread* client);
    int connected_clients_count();

    std::vector<ClientInfo> get_clients_list();
};
