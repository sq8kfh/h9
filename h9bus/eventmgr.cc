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
#include "protocol/executemethodmsg.h"
#include "protocol/methodresponsemsg.h"
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
            h9_log_info("Process SEND_FRAME msg (id: %llu) from client: %s", msg.get_id(), origin_tcp_client->get_client_idstring().c_str());
            SendFrameMsg sf_msg = std::move(msg);
            H9frame tmp = sf_msg.get_frame();
            _bus_mgr->send_frame(tmp, origin_tcp_client->get_client_idstring(), origin_tcp_client->get_socket(), sf_msg.get_id());
            break;
        }
        case GenericMsg::Type::SUBSCRIBE: {
            h9_log_info("Process SUBSCRIBE msg (id: %llu) from client: %s", msg.get_id(), origin_tcp_client->get_client_idstring().c_str());
            SubscribeMsg sc_msg = std::move(msg);
            origin_tcp_client->subscriber(1);
            break;
        }
        case GenericMsg::Type::EXECUTEMETHOD: {
            ExecuteMethodMsg call_msg = std::move(msg);
            h9_log_info("Process EXECUTEMETHOD msg (id: %llu method: %s) from client %s", call_msg.get_id(), call_msg.get_method_name().c_str(), origin_tcp_client->get_client_idstring().c_str());
            execute_method(origin_tcp_client, std::move(call_msg));
            break;
        }
        default:
            h9_log_warn("Recv unknown (id: %llu type: %d) msg from client: %s", msg.get_id(), msg.get_type(), origin_tcp_client, origin_tcp_client->get_client_idstring().c_str());
            ErrorMsg err_msg = {ErrorMsg::ErrorNumber::UNSUPPORTED_MESSAGE_TYPE, "EventMgr::flush_msg"};
            err_msg.set_request_id(msg.get_id());
            origin_tcp_client->send(err_msg);
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

void EventMgr::execute_method(TcpClient* tcp_client, ExecuteMethodMsg call_msg) {
    std::string method_name = call_msg.get_method_name();
    if (method_name == "methods_list") {
        MethodResponseMsg res(method_name);
        res.set_request_id(call_msg.get_id());

        auto methods = res.add_array("methods");
        methods.add_value("methods_list");
        methods.add_value("h9bus_stat");
        methods.add_value("events_list");
        methods.add_value("subscribe");
        methods.add_value("unsubscribe");

        tcp_client->send(res);
    }
    else if (method_name == "h9bus_stat") {
        MethodResponseMsg res = get_stat();
        res.set_request_id(call_msg.get_id());
        tcp_client->send(res);
    }
    else if (method_name == "events_list") {
        MethodResponseMsg res(method_name);
        res.set_request_id(call_msg.get_id());

        auto events = res.add_array("events");
        events.add_value("frame");

        tcp_client->send(res);
    }
    else if (method_name == "subscribe") {
        try {
            std::string event = call_msg["event"].get_value_as_str();
            if (event == "frame") {
                tcp_client->subscriber(1);

                MethodResponseMsg res("subscribe", false);
                res.set_request_id(call_msg.get_id());

                tcp_client->send(res);
            }
            else {
                h9_log_warn("Execute method 'subscribe' with unsupported event: %s (from %s)", event.c_str(), tcp_client->get_client_idstring().c_str());
                //TODO: return MethodResponseMsg with error
                //MethodResponseMsg res("subscribe", true);
                ErrorMsg err_msg(ErrorMsg::ErrorNumber::UNSUPPORTED_EVENT, "Unsupported event: " + event);
                err_msg.set_request_id(call_msg.get_id());
                tcp_client->send(err_msg);
            }
        }
        catch (std::out_of_range &e) {
            h9_log_warn("Execute method 'subscribe' with missing 'event' attribute (from: %s)", tcp_client->get_client_idstring().c_str());
            ErrorMsg err_msg(ErrorMsg::ErrorNumber::INVALID_PARAMETERS, "Missing 'event' attribute");
            err_msg.set_request_id(call_msg.get_id());
            tcp_client->send(err_msg);
        }
    }
    else if (method_name == "unsubscribe") {
        try {
            std::string event = call_msg["event"].get_value_as_str();
            if (event == "frame") {
                tcp_client->subscriber(0);

                MethodResponseMsg res("unsubscribe", false);
                res.set_request_id(call_msg.get_id());

                tcp_client->send(res);
            }
            else {
                h9_log_warn("Execute method 'unsubscribe' with unsupported event: %s (from %s)", event.c_str(), tcp_client->get_client_idstring().c_str());
                //TODO: return MethodResponseMsg with error
                //MethodResponseMsg res("unsubscribe", true);
                ErrorMsg err_msg(ErrorMsg::ErrorNumber::UNSUPPORTED_EVENT, "Unsupported event: " + event);
                err_msg.set_request_id(call_msg.get_id());
                tcp_client->send(err_msg);
            }
        }
        catch (std::out_of_range &e) {
            h9_log_warn("Execute method 'unsubscribe' with missing 'event' attribute (from: %s)", tcp_client->get_client_idstring().c_str());
            ErrorMsg err_msg(ErrorMsg::ErrorNumber::INVALID_PARAMETERS, "Missing 'event' attribute");
            err_msg.set_request_id(call_msg.get_id());
            tcp_client->send(err_msg);
        }
    }
    else {
        h9_log_warn("Execute unsupported method '%s' (from: %s)", method_name.c_str(),tcp_client->get_client_idstring().c_str());
        ErrorMsg err_msg(ErrorMsg::ErrorNumber::UNSUPPORTED_METHOD, "Unsupported method: " + method_name);
        err_msg.set_request_id(call_msg.get_id());
        tcp_client->send(err_msg);
    }
}

MethodResponseMsg EventMgr::get_stat() {
    MethodResponseMsg res = {"h9bus_stat"};
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
