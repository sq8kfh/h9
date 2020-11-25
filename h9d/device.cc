/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-11-23.
 *
 * Copyright (C) 2020 Kamil Palkowski. All rights reserved.
 */

#include "device.h"
#include "common/logger.h"
#include "tcpclientthread.h"


void Device::update_device_state(H9frame frame) {

    ////
    //// POC
    ////
    if (frame.type == H9frame::Type::REG_EXTERNALLY_CHANGED && frame.source_id == 32 && frame.data[0] == 10) {
        MethodResponseMsg res("dev");
        res.add_value("object", "antenna-switch");
        res.add_value("id", 8);
        res.add_value("method", "switch");
        res.add_value("response", frame.data[1]);
        notify_event_observer("antenna-switch", std::move(res));
    }
}

void Device::attach_event_observer(TCPClientThread *observer, std::string event_name) {
    event_name_mtx.lock();
    event_observers[event_name].insert(observer);
    event_name_mtx.unlock();
    h9_log_info("Attach observer %s:%s to '%s' event on device %hu", observer->get_remote_address().c_str(), observer->get_remote_port().c_str(), event_name.c_str(), get_node_id());
}

void Device::detach_event_observer(TCPClientThread *observer, std::string event_name) {
    event_name_mtx.lock();
    event_observers[event_name].erase(observer);
    event_name_mtx.unlock();
    h9_log_info("Detach observer %s:%s from '%s' event on device %hu", observer->get_remote_address().c_str(), observer->get_remote_port().c_str(), event_name.c_str(), get_node_id());
}

void Device::notify_event_observer(std::string event_name, GenericMsg msg) {
    event_name_mtx.lock();
    for (auto observer : event_observers[event_name]) {
        (*observer).send_msg(msg);
    }
    event_name_mtx.unlock();
}

Device::Device(Bus* bus, std::uint16_t node_id, std::uint16_t node_type, std::uint16_t node_version) noexcept: Node(bus, node_id), node_type(node_type), node_version(node_version) {
    h9_log_notice("Create device descriptor: id: %hu type: %hu version: %hhu.%hhu", node_id, node_type, node_version >> 8, node_version);
}

std::uint16_t Device::get_node_type() noexcept {
    return node_type;
}

std::uint16_t Device::get_node_version() noexcept {
    return node_version;
}

std::uint8_t Device::get_node_version_major() noexcept {
    return node_version >> 8;
}

std::uint8_t Device::get_node_version_minor() noexcept {
    return node_version;
}
