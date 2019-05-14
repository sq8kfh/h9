#ifndef _H9_TCPSERVER_H_
#define _H9_TCPSERVER_H_

#include "socketmgr.h"

class TcpServer: public SocketMgr::Socket {
public:
    TcpServer(std::uint16_t port);
    void on_select();
};


#endif //_H9_TCPSERVER_H_
