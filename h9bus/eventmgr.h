#ifndef _H9_EVENTMGR_H_
#define _H9_EVENTMGR_H_


#include "common/daemonctx.h"
#include "servermgr.h"
#include "busmgr.h"

class EventMgr {
private:
    BusMgr* const _bus_mgr;
    ServerMgr* const _server_mgr;
    DaemonCtx* const _ctx;
public:
    EventMgr(DaemonCtx* ctx, BusMgr* bus_mgr, ServerMgr* server_mgr);
    void on_msg_recv(int client_socket, GenericMsg& msg);
    void on_fame_recv(const std::string& bus_id, const H9frame& frame);
};


#endif //_H9_EVENTMGR_H_
