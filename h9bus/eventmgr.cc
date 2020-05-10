/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-05-18.
 *
 * Copyright (C) 2019-2020 Kamil Palkowski. All rights reserved.
 */

#include "eventmgr.h"

#include <ctime>
#include "tcpclient.h"
#include "busmgr.h"
#include "common/logger.h"
#include "protocol/errormsg.h"
#include "protocol/framemsg.h"
#include "protocol/callmsg.h"
#include "protocol/responsemsg.h"
#include "protocol/sendframemsg.h"
#include "protocol/subscribemsg.h"


EventMgr::EventMgr(DaemonCtx* ctx, BusMgr* bus_mgr, ServerMgr* server_mgr):
        _ctx(ctx),
        _bus_mgr(bus_mgr),
        _server_mgr(server_mgr) {

    _server_mgr->set_eventmgr_handler(this);
}

void EventMgr::flush_frame_queue() {
    auto& frame_queue = _bus_mgr->get_recv_queue();
    while (!frame_queue.empty()) {
        flush_frame(std::get<0>(frame_queue.front()),std::get<1>(frame_queue.front()), std::get<2>(frame_queue.front()));
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

void EventMgr::flush_frame(const std::string& origin, const std::string& endpoint, const H9frame& frame) {
    h9_log_debug("EventMgr::flush_frame(%s, ?)", endpoint.c_str());
    FrameMsg msg(frame, origin, endpoint);
    _server_mgr->send_msg_to_subscriber(msg);
}

void EventMgr::flush_msg(int client_socket, GenericMsg& msg) {
    h9_log_debug("EventMgr::flush_msg(%d, ?)", client_socket);
    switch (msg.get_type()) {
        case GenericMsg::Type::SEND_FRAME: {
            SendFrameMsg sf_msg = std::move(msg);
            H9frame tmp = sf_msg.get_frame();
            _bus_mgr->send_frame(tmp, std::string("tcp#") + std::to_string(client_socket));
            break;
        }
        case GenericMsg::Type::SUBSCRIBE: {
            SubscribeMsg sc_msg = std::move(msg);
            _server_mgr->client_subscription(client_socket, 1);
            break;
        }
        case GenericMsg::Type::CALL: {
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

void EventMgr::process_msg(TcpClient* origin_tcp_client, GenericMsg& msg) {
    int client_socket = origin_tcp_client->get_socket();
    h9_log_debug("EventMgr::process_msg(%d, ?)", client_socket);
    switch (msg.get_type()) {
        case GenericMsg::Type::SEND_FRAME: {
            SendFrameMsg sf_msg = std::move(msg);
            H9frame tmp = sf_msg.get_frame();
            _bus_mgr->send_frame(tmp, std::string("tcp#") + std::to_string(client_socket));
            break;
        }
        case GenericMsg::Type::SUBSCRIBE: {
            SubscribeMsg sc_msg = std::move(msg);
            _server_mgr->client_subscription(client_socket, 1);
            break;
        }
        case GenericMsg::Type::CALL: {
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

void EventMgr::exec_method_call(int client_socket, CallMsg call_msg) {
    std::string mnethod_name = call_msg.get_method_name();
    if (mnethod_name == "h9bus_stat") {
        ResponseMsg res = get_stat(client_socket);
        _server_mgr->send_msg(client_socket, res);
    }
}

ResponseMsg EventMgr::get_stat(int client_socket) {
    ResponseMsg res = {"h9bus_stat"};
    res.set_value("version", H9_VERSION);
    res.set_value("uptime", std::time(nullptr) - _ctx->get_start_time());
    res.set_value("connected_clients_count", _server_mgr->connected_clients_count());
    auto devs = res.set_array("endpoint");
    for(std::string& dev_name: _bus_mgr->get_dev_list()) {
        auto dev = devs.add_array(dev_name.c_str());
        dev.add_value("name", dev_name);
        dev.add_value("send_frames_counter", _bus_mgr->get_dev_counter(dev_name, BusMgr::CounterType::SEND_FRAMES));
        dev.add_value("received_frames_counter", _bus_mgr->get_dev_counter(dev_name, BusMgr::CounterType::RECEIVED_FRAMES));
    }
    return std::move(res);
}
