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
#include "protocol/devicemethodresponsemsg.h"
#include "protocol/errormsg.h"
#include "tcpclientthread.h"
#include "tcpserver.h"


int ExecutorAdapter::attach_device_event_observer(std::uint16_t dev_id, std::string event_name) {
    attach_device_event_observer_memento.push_back(std::make_tuple(dev_id, event_name));
    return executor->attach_device_event_observer(_client, dev_id, event_name);
}

int ExecutorAdapter::detach_device_event_observer(std::uint16_t dev_id, std::string event_name) {
    attach_device_event_observer_memento.remove(std::make_tuple(dev_id, event_name));
    return executor->detach_device_event_observer(_client, dev_id, event_name);
}

ExecutorAdapter::ExecutorAdapter(Executor *executor, TCPServer *tcpserver) noexcept: executor(executor), tcpserver(tcpserver) {
    _client = nullptr;
}

void ExecutorAdapter::set_client(TCPClientThread *client) {
    assert(_client == nullptr);
    _client = client;
}

GenericMsg ExecutorAdapter::execute_method(ExecuteMethodMsg execmsg) {
    assert(_client);

    if (execmsg.get_method_name() == "methods_list") {
        MethodResponseMsg res(execmsg.get_method_name());
        auto methods = res.add_array("methods");
        methods.add_value("methods_list");
        methods.add_value("h9d_stat");
        methods.add_value("devices_list");

        res.set_request_id(execmsg.get_id());
        return std::move(res);
    }
    else if (execmsg.get_method_name() == "h9d_stat") {
        std::string version;
        std::time_t uptime;

        executor->get_stat(version, uptime);

        MethodResponseMsg res(execmsg.get_method_name());
        res.add_value("version", version);
        res.add_value("uptime", uptime);
        res.add_value("connected_clients_count", tcpserver->connected_clients_count());
        res.add_value("active_devices_count", executor->active_devices_count());

        res.set_request_id(execmsg.get_id());
        return std::move(res);
    }
    else if (execmsg.get_method_name() == "devices_list") {
        MethodResponseMsg res(execmsg.get_method_name());
        auto devices = res.add_array("devices");

        for (const auto &it: executor->get_devices_list()) {
            auto desc = devices.add_dict();
            desc.add_value("id", it.id);
            desc.add_value("type", it.type);
            desc.add_value("version", std::to_string(it.version_major) + "." + std::to_string(it.version_minor));
        }

        res.set_request_id(execmsg.get_id());
        return std::move(res);
    }
    else if (execmsg.get_method_name() == "dev") {
        //
        // POC
        //
        attach_device_event_observer(32, "antenna-switch");

        int active_antenna = executor->execute_object_method(execmsg["variable_antenna"].get_value_as_int(), _client);

        MethodResponseMsg res("dev");
        res.add_value("object", execmsg["object"].get_value_as_str());
        res.add_value("id", execmsg["id"].get_value_as_int());
        res.add_value("method", execmsg["method"].get_value_as_str());
        res.add_value("response", active_antenna);

        res.set_request_id(execmsg.get_id());
        return std::move(res);
    }

    ErrorMsg err_msg(ErrorMsg::ErrorNumber::UNSUPPORTED_METHOD, "Unsupported method: " + execmsg.get_method_name());
    err_msg.set_request_id(execmsg.get_id());
    return std::move(err_msg);
}

GenericMsg ExecutorAdapter::execute_device_method(DeviceMethodResponseMsg exedevcmsg) {
    assert(_client);

    std::uint16_t dev_id = exedevcmsg.get_device_id();
    std::string method = exedevcmsg.get_method_name();

    if (!executor->is_device_exist(dev_id)) {
        ErrorMsg err_msg(ErrorMsg::ErrorNumber::INVALID_DEVICE, "Invalid device (id: " + std::to_string(dev_id) + ")");
        err_msg.set_request_id(exedevcmsg.get_id());
        return std::move(err_msg);
    }

    if (method == "methods_list") {
        DeviceMethodResponseMsg res(dev_id, method);
        auto methods = res.add_array("methods");
        methods.add_value("methods_list");
        methods.add_value("events_list");
        methods.add_value("registers_list");
        methods.add_value("subscribe");
        methods.add_value("unsubscribe");

        res.set_request_id(exedevcmsg.get_id());
        return std::move(res);
    }
    else if (method == "events_list") {

    }
    else if (method == "subscribe") {
        try {
            std::string event = exedevcmsg["event"].get_value_as_str();

            attach_device_event_observer(dev_id, event);

            DeviceMethodResponseMsg res(dev_id, method);
            res.set_request_id(exedevcmsg.get_id());
            return std::move(res);

        }
        catch (std::out_of_range &e) {
            h9_log_warn("Execute device method 'subscribe' with missing 'event' attribute (from: %s)", _client->get_client_idstring().c_str());
            ErrorMsg err_msg(ErrorMsg::ErrorNumber::INVALID_PARAMETERS, "Missing 'event' attribute");
            err_msg.set_request_id(exedevcmsg.get_id());
            return std::move(err_msg);
        }
    }
    else if (method == "unsubscribe") {
        try {
            std::string event = exedevcmsg["event"].get_value_as_str();

            detach_device_event_observer(dev_id, event);

            DeviceMethodResponseMsg res(dev_id, method);
            res.set_request_id(exedevcmsg.get_id());
            return std::move(res);
        }
        catch (std::out_of_range &e) {
            h9_log_warn("Execute device method 'subscribe' with missing 'event' attribute (from: %s)", _client->get_client_idstring().c_str());
            ErrorMsg err_msg(ErrorMsg::ErrorNumber::INVALID_PARAMETERS, "Missing 'event' attribute");
            err_msg.set_request_id(exedevcmsg.get_id());
            return std::move(err_msg);
        }
    }

    ErrorMsg err_msg(ErrorMsg::ErrorNumber::UNSUPPORTED_DEVICE_METHOD, "Unsupported device (id: " + std::to_string(dev_id) + ") method: " + method);
    err_msg.set_request_id(exedevcmsg.get_id());
    return std::move(err_msg);
}

void ExecutorAdapter::cleanup_connection() {
    assert(_client);
    h9_log_debug("Closing connection %s:%s", _client->get_remote_address().c_str(), _client->get_remote_port().c_str());
    for (auto memento: attach_device_event_observer_memento) {
        executor->detach_device_event_observer(_client, std::get<0>(memento), std::get<1>(memento));
    }


    tcpserver->cleanup_tcpclientthread(_client); //MUST BE LAST!
}