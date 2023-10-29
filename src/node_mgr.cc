/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-11-19.
 *
 * Copyright (C) 2020-2023 Kamil Palkowski. All rights reserved.
 */

#include "node_mgr.h"

#include <cassert>
#include <utility>

#include "bus.h"
#include "dev_node_exception.h"
#include "h9d_configurator.h"
#include "tcpclientthread.h"

void NodeMgr::on_frame_recv(const ExtH9Frame& frame) noexcept {
    notify_frame_observer(frame);

    frame_queue_mtx.lock();
    frame_queue.push(frame);
    frame_queue_mtx.unlock();
    frame_queue_cv.notify_one();
}

void NodeMgr::nodes_update_thread() {
    while (nodes_update_thread_run) {
        std::unique_lock<std::mutex> lk(frame_queue_mtx);
        frame_queue_cv.wait(lk, [this]() { return !frame_queue.empty(); });
        ExtH9Frame frame = frame_queue.front();
        frame_queue.pop();
        int remained_frame = frame_queue.size();
        lk.unlock();

        SPDLOG_LOGGER_TRACE(logger, "Devices update thread: pop frame (remained {}).", remained_frame);

        if (frame.type() == H9frame::Type::NODE_INFO) {
            uint16_t version_major = frame.data()[2] << 8 | frame.data()[3];
            uint16_t version_minor = frame.data()[4] << 8 | frame.data()[5];
            uint16_t version_patch = frame.data()[6] << 8 | frame.data()[7];
            uint64_t version = version_major;
            version = version << 16 | version_minor;
            version = version << 16 | version_patch;
            SPDLOG_LOGGER_INFO(logger, "Dev discovered id: {}, type: {}, version: {}.{}.{}.", frame.source_id(), frame.data()[0] << 8 | frame.data()[1], version_major, version_minor, version_patch);
            add_node(frame.source_id(), frame.data()[0] << 8 | frame.data()[1], version);
        }
        else if (frame.type() == H9frame::Type::NODE_TURNED_ON) {
            uint16_t version_major = frame.data()[2] << 8 | frame.data()[3];
            uint16_t version_minor = frame.data()[4] << 8 | frame.data()[5];
            uint16_t version_patch = frame.data()[6] << 8 | frame.data()[7];
            uint64_t version = version_major;
            version = version << 16 | version_minor;
            version = version << 16 | version_patch;
            SPDLOG_LOGGER_INFO(logger, "Dev turned on id: {}, type: {}, version: {}.{}.{}.", frame.source_id(), frame.data()[0] << 8 | frame.data()[1], version_major, version_minor, version_patch);
            add_node(frame.source_id(), frame.data()[0] << 8 | frame.data()[1], version);
        }
        //        else if(frame.type >= H9frame::Type::REG_EXTERNALLY_CHANGED) {
        //        /*else if(frame.type == H9frame::Type::REG_EXTERNALLY_CHANGED || frame.type == H9frame::Type::REG_INTERNALLY_CHANGED ||
        //                frame.type == H9frame::Type::REG_VALUE_BROADCAST || frame.type == H9frame::Type::REG_VALUE ||
        //                frame.type == H9frame::Type::ERROR || frame.type == H9frame::Type::NODE_HEARTBEAT ||
        //                frame.type == H9frame::Type::NODE_SPECIFIC_BULK0 || frame.type == H9frame::Type::NODE_SPECIFIC_BULK1 ||
        //                frame.type == H9frame::Type::NODE_SPECIFIC_BULK2 || frame.type == H9frame::Type::NODE_SPECIFIC_BULK3 ||
        //                frame.type == H9frame::Type::NODE_SPECIFIC_BULK4 || frame.type == H9frame::Type::NODE_SPECIFIC_BULK5 ||
        //                frame.type == H9frame::Type::NODE_SPECIFIC_BULK6 || frame.type == H9frame::Type::NODE_SPECIFIC_BULK7) {*/
        //
        //            devices_map_mtx.lock_shared();
        //            if (devices_map.count(frame.source_id)) {
        //                devices_map[frame.source_id]->update_device_state(frame);
        //                devices_map_mtx.unlock_shared();
        //            }
        //            else {
        //                devices_map_mtx.unlock_shared();
        //                h9_log_warn("Received frame from unknown device (id: %hu)", frame.source_id);
        //                auto seqnum = h9bus->get_next_seqnum(1);
        //                h9bus->send_node_discover(H9frame::Priority::LOW, seqnum, 1, frame.source_id);
        //            }
        //        }
    }
}

Node* NodeMgr::build_node(std::uint16_t node_id, std::uint16_t node_type, std::uint64_t node_version) noexcept {
    if (false && node_type == 5) {
        // return new AntennaSwitch(bus, node_id, node_version);
    }
    else {
        // Node _node = node(node_id);
        return new Node(this, bus, node_id, node_type, node_version);
    }
}

void NodeMgr::add_node(std::uint16_t node_id, std::uint16_t node_type, std::uint64_t node_version) noexcept {
    // TODO: sharelock, pelny lock tylko przy usuwaniu, update_device_last_seen_time przerobic na atomic
    nodes_map_mtx.lock();
    if (nodes_map.count(node_id)) {
        if (nodes_map[node_id]->device_type() != node_type || nodes_map[node_id]->device_version() != node_version) {
            uint16_t major = static_cast<std::uint16_t>(node_version >> 32);
            uint16_t minor = static_cast<std::uint16_t>(node_version >> 16);
            uint16_t patch = static_cast<std::uint16_t>(node_version);
            SPDLOG_LOGGER_WARN(logger, "Node {} (type: {}, version: {}.{}.{}) exist, override by node type: {} version: {}.{}.{}.",
                               node_id, nodes_map[node_id]->device_type(), nodes_map[node_id]->device_version_major(),
                               nodes_map[node_id]->device_version_minor(), nodes_map[node_id]->device_version_patch(),
                               node_type, major, minor, patch);
            delete nodes_map[node_id];
            nodes_map[node_id] = build_node(node_id, node_type, node_version);
        }
        else {
            nodes_map[node_id]->update_device_last_seen_time();
        }
    }
    else {
        nodes_map[node_id] = build_node(node_id, node_type, node_version);
    }
    nodes_map_mtx.unlock();
}

NodeMgr::NodeMgr(Bus* bus):
    frame_obs(bus, this),
    bus(bus) {
    logger = spdlog::get(H9dConfigurator::nodes_logger_name);
    nodes_update_thread_run = true;
    nodes_update_thread_desc = std::thread([this]() {
        this->nodes_update_thread();
    });
}

NodeMgr::~NodeMgr() {
    nodes_update_thread_run = false;
    if (nodes_update_thread_desc.joinable())
        nodes_update_thread_desc.join();

    for (auto it = nodes_map.begin(); it != nodes_map.end();) {
        delete it->second;
        it = nodes_map.erase(it);
    }
}

void NodeMgr::load_devices_description(const std::string& nodes_description_filename) {
    SPDLOG_LOGGER_INFO(logger, "Loading devices description file: '{}'.", nodes_description_filename);
    Node::nodedescloader.load_file(nodes_description_filename);
}

void NodeMgr::response_timeout_duration(int response_timeout_duration) {
    _response_timeout_duration = response_timeout_duration;
}

int NodeMgr::response_timeout_duration() {
    return _response_timeout_duration;
}

int NodeMgr::discover() noexcept {
    ExtH9Frame frame("?", H9frame::Type::DISCOVER, H9frame::BROADCAST_ID, 0, {});

    return bus->send_frame(frame);
}

int NodeMgr::active_devices_count() noexcept {
    nodes_map_mtx.lock_shared();
    int ret = nodes_map.size();
    nodes_map_mtx.unlock_shared();
    return ret;
}

bool NodeMgr::is_node_exist(std::uint16_t node_id) noexcept {
    nodes_map_mtx.lock_shared();
    bool ret = nodes_map.count(node_id);
    nodes_map_mtx.unlock_shared();
    return ret;
}

std::vector<NodeMgr::NodeDsc> NodeMgr::get_nodes_list() noexcept {
    nodes_map_mtx.lock_shared();
    std::vector<NodeMgr::NodeDsc> ret;

    for (auto it : nodes_map) {
        ret.push_back({it.first, it.second->device_type(), it.second->device_version_major(), it.second->device_version_minor(), it.second->device_version_patch(), it.second->device_name()});
    }

    nodes_map_mtx.unlock_shared();
    return std::move(ret);
}

// int DevicesMgr::attach_event_observer(TCPClientThread *observer, const std::string& event_name, std::uint16_t dev_id) noexcept {
//     devices_map_mtx.lock_shared();
//     if (devices_map.count(dev_id)) {
//         devices_map[dev_id]->attach_event_observer(observer, event_name);
//     }
//     else {
//         //h9_log_warn("Can not attach event observer %s:%s, device id: %hu does not exist", observer->get_remote_address().c_str(), observer->get_remote_port().c_str(), dev_id);
//     }
//     devices_map_mtx.unlock_shared();
//     return 0;
// }
//
// int DevicesMgr::detach_event_observer(TCPClientThread *observer, const std::string& event_name, std::uint16_t dev_id) noexcept {
//     devices_map_mtx.lock_shared();
//     if (devices_map.count(dev_id)) {
//         devices_map[dev_id]->detach_event_observer(observer, event_name);
//     }
//     devices_map_mtx.unlock_shared();
//     return 0;
// }
//
// std::vector<std::string> DevicesMgr::get_events_list(std::uint16_t dev_id) noexcept {
//     devices_map_mtx.lock_shared();
//     if (devices_map.count(dev_id)) {
//         auto ret = devices_map[dev_id]->get_events_list();
//         devices_map_mtx.unlock_shared();
//         return ret;
//     }
//     devices_map_mtx.unlock_shared();
//     return std::vector<std::string>();
// }
//
// std::vector<std::string> DevicesMgr::get_device_specific_methods(std::uint16_t dev_id) noexcept {
//     devices_map_mtx.lock_shared();
//     if (devices_map.count(dev_id)) {
//         auto ret = devices_map[dev_id]->get_device_specific_methods();
//         devices_map_mtx.unlock_shared();
//         return ret;
//     }
//     devices_map_mtx.unlock_shared();
//     return std::vector<std::string>();
// }

// H9Value DevicesMgr::execute_device_specific_method(std::uint16_t dev_id, const std::string &method_name, const H9Tuple& tuple) {
//     devices_map_mtx.lock_shared();
//     if (devices_map.count(dev_id)) {
//         try {
//             auto ret = devices_map[dev_id]->execute_device_specific_method(method_name, tuple);
//             devices_map_mtx.unlock_shared();
//             return ret;
//         }
//         catch (...) {
//             devices_map_mtx.unlock_shared();
//             throw;
//         }
//     }
//     devices_map_mtx.unlock_shared();
//     assert(0);
// }

std::vector<Node::RegisterDsc> NodeMgr::get_registers_list(std::uint16_t node_id) noexcept {
    nodes_map_mtx.lock_shared();
    if (nodes_map.count(node_id)) {
        auto ret = nodes_map[node_id]->get_registers_list();
        nodes_map_mtx.unlock_shared();
        return ret;
    }
    nodes_map_mtx.unlock_shared();
    return std::vector<Node::RegisterDsc>();
}

int NodeMgr::get_node_info(std::uint16_t dev_id, NodeMgr::NodeInfo& device_info) {
    nodes_map_mtx.lock_shared();
    if (nodes_map.count(dev_id)) {
        int ret = 0;
        auto dev = nodes_map[dev_id];
        device_info.id = dev_id;
        device_info.type = dev->device_type();
        device_info.name = dev->device_name();
        device_info.version_major = dev->device_version_major();
        device_info.version_minor = dev->device_version_minor();
        device_info.version_patch = dev->device_version_patch();
        device_info.created_time = dev->device_created_time();
        device_info.last_seen_time = dev->device_last_seen_time();
        device_info.description = dev->device_description();

        nodes_map_mtx.unlock_shared();
        return ret;
    }
    nodes_map_mtx.unlock_shared();
    throw DeviceNotExistException();
}

void NodeMgr::node_reset(std::uint16_t node_id) {
    nodes_map_mtx.lock_shared();
    if (nodes_map.count(node_id)) {
        try {
            nodes_map[node_id]->node_reset();
        }
        catch (...) {
            nodes_map_mtx.unlock_shared();
            throw;
        }
    }
    nodes_map_mtx.unlock_shared();
    throw DeviceNotExistException();
}

Node::regvalue_t NodeMgr::set_register(std::uint16_t node_id, std::uint8_t reg, Node::regvalue_t value) {
    nodes_map_mtx.lock_shared();
    if (nodes_map.count(node_id)) {
        try {
            auto ret = nodes_map[node_id]->set_register(reg, std::move(value));
            nodes_map_mtx.unlock_shared();
            return ret;
        }
        catch (...) {
            nodes_map_mtx.unlock_shared();
            throw;
        }
    }
    nodes_map_mtx.unlock_shared();
    throw DeviceNotExistException();
}

Node::regvalue_t NodeMgr::get_register(std::uint16_t node_id, std::uint8_t reg) {
    nodes_map_mtx.lock_shared();
    if (nodes_map.count(node_id)) {
        try {
            auto ret = nodes_map[node_id]->get_register(reg);
            nodes_map_mtx.unlock_shared();
            return ret;
        }
        catch (...) {
            nodes_map_mtx.unlock_shared();
            throw;
        }
    }
    nodes_map_mtx.unlock_shared();
    throw DeviceNotExistException();
}

Node::regvalue_t NodeMgr::set_register_bit(std::uint16_t node_id, std::uint8_t reg, std::uint8_t bit_num) {
    nodes_map_mtx.lock_shared();
    if (nodes_map.count(node_id)) {
        try {
            auto ret = nodes_map[node_id]->set_register_bit(reg, bit_num);
            nodes_map_mtx.unlock_shared();
            return ret;
        }
        catch (...) {
            nodes_map_mtx.unlock_shared();
            throw;
        }
    }
    nodes_map_mtx.unlock_shared();
    throw DeviceNotExistException();
}

Node::regvalue_t NodeMgr::clear_register_bit(std::uint16_t node_id, std::uint8_t reg, std::uint8_t bit_num) {
    nodes_map_mtx.lock_shared();
    if (nodes_map.count(node_id)) {
        try {
            auto ret = nodes_map[node_id]->clear_register_bit(reg, bit_num);
            nodes_map_mtx.unlock_shared();
            return ret;
        }
        catch (...) {
            nodes_map_mtx.unlock_shared();
            throw;
        }
    }
    nodes_map_mtx.unlock_shared();
    throw DeviceNotExistException();
}

Node::regvalue_t NodeMgr::toggle_register_bit(std::uint16_t node_id, std::uint8_t reg, std::uint8_t bit_num) {
    nodes_map_mtx.lock_shared();
    if (nodes_map.count(node_id)) {
        try {
            auto ret = nodes_map[node_id]->toggle_register_bit(reg, bit_num);
            nodes_map_mtx.unlock_shared();
            return ret;
        }
        catch (...) {
            nodes_map_mtx.unlock_shared();
            throw;
        }
    }
    nodes_map_mtx.unlock_shared();
    throw DeviceNotExistException();
}
