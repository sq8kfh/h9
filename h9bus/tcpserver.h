/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-05-14.
 *
 * Copyright (C) 2019-2020 Kamil Palkowski. All rights reserved.
 */

#ifndef _H9_TCPSERVER_H_
#define _H9_TCPSERVER_H_

#include "config.h"
#include <functional>
#include <string>
#include "socketmgr.h"


class TcpServer: public SocketMgr::Socket {
public:
    using TNewConnectionCallback = std::function<void(int client_socket, const std::string& remote_address, std::uint16_t remote_port)>;
private:
    TNewConnectionCallback new_connection_callback;
public:
    TcpServer(TNewConnectionCallback new_connection_callback, std::uint16_t port);
    void on_select();
    void on_close() noexcept;
};


#endif //_H9_TCPSERVER_H_
