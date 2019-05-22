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
#include "socketmgr.h"
#include "servermgr.h"


class TcpClient: public SocketMgr::Socket {
private:
    ServerMgr::EventCallback _event_callback;
    size_t data_to_read;
    std::string recv_data_buf;

    int active_subscription;

    void recv();
    void recv_header();
    void recv_data();
    void recv_msg(const std::string& msg_str);
public:
    const std::string remote_address;
    const std::uint16_t remote_port;
    TcpClient(ServerMgr::EventCallback event_callback, int sockfd, std::string remote_address, std::uint16_t remote_port);
    bool is_subscriber();
    void subscriber(int active);
    void send(GenericMsg& msg);
    ~TcpClient();
    void on_select();
    void on_close() noexcept;
};


#endif //_H9_TCPCLIENT_H_
