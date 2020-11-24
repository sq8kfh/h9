/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-11-21.
 *
 * Copyright (C) 2020 Kamil Palkowski. All rights reserved.
 */

#include "executoradapter.h"
#include <cassert>
#include "common/logger.h"
#include "protocol/errormsg.h"
#include "tcpclientthread.h"
#include "tcpserver.h"


int ExecutorAdapter::attach_device_event_observer(std::uint16_t dev_id, std::string event_name) {
    attach_device_event_observer_memento.push_back(std::make_tuple(dev_id, event_name));
    return executor->attach_device_event_observer(_client, dev_id, event_name);
}

ExecutorAdapter::ExecutorAdapter(Executor *executor, TCPServer *tcpserver) noexcept: executor(executor), tcpserver(tcpserver) {
    _client = nullptr;
}

void ExecutorAdapter::set_client(TCPClientThread *client) {
    assert(_client == nullptr);
    _client = client;
}

GenericMsg ExecutorAdapter::execute_method(CallMsg callmsg) {
    assert(_client);

    if (callmsg.get_method_name() == "h9d_stat") {
        std::string version;
        std::time_t uptime;

        executor->get_stat(version, uptime);

        ResponseMsg res("h9d_stat");
        res.add_value("version", version);
        res.add_value("uptime", uptime);
        res.add_value("connected_clients_count", tcpserver->connected_clients_count());
        res.add_value("active_devices_count", executor->active_devices_count());

        res.set_request_id(callmsg.get_id());
        return std::move(res);
    }
    else if (callmsg.get_method_name() == "dev") {
        //
        // POC
        //
        attach_device_event_observer(32, "antenna-switch");

        int active_antenna = executor->execute_object_method(callmsg["variable_antenna"].get_value_as_int(), _client);

        ResponseMsg res("dev");
        res.add_value("object", callmsg["object"].get_value_as_str());
        res.add_value("id", callmsg["id"].get_value_as_int());
        res.add_value("method", callmsg["method"].get_value_as_str());
        res.add_value("response", active_antenna);

        res.set_request_id(callmsg.get_id());
        return std::move(res);
    }

    ErrorMsg err_msg(ErrorMsg::ErrorNumber::UNSUPPORTED_METHOD, "Unsupported method: " + callmsg.get_method_name());
    err_msg.set_request_id(callmsg.get_id());
    return std::move(err_msg);
}

void ExecutorAdapter::cleanup_connection() {
    assert(_client);
    h9_log_debug("Closing connection %s:%s", _client->get_remote_address().c_str(), _client->get_remote_port().c_str());
    for (auto memento: attach_device_event_observer_memento) {
        executor->detach_device_event_observer(_client, std::get<0>(memento), std::get<1>(memento));
    }
    //executor->detach_device_event_observer()
    //executor->cleanup_connection(client);

    tcpserver->cleanup_tcpclientthread(_client); //MUST BE LAST!
}
