/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-05-15.
 *
 * Copyright (C) 2019-2020 Kamil Palkowski. All rights reserved.
 */

#ifndef _H9_TCPCLIENT_H_
#define _H9_TCPCLIENT_H_

#include "config.h"
#include <functional>
#include "protocol/h9msgsocket.h"
#include "socketmgr.h"
#include "servermgr.h"


class TcpClient: public SocketMgr::Socket {
public:
    using TNewMsgCallback = std::function<void(TcpClient*, GenericMsg&)>;
private:
    TNewMsgCallback recv_msg_callback;
    H9MsgSocket h9socket;

    int active_subscription;

    void recv_msg();
public:
    TcpClient(TNewMsgCallback new_msg_callback, int sockfd);
    bool is_subscriber();
    void subscriber(int active);
    void send(GenericMsg& msg);
    ~TcpClient();
    void on_select();
    void on_close() noexcept;

    std::string get_remote_address() noexcept;
    std::string get_remote_port() noexcept;
};


#endif //_H9_TCPCLIENT_H_
