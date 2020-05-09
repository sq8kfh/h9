/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-05-18.
 *
 * Copyright (C) 2019-2020 Kamil Palkowski. All rights reserved.
 */

#ifndef _H9_EVENTMGR_H_
#define _H9_EVENTMGR_H_

#include "config.h"
#include "busmgr.h"
#include "servermgr.h"
#include "common/daemonctx.h"
#include "protocol/callmsg.h"
#include "protocol/responsemsg.h"

class EventMgr {
private:
    BusMgr* const _bus_mgr;
    ServerMgr* const _server_mgr;
    DaemonCtx* const _ctx;
    void exec_method_call(int client_socket, CallMsg call_msg);
    ResponseMsg get_stat(int client_socket);
public:
    EventMgr(DaemonCtx* ctx, BusMgr* bus_mgr, ServerMgr* server_mgr);
    void flush_frame_queue();
    void flush_msg_queue();
    void flush_all();
    void flush_frame(const std::string& origin, const std::string& endpoint, const H9frame& frame);
    void flush_msg(int client_socket, GenericMsg& msg);
    void cron();
};


#endif //_H9_EVENTMGR_H_
