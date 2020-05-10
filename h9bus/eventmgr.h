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
    void exec_method_call(TcpClient* tcp_client, CallMsg call_msg);
    ResponseMsg get_stat();
public:
    EventMgr(DaemonCtx* ctx, BusMgr* bus_mgr, ServerMgr* server_mgr);
    void cron();

    void process_msg(TcpClient* origin_tcp_client, GenericMsg& msg);
    void process_recv_frame(const std::string& endpoint, BusFrame* busframe);
    void process_sent_frame(const std::string& endpoint, BusFrame* busframe);
};


#endif //_H9_EVENTMGR_H_
