/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-05-15.
 *
 * Copyright (C) 2019 Kamil Palkowski. All rights reserved.
 */

#ifndef _H9_TCPCLIENT_H_
#define _H9_TCPCLIENT_H_

#include "config.h"
#include "protocol/h9socket.h"
#include "socketmgr.h"
#include "servermgr.h"


class TcpClient: public SocketMgr::Socket {
private:
    ServerMgr::EventCallback _event_callback;
    H9Socket h9socket;

    int active_subscription;

    void recv();
    void recv_msg(const std::string& msg_str);
public:
    TcpClient(ServerMgr::EventCallback event_callback, int sockfd);
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
