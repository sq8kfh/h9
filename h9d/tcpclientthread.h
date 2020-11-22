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
#include <thread>
#include "executoradapter.h"
#include "protocol/callmsg.h"
#include "protocol/genericmsg.h"
#include "protocol/h9msgsocket.h"
#include "kqueue.h"

class TCPClientThread {
private:
    H9MsgSocket h9socket;
    ExecutorAdapter execadapter;

    KQueue event;

    std::thread client_thread_desc;
    std::atomic_bool runing;

    std::mutex async_msg_queue_mtx;
    std::queue<GenericMsg> async_msg_queue;

    void thread();
    void thread_recv_msg();
    void thread_send_async_msg();
    void send(GenericMsg msg);
public:
    TCPClientThread(int sockfd, ExecutorAdapter execadapter);
    TCPClientThread(const TCPClientThread &a) = delete;
    ~TCPClientThread();
    bool is_running();
    void send_msg(GenericMsg msg);
};


#endif //H9_TCPCLIENTTHREAD_H
