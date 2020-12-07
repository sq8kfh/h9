/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-11-19.
 *
 * Copyright (C) 2020 Kamil Palkowski. All rights reserved.
 */

#ifndef H9_TCPCLIENTTHREAD_H
#define H9_TCPCLIENTTHREAD_H

#include "config.h"
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include "executoradapter.h"
#include "protocol/executemethodmsg.h"
#include "protocol/genericmsg.h"
#include "protocol/h9msgsocket.h"
#if (defined(__unix__) && defined(BSD)) || defined(__APPLE__) && defined(__MACH__)
#include "kqueue.h"
#elif defined(__linux__)
#include "epoll.h"
#endif


class TCPClientThread {
private:
    H9MsgSocket h9socket;
    ExecutorAdapter execadapter;

#if (defined(__unix__) && defined(BSD)) || (defined(__APPLE__) && defined(__MACH__))
    KQueue event;
#elif defined(__linux__)
    Epoll event;
#endif

    std::thread client_thread_desc;
    std::atomic_bool running;
    std::atomic_bool thread_running;

    std::string entity;

    std::mutex async_msg_queue_mtx;
    std::queue<GenericMsg> async_msg_queue;

    void thread();
    void thread_recv_msg();
    void thread_send_async_msg();
    void send(GenericMsg msg);
    void close_connection();
public:
    TCPClientThread(int sockfd, ExecutorAdapter execadapter);
    TCPClientThread(const TCPClientThread &a) = delete;
    ~TCPClientThread();

    std::string get_remote_address() noexcept;
    std::string get_remote_port() noexcept;
    std::string get_entity() noexcept;
    std::string get_client_idstring() noexcept;

    bool is_running();
    void send_msg(GenericMsg msg);
};


#endif //H9_TCPCLIENTTHREAD_H
