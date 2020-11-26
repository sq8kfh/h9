/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-11-23.
 *
 * Copyright (C) 2020 Kamil Palkowski. All rights reserved.
 */

#include "device.h"
#include "common/logger.h"
#include "protocol/deviceevent.h"
#include "tcpclientthread.h"


void Device::update_device_state(H9frame frame) {
    if (frame.type == H9frame::Type::REG_EXTERNALLY_CHANGED && frame.source_id == 32 && frame.data[0] == 10) {
        DeviceEvent event_msg(node_id, "register_change");

        event_msg.add_value("register_id", frame.data[0]);
        event_msg.add_value("value_len", frame.dlc-1);
        uint64_t tmp_value = 0;
        for (int i=1; i < frame.dlc; ++i) {
            tmp_value <<= 8;
            tmp_value |= frame.data[i];
        }
        event_msg.add_value("value", tmp_value);

        notify_event_observer("register_change", std::move(event_msg));
    }
}

void Device::attach_event_observer(TCPClientThread *observer, std::string event_name) {
    event_name_mtx.lock();
    event_observers[event_name].insert(observer);
    event_name_mtx.unlock();
    h9_log_info("Attach observer %s to '%s' event on device %hu", observer->get_client_idstring().c_str(), event_name.c_str(), get_node_id());
}

void Device::detach_event_observer(TCPClientThread *observer, std::string event_name) {
    event_name_mtx.lock();
    event_observers[event_name].erase(observer);
    event_name_mtx.unlock();
    h9_log_info("Detach observer %s from '%s' event on device %hu", observer->get_client_idstring().c_str(), event_name.c_str(), get_node_id());
}

void Device::notify_event_observer(std::string event_name, GenericMsg msg) {
    event_name_mtx.lock();
    for (auto observer : event_observers[event_name]) {
        h9_log_debug("Notify observer %s, event '%s' on device %hu", observer->get_client_idstring().c_str(), event_name.c_str(), node_id);
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
