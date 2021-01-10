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


DeviceMethodResponseMsg ExecutorAdapter::attach_device_event_observer(std::uint16_t dev_id, std::string event_name) {
    attach_device_event_observer_memento.push_back(std::make_tuple(dev_id, event_name));
    return executor->attach_device_event_observer(_client, dev_id, event_name);
}

DeviceMethodResponseMsg ExecutorAdapter::detach_device_event_observer(std::uint16_t dev_id, std::string event_name) {
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

    std::string method = execmsg.get_method_name();

    try {
        if (method == "methods_list") {
            MethodResponseMsg res(execmsg.get_method_name());
            auto methods = res.add_array("methods");
            methods.add_value("methods_list");
            methods.add_value("h9d_stat");
            methods.add_value("devices_list");
            methods.add_value("discover");

            res.set_request_id(execmsg.get_id());
            return std::move(res);
        } else if (method == "h9d_stat") {
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
        } else if (method == "devices_list") {
            MethodResponseMsg res(execmsg.get_method_name());
            auto devices = res.add_array("devices");

            for (const auto &it: executor->get_devices_list()) {
                auto desc = devices.add_dict();
                desc.add_value("id", it.id);
                desc.add_value("type", it.type);
                desc.add_value("version", std::to_string(it.version_major) + "." + std::to_string(it.version_minor));
                desc.add_value("name", it.name);
            }

            res.set_request_id(execmsg.get_id());
            return std::move(res);
        } else if (method == "discover") {
            auto res = executor->discover(_client);
            res.set_request_id(execmsg.get_id());
            return std::move(res);
        }
    }
    catch (std::out_of_range &e) {
        h9_log_warn("Execute method '%s' with missing '%s' parameters (from: %s)", method.c_str(), e.what(), _client->get_client_idstring().c_str());
        ErrorMsg err_msg(ErrorMsg::ErrorNumber::INVALID_PARAMETERS, "Missing 'event' attribute");
        err_msg.set_request_id(execmsg.get_id());
        return std::move(err_msg);
    }
    catch (std::invalid_argument &e) {
        h9_log_warn("Execute method '%s' with invalid '%s' parameters (from: %s)", method.c_str(), e.what(), _client->get_client_idstring().c_str());
        ErrorMsg err_msg(ErrorMsg::ErrorNumber::INVALID_PARAMETERS, "Invalid parameters");
        err_msg.set_request_id(execmsg.get_id());
        return std::move(err_msg);
    }

    h9_log_warn("Unknown method '%s' from: %s", execmsg.get_method_name().c_str(), _client->get_client_idstring().c_str());
    ErrorMsg err_msg(ErrorMsg::ErrorNumber::UNSUPPORTED_METHOD, "Unsupported method: " + execmsg.get_method_name());
    err_msg.set_request_id(execmsg.get_id());
    return std::move(err_msg);
}

GenericMsg ExecutorAdapter::execute_device_method(ExecuteDeviceMethodMsg exedevcmsg) {
    assert(_client);

    std::uint16_t dev_id = exedevcmsg.get_device_id();
    std::string method = exedevcmsg.get_method_name();

    if (!executor->is_device_exist(dev_id)) {
        ErrorMsg err_msg(ErrorMsg::ErrorNumber::INVALID_DEVICE, "Invalid device (id: " + std::to_string(dev_id) + ")");
        err_msg.set_request_id(exedevcmsg.get_id());
        return std::move(err_msg);
    }

    try {
        if (method == "methods_list") {
            DeviceMethodResponseMsg res(dev_id, method);
            auto methods = res.add_array("methods");
            methods.add_value("methods_list");
            methods.add_value("events_list");
            methods.add_value("registers_list");
            methods.add_value("subscribe");
            methods.add_value("unsubscribe");
            methods.add_value("reset");

            for (const auto &it: executor->get_device_methods_list(dev_id)) {
                methods.add_value(it);
            }

            res.set_request_id(exedevcmsg.get_id());
            return std::move(res);
        } else if (method == "events_list") {
            DeviceMethodResponseMsg res(dev_id, method);
            auto events = res.add_array("events");

            for (const auto &it: executor->get_device_events_list(dev_id)) {
                events.add_value(it);
            }

            res.set_request_id(exedevcmsg.get_id());
            return std::move(res);
        } else if (method == "registers_list") {
            DeviceMethodResponseMsg res(dev_id, method);
            auto registers = res.add_array("registers");

            for (const auto &it: executor->get_device_registers_list(dev_id)) {
                auto desc = registers.add_dict();
                desc.add_value("register", it.number);
                desc.add_value("name", it.name);
                desc.add_value("type", it.type);
                desc.add_value("size", it.size);
                desc.add_value("readable", it.readable);
                desc.add_value("writable", it.writable);
                desc.add_value("description", it.description);
                if (!it.bits_names.empty()) {
                    auto bits_list = desc.add_array("bits_names");
                    for (const auto &name: it.bits_names) {
                        bits_list.add_value(name);
                    }
                }
            }

            res.set_request_id(exedevcmsg.get_id());
            return std::move(res);
        } else if (method == "subscribe") {
            std::string event = exedevcmsg["event"].get_value_as_str();

            auto res = attach_device_event_observer(dev_id, event);

            res.set_request_id(exedevcmsg.get_id());
            return std::move(res);
        } else if (method == "unsubscribe") {
            std::string event = exedevcmsg["event"].get_value_as_str();

            auto res = detach_device_event_observer(dev_id, event);

            res.set_request_id(exedevcmsg.get_id());
            return std::move(res);
        } else if (executor->has_device_specific_method(dev_id, method)) {
            auto res = executor->execute_device_method(_client, dev_id, method, exedevcmsg);
            res.set_request_id(exedevcmsg.get_id());
            return std::move(res);
        }
    }
    catch (std::out_of_range &e) {
        h9_log_warn("Execute device method '%s' with missing '%s' parameters (from: %s)", method.c_str(), e.what(),
                    _client->get_client_idstring().c_str());
        ErrorMsg err_msg(ErrorMsg::ErrorNumber::INVALID_PARAMETERS, "Missing parameters");
        err_msg.set_request_id(exedevcmsg.get_id());
        return std::move(err_msg);
    }
    catch (std::invalid_argument &e) {
        h9_log_warn("Execute device method '%s' with invalid '%s' parameters (from: %s)", method.c_str(), e.what(),
                    _client->get_client_idstring().c_str());
        ErrorMsg err_msg(ErrorMsg::ErrorNumber::INVALID_PARAMETERS, "Invalid parameters");
        err_msg.set_request_id(exedevcmsg.get_id());
        return std::move(err_msg);
    }

    h9_log_warn("Unknown device (%hu) method '%s' from: %s", dev_id, method.c_str(), _client->get_client_idstring().c_str());
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
