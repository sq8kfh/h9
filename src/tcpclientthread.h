/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-11-19.
 *
 * Copyright (C) 2020-2023 Kamil Palkowski. All rights reserved.
 */

#pragma once

#include "config.h"

#include <jsonrpcpp/jsonrpcpp.hpp>
#include <mutex>
#include <queue>
#include <spdlog/spdlog.h>
#include <string>
#include <thread>

#include "api.h"
#include "client_frame_obs.h"
#include "h9msgsocket.h"
#if (defined(__unix__) && defined(BSD)) || defined(__APPLE__) && defined(__MACH__)
#include "kqueue.h"
#elif defined(__linux__)
#include "epoll.h"
#endif

class API;
class ClientFrameObs;
class DevStatusObserver;
class TCPServer;

class TCPClientThread {
  private:
    std::shared_ptr<spdlog::logger> logger;
    H9MsgSocket h9socket;

    API* api;
    TCPServer* server;

#if (defined(__unix__) && defined(BSD)) || (defined(__APPLE__) && defined(__MACH__))
    using IOEventQueue = KQueue;
#else
    using IOEventQueue = Epoll;
#endif

    IOEventQueue event;

    std::thread client_thread_desc;
    std::atomic_bool running;
    std::atomic_bool thread_running;

    std::string _entity;
    bool _authenticated;

    ClientFrameObs* _frame_observer;
    DevStatusObserver* _dev_status_observer;

    std::mutex async_msg_queue_mtx;
    std::queue<jsonrpcpp::entity_ptr> async_msg_queue;

    void thread();
    void thread_recv_msg();
    void thread_send_async_msg();
    void send(jsonrpcpp::entity_ptr msg);
    void close_connection();

  public:
    const std::time_t connection_time;

    TCPClientThread(int sockfd, API* api, TCPServer* server);
    TCPClientThread(const TCPClientThread& a) = delete;
    ~TCPClientThread();

    void set_frame_observer(ClientFrameObs* frame_observer);
    bool is_frame_observer_set() const;
    void set_dev_status_observer(DevStatusObserver* dev_status_observer);
    bool is_dev_status_observer_set() const;

    std::string get_remote_address() const noexcept;
    std::string get_remote_port() const noexcept;
    std::string entity() const noexcept;
    void entity(const std::string& entity) noexcept;

    bool authenticated() const noexcept;
    void authenticate(const std::string& entity) noexcept;

    std::string get_client_idstring() const noexcept;

    bool is_running();
    void send_msg(jsonrpcpp::entity_ptr msg);
};
