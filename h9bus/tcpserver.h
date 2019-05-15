#ifndef _H9_TCPSERVER_H_
#define _H9_TCPSERVER_H_

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
