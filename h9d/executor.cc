/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-11-21.
 *
 * Copyright (C) 2020 Kamil Palkowski. All rights reserved.
 */

#include "executor.h"
#include <algorithm>
#include <cassert>
#include "node.h"
#include "tcpclientthread.h"


Executor::Executor(DCtx *ctx, Bus *bus, DevMgr *devmgr) noexcept: ctx(ctx), bus(bus), devmgr(devmgr) {

}

void Executor::get_stat(std::string &version, std::time_t &uptime) {
    version = std::string(H9_VERSION);
    uptime = std::time(nullptr) - ctx->get_start_time();
}

int Executor::active_devices_count() {
    return devmgr->active_devices_count();
}

bool Executor::is_device_exist(std::uint16_t dev_id) {
    if (dev_id < H9frame::BROADCAST_ID)
        return devmgr->is_device_exist(dev_id);
    else
        return false;
}

std::vector<DevMgr::DeviceDsc> Executor::get_devices_list() {
    return std::move(devmgr->get_devices_list());
}

MethodResponseMsg Executor::discover(TCPClientThread *client) {
    if (devmgr->discover() < 0) {
        h9_log_warn("Execute 'discover' (from %s)- generic error", client->get_client_idstring().c_str());
        MethodResponseMsg res("subscribe", 101, "Generic error");
        return std::move(res);
    }
    return MethodResponseMsg("discover");
}

DeviceMethodResponseMsg Executor::attach_device_event_observer(TCPClientThread *client, std::uint16_t dev_id, std::string event_name) {
    if (devmgr->attach_event_observer(client, event_name, dev_id) < 0) {
        h9_log_warn("Execute device method 'subscribe' with event: %s (from %s) - generic error", event_name.c_str(), client->get_client_idstring().c_str());
        MethodResponseMsg res("subscribe", 101, "Generic error");
        return std::move(res);
    }
    return MethodResponseMsg("subscribe");
}

DeviceMethodResponseMsg Executor::detach_device_event_observer(TCPClientThread *client, std::uint16_t dev_id, std::string event_name) {
    if (devmgr->detach_event_observer(client, event_name, dev_id) < 0) {
        h9_log_warn("Execute device method 'unsubscribe' with event: %s (from %s) - generic error", event_name.c_str(), client->get_client_idstring().c_str());
        MethodResponseMsg res("unsubscribe", 101, "Generic error");
        return std::move(res);
    }
    return MethodResponseMsg("unsubscribe");
}

std::vector<std::string> Executor::get_device_events_list(std::uint16_t dev_id) {
    return devmgr->get_events_list(dev_id);
}

std::vector<Device::RegisterDsc> Executor::get_device_registers_list(std::uint16_t dev_id) {
    return devmgr->get_registers_list(dev_id);
}

std::vector<std::string> Executor::get_device_methods_list(std::uint16_t dev_id) {
    std::vector<std::string> ret;
    ret.push_back("set_register");
    ret.push_back("get_register");
    ret.push_back("reset");
    ret.push_back("info");

    auto tmp = devmgr->get_device_specific_methods(dev_id);
    ret.insert(ret.end(), tmp.begin(), tmp.end());

    return ret;
}

bool Executor::has_device_specific_method(std::uint16_t dev_id, std::string method_name) {
    auto method_list = get_device_methods_list(dev_id);
    return std::find(method_list.begin(), method_list.end(), method_name) != method_list.end();
}

DeviceMethodResponseMsg Executor::execute_device_method(const TCPClientThread *client, std::uint16_t device_id, std::string method_name, ExecuteDeviceMethodMsg &exedevcmsg) {
    h9_log_debug("Execute device (%hu) method '%s' for %s", device_id, method_name.c_str(), client->get_client_idstring().c_str());
    if (method_name == "set_register") {
        std::int64_t setted; //setted value
        if (devmgr->set_device_register(device_id, exedevcmsg["register"].get_value_as_int(), exedevcmsg["value"].get_value_as_int(), &setted) < 0) {
            return DeviceMethodResponseMsg(device_id, method_name, 102, "Timeout");
        }
        return DeviceMethodResponseMsg(device_id, method_name).add_value("register", exedevcmsg["register"].get_value_as_int()).add_value("value", setted);
    }
    else if (method_name == "get_register") {
        std::int64_t buf;
        if (devmgr->get_device_register(device_id, exedevcmsg["register"].get_value_as_int(), buf) < 0) {
            return DeviceMethodResponseMsg(device_id, method_name,  102, "Timeout");
        }
        return DeviceMethodResponseMsg(device_id, method_name).add_value("register", exedevcmsg["register"].get_value_as_int()).add_value("value", buf);
    }
    else if (method_name == "reset") {
        if (devmgr->device_reset(device_id) < 0) {
            return DeviceMethodResponseMsg(device_id, method_name,  102, "Timeout");
        }
        return DeviceMethodResponseMsg(device_id, method_name);
    }
    else if (method_name == "info") {
        DevMgr::DeviceInfo dev_info;
        if (devmgr->get_device_info(device_id, dev_info) < 0) {
            return DeviceMethodResponseMsg(device_id, method_name,  102, "Timeout");
        }
        auto res = DeviceMethodResponseMsg(device_id, method_name);
        res.add_value("id", dev_info.id);
        res.add_value("type", dev_info.type);
        res.add_value("version", std::to_string(dev_info.version_major) + "." + std::to_string(dev_info.version_minor));
        res.add_value("name", dev_info.name);

        char buf[sizeof "2016-06-30T17:26:29Z"];
        strftime(buf, sizeof buf, "%FT%TZ", gmtime(&dev_info.created_time));
        res.add_value("created_time", std::string(buf));
        strftime(buf, sizeof buf, "%FT%TZ", gmtime(&dev_info.last_seen_time));
        res.add_value("last_seen_time", std::string(buf));

        res.add_value("description", dev_info.description);
        return res;
    }
    else {
        assert(false);
    }
}
