/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-05-18.
 *
 * Copyright (C) 2019-2020 Kamil Palkowski. All rights reserved.
 */

#include "eventmgr.h"

#include <ctime>
#include <sstream>
#include "tcpclient.h"
#include "busmgr.h"
#include "common/logger.h"
#include "protocol/errormsg.h"
#include "protocol/framemsg.h"
#include "protocol/callmsg.h"
#include "protocol/responsemsg.h"
#include "protocol/sendframemsg.h"
#include "protocol/subscribemsg.h"

#include "socketmgr.h"
EventMgr::EventMgr(DaemonCtx* ctx, BusMgr* bus_mgr, ServerMgr* server_mgr):
        _ctx(ctx),
        _bus_mgr(bus_mgr),
        _server_mgr(server_mgr) {

    _server_mgr->set_eventmgr_handler(this);
    _bus_mgr->set_eventmgr_handler(this);
}

void EventMgr::cron() {
    _bus_mgr->cron();
    _server_mgr->cron();
}

void EventMgr::process_msg(TcpClient* origin_tcp_client, GenericMsg& msg) {
    switch (msg.get_type()) {
        case GenericMsg::Type::SEND_FRAME: {
            h9_log_info("Process SEND_FRAME msg (id: %llu) from client: %s:%s", msg.get_id(), origin_tcp_client->get_remote_address().c_str(), origin_tcp_client->get_remote_port().c_str());
            SendFrameMsg sf_msg = std::move(msg);
            H9frame tmp = sf_msg.get_frame();
            std::ostringstream ss;
            ss << "h9?@" << origin_tcp_client->get_remote_address() << ":" << origin_tcp_client->get_remote_port();
            _bus_mgr->send_frame(tmp, ss.str(), origin_tcp_client->get_socket(), sf_msg.get_id());
            break;
        }
        case GenericMsg::Type::SUBSCRIBE: {
            h9_log_info("Process SUBSCRIBE msg (id: %llu) from client: %s:%s", msg.get_id(), origin_tcp_client->get_remote_address().c_str(), origin_tcp_client->get_remote_port().c_str());
            SubscribeMsg sc_msg = std::move(msg);
            origin_tcp_client->subscriber(1);
            break;
        }
        case GenericMsg::Type::CALL: {
            CallMsg call_msg = std::move(msg);
            h9_log_info("Process CALL msg (id: %llu method: %s) from client %s:%s", call_msg.get_id(), call_msg.get_method_name().c_str(), origin_tcp_client->get_remote_address().c_str(), origin_tcp_client->get_remote_port().c_str());
            exec_method_call(origin_tcp_client, std::move(call_msg));
            break;
        }
        default:
            h9_log_warn("Recv unknown (id: %llu type: %d) msg from client: %s:%s", msg.get_id(), msg.get_type(), origin_tcp_client, origin_tcp_client->get_remote_address().c_str(), origin_tcp_client->get_remote_port().c_str());
            ErrorMsg err_msg = {ErrorMsg::ErrorNumber::UNSUPPORTED_MESSAGE_TYPE, "EventMgr::flush_msg"};
            err_msg.set_request_id(msg.get_id());
            origin_tcp_client->send(err_msg);
            //_server_mgr->send_msg(client_socket, err_msg);
            break;
    }
}

void EventMgr::process_recv_frame(const std::string& endpoint, BusFrame* busframe) {
    h9_log_debug("EventMgr::process_recv_frame(%s, %p)", endpoint.c_str(), busframe);
    FrameMsg msg(busframe->get_frame(), busframe->get_origin(), endpoint);
    _server_mgr->send_msg_to_subscriber(msg, 0, 0);
}

void EventMgr::process_sent_frame(const std::string& endpoint, BusFrame* busframe) {
    h9_log_debug("EventMgr::process_sent_frame(%s, %p)", endpoint.c_str(), busframe);
    FrameMsg msg(busframe->get_frame(), busframe->get_origin(), endpoint);
    _server_mgr->send_msg_to_subscriber(msg, busframe->get_orgin_client_id(), busframe->get_orgin_msg_id());
}

void EventMgr::exec_method_call(TcpClient* tcp_client, CallMsg call_msg) {
    std::string mnethod_name = call_msg.get_method_name();
    if (mnethod_name == "h9bus_stat") {
        ResponseMsg res = get_stat();
        res.set_request_id(call_msg.get_id());
        tcp_client->send(res);
    }
}

ResponseMsg EventMgr::get_stat() {
    ResponseMsg res = {"h9bus_stat"};
    res.add_value("version", H9_VERSION);
    res.add_value("uptime", std::time(nullptr) - _ctx->get_start_time());
    res.add_value("connected_clients_count", _server_mgr->connected_clients_count());
    auto devs = res.add_array("endpoint");
    for(std::string& dev_name: _bus_mgr->get_dev_list()) {
        auto dev = devs.add_dict();
        dev.add_value("name", dev_name);
        dev.add_value("send_frames_counter", _bus_mgr->get_dev_counter(dev_name, BusMgr::CounterType::SEND_FRAMES));
        dev.add_value("received_frames_counter", _bus_mgr->get_dev_counter(dev_name, BusMgr::CounterType::RECEIVED_FRAMES));
    }
    return std::move(res);
}
