/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-05-14.
 *
 * Copyright (C) 2019 Kamil Palkowski. All rights reserved.
 */

#ifndef _H9_TCPSERVER_H_
#define _H9_TCPSERVER_H_

#include "config.h"
#include "socketmgr.h"
#include "servermgr.h"


class TcpServer: public SocketMgr::Socket {
private:
    ServerMgr::EventCallback _event_callback;
public:
    TcpServer(ServerMgr::EventCallback event_callback, std::uint16_t port);
    void on_select();
    void close();
};


#endif //_H9_TCPSERVER_H_
