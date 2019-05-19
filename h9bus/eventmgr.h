/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-05-18.
 *
 * Copyright (C) 2019 Kamil Palkowski. All rights reserved.
 */

#ifndef _H9_EVENTMGR_H_
#define _H9_EVENTMGR_H_

#include "config.h"
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
    void flush_frame_queue();
    void flush_msg_queue();
    void flush_all();
    void flush_frame(const std::string& bus_id, const H9frame& frame);
    void flush_msg(int client_socket, GenericMsg& msg);
};


#endif //_H9_EVENTMGR_H_
