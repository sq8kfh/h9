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
    _bus_mgr->set_eventmgr_handler(this);
}

void EventMgr::cron() {
    //h9_log_debug("EventMgr::cron");
    _bus_mgr->cron();
    _server_mgr->cron();
}

void EventMgr::process_msg(TcpClient* origin_tcp_client, GenericMsg& msg) {
    int client_socket = origin_tcp_client->get_socket();
    h9_log_debug("EventMgr::process_msg(%p, %p)", origin_tcp_client, msg.id());
    switch (msg.get_type()) {
        case GenericMsg::Type::SEND_FRAME: {
            h9_log_info("Process SEND_FRAME msg from client: %p", origin_tcp_client);
            SendFrameMsg sf_msg = std::move(msg);
            H9frame tmp = sf_msg.get_frame();
            h9_log_debug("EventMgr::process_msg2(%p, %p)", origin_tcp_client, sf_msg.id());
            _bus_mgr->send_frame(tmp, std::string("tcp#") + std::to_string(client_socket));
            break;
        }
        case GenericMsg::Type::SUBSCRIBE: {
            h9_log_info("Process SUBSCRIBE msg from client: %p", origin_tcp_client);
            SubscribeMsg sc_msg = std::move(msg);
            _server_mgr->client_subscription(client_socket, 1);
            break;
        }
        case GenericMsg::Type::CALL: {
            h9_log_info("Process CALL msg from client %p", origin_tcp_client);
            exec_method_call(origin_tcp_client, std::move(msg));
            break;
        }
        default:
            h9_log_err("Recv unknown (type: %d) msg from client: %p", msg.get_type(), origin_tcp_client);
            ErrorMsg err_msg = {ErrorMsg::ErrorNumber::UNSUPPORTED_MESSAGE_TYPE, "EventMgr::flush_msg"};
            _server_mgr->send_msg(client_socket, err_msg);
            break;
    }
}

void EventMgr::process_recv_frame(const std::string& endpoint, BusFrame* busframe) {
    h9_log_debug("EventMgr::process_recv_frame(%s, %p)", endpoint.c_str(), busframe);
    FrameMsg msg(busframe->get_frame(), busframe->get_origin(), endpoint);
    _server_mgr->send_msg_to_subscriber(msg);
}

void EventMgr::process_sent_frame(const std::string& endpoint, BusFrame* busframe) {
    h9_log_debug("EventMgr::process_sent_frame(%s, %p)", endpoint.c_str(), busframe);
    FrameMsg msg(busframe->get_frame(), busframe->get_origin(), endpoint);
    _server_mgr->send_msg_to_subscriber(msg);
}

void EventMgr::exec_method_call(TcpClient* tcp_client, CallMsg call_msg) {
    std::string mnethod_name = call_msg.get_method_name();
    if (mnethod_name == "h9bus_stat") {
        ResponseMsg res = get_stat();
        tcp_client->send(res);
        //_server_mgr->send_msg(client_socket, res);
    }
}

ResponseMsg EventMgr::get_stat() {
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
