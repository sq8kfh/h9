#ifndef _H9_SERVERMGR_H_
#define _H9_SERVERMGR_H_


#include "socketmgr.h"
#include "common/ctx.h"
#include "tcpserver.h"

class ServerMgr {
private:
    SocketMgr * const _socket_mgr;
    TcpServer *tcp_server;
public:
    ServerMgr(SocketMgr *socket_mgr);
    void load_config(Ctx *ctx);
    ~ServerMgr();
};


#endif //_H9_SERVERMGR_H_
