/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-05-18.
 *
 * Copyright (C) 2019 Kamil Palkowski. All rights reserved.
 */

#include "eventmgr.h"

#include "tcpclient.h"
#include "busmgr.h"
#include "common/logger.h"
#include "protocol/errormsg.h"
#include "protocol/framereceivedmsg.h"
#include "protocol/methodcallmsg.h"
#include "protocol/methodresponsemsg.h"
#include "protocol/sendframemsg.h"
#include "protocol/subscribemsg.h"


EventMgr::EventMgr(DaemonCtx* ctx, BusMgr* bus_mgr, ServerMgr* server_mgr):
        _ctx(ctx),
        _bus_mgr(bus_mgr),
        _server_mgr(server_mgr) {
}

void EventMgr::flush_frame_queue() {
    auto& frame_queue = _bus_mgr->get_recv_queue();
    while (!frame_queue.empty()) {
        flush_frame(std::get<0>(frame_queue.front()),std::get<1>(frame_queue.front()),  std::get<2>(frame_queue.front()));
        frame_queue.pop();
    }
}

void EventMgr::flush_msg_queue() {
    auto& msg_queue = _server_mgr->get_recv_queue();
    while (!msg_queue.empty()) {
        flush_msg(msg_queue.front().first, msg_queue.front().second);
        msg_queue.pop();
    }
}

void EventMgr::flush_all() {
    flush_frame_queue();
    flush_msg_queue();
}

void EventMgr::flush_frame(bool recv_not_send, const std::string& bus_id, const H9frame& frame) {
    h9_log_debug("EventMgr::flush_frame(%s, ?)", bus_id.c_str());
    FrameReceivedMsg msg(frame);
    _server_mgr->send_msg_to_subscriber(msg);
}

void EventMgr::flush_msg(int client_socket, GenericMsg& msg) {
    h9_log_debug("EventMgr::flush_msg(%d, ?)", client_socket);
    switch (msg.get_type()) {
        case GenericMsg::Type::SEND_FRAME: {
            SendFrameMsg sf_msg = std::move(msg);
            H9frame tmp = sf_msg.get_frame();
            _bus_mgr->send_frame(tmp);
            break;
        }
        case GenericMsg::Type::SUBSCRIBE: {
            SubscribeMsg sc_msg = std::move(msg);
            _server_mgr->client_subscription(client_socket, 1);
            break;
        }
        case GenericMsg::Type::METHODCALL: {
            exec_method_call(client_socket, std::move(msg));
            break;
        }
        default:
            h9_log_err("recv unknown msg: %d", msg.get_type());
            ErrorMsg err_msg = {ErrorMsg::ErrorNumber::UNSUPPORTED_MESSAGE_TYPE, "EventMgr::flush_msg"};
            _server_mgr->send_msg(client_socket, err_msg);
            break;
    }
}

void EventMgr::cron() {
    //h9_log_debug("EventMgr::cron");
    _bus_mgr->cron();
    _server_mgr->cron();
}

void EventMgr::exec_method_call(int client_socket, MethodCallMsg call_msg) {
    std::string mnethod_name = call_msg.get_method_name();
    if (mnethod_name == "get_h9bus_stat") {
        MethodResponseMsg res = {mnethod_name};
        _server_mgr->send_msg(client_socket, res);
    }
}
