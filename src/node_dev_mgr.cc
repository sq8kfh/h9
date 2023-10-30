/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-11-19.
 *
 * Copyright (C) 2020-2023 Kamil Palkowski. All rights reserved.
 */

#include "node_dev_mgr.h"

#include <cassert>
#include <utility>

#include "antenna_switch_dev.h"
#include "bus.h"
#include "dev_node_exception.h"
#include "h9d_configurator.h"
#include "tcpclientthread.h"

void NodeDevMgr::on_frame_recv(const ExtH9Frame& frame) noexcept {
    notify_frame_observer(frame);

    frame_queue_mtx.lock();
    frame_queue.push(frame);
    frame_queue_mtx.unlock();
    frame_queue_cv.notify_one();
}

void NodeDevMgr::nodes_dev_update_thread() {
    while (nodes_update_thread_run) {
        std::unique_lock<std::mutex> lk(frame_queue_mtx);
        frame_queue_cv.wait(lk, [this]() { return !frame_queue.empty(); });
        ExtH9Frame frame = frame_queue.front();
        frame_queue.pop();
        int remained_frame = frame_queue.size();
        lk.unlock();

        SPDLOG_LOGGER_TRACE(logger, "Devices update thread: pop frame (remained {}).", remained_frame);

        if (frame.type() == H9frame::Type::NODE_INFO) {
            uint16_t node_type = frame.data()[0] << 8 | frame.data()[1];
            uint16_t version_major = frame.data()[2] << 8 | frame.data()[3];
            uint16_t version_minor = frame.data()[4] << 8 | frame.data()[5];
            uint16_t version_patch = frame.data()[6] << 8 | frame.data()[7];
            uint64_t version = version_major;
            version = version << 16 | version_minor;
            version = version << 16 | version_patch;
            SPDLOG_LOGGER_INFO(logger, "Dev discovered id: {}, type: {}, version: {}.{}.{}.", frame.source_id(), frame.data()[0] << 8 | frame.data()[1], version_major, version_minor, version_patch);
            add_node(frame.source_id(), node_type, version);

            update_dev_after_node_discovered(frame.source_id(), node_type);
        }
        else if (frame.type() == H9frame::Type::NODE_TURNED_ON) {
            uint16_t node_type = frame.data()[0] << 8 | frame.data()[1];
            uint16_t version_major = frame.data()[2] << 8 | frame.data()[3];
            uint16_t version_minor = frame.data()[4] << 8 | frame.data()[5];
            uint16_t version_patch = frame.data()[6] << 8 | frame.data()[7];
            uint64_t version = version_major;
            version = version << 16 | version_minor;
            version = version << 16 | version_patch;
            SPDLOG_LOGGER_INFO(logger, "Dev turned on id: {}, type: {}, version: {}.{}.{}.", frame.source_id(), frame.data()[0] << 8 | frame.data()[1], version_major, version_minor, version_patch);
            add_node(frame.source_id(), node_type, version);

            update_dev_after_node_discovered(frame.source_id(), node_type);
        }

        update_device_last_seen_time(frame.source_id());

        if (frame.type() >= H9frame::Type::REG_EXTERNALLY_CHANGED) {
            nodes_map_mtx.lock_shared();
            if (nodes_map.count(frame.source_id())) {
                nodes_map[frame.source_id()]->update_node_state(frame);
            }
            nodes_map_mtx.unlock_shared();
        }
    }
}

void NodeDevMgr::update_dev_after_node_discovered(std::uint16_t node_id, std::uint16_t node_type) {
    if (node_id == 32) {
        devs_map_mtx.lock();
        if (devs_map.count("*") == 0) {
            auto* dev = new AntennaSwitchDev(this, 32);
            devs_map["*"] = dev;
            devs_map_mtx.unlock();

            dev->init();
        }
        else {
            devs_map_mtx.unlock();
        }
    }
}

Node* NodeDevMgr::build_node(std::uint16_t node_id, std::uint16_t node_type, std::uint64_t node_version) noexcept {
    return new Node(this, bus, node_id, node_type, node_version);
}

void NodeDevMgr::add_node(std::uint16_t node_id, std::uint16_t node_type, std::uint64_t node_version) noexcept {
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
    }
    else {
        nodes_map[node_id] = build_node(node_id, node_type, node_version);
    }
    nodes_map_mtx.unlock();
}

void NodeDevMgr::update_device_last_seen_time(std::uint16_t node_id) noexcept {
    nodes_map_mtx.lock_shared();
    if (nodes_map.count(node_id)) {
        nodes_map[node_id]->update_node_last_seen_time();
    }
    nodes_map_mtx.unlock_shared();
}

NodeDevMgr::NodeDevMgr(Bus* bus):
    frame_obs(bus, this),
    bus(bus) {
    logger = spdlog::get(H9dConfigurator::nodes_logger_name);
    nodes_update_thread_run = true;
    nodes_update_thread_desc = std::thread([this]() {
        this->nodes_dev_update_thread();
    });
}

NodeDevMgr::~NodeDevMgr() {
    nodes_update_thread_run = false;
    if (nodes_update_thread_desc.joinable())
        nodes_update_thread_desc.join();

    for (auto it = nodes_map.begin(); it != nodes_map.end();) {
        delete it->second;
        it = nodes_map.erase(it);
    }
}

void NodeDevMgr::load_nodes_description(const std::string& nodes_description_filename) {
    SPDLOG_LOGGER_INFO(logger, "Loading devices description file: '{}'.", nodes_description_filename);
    Node::nodedescloader.load_file(nodes_description_filename);
}

void NodeDevMgr::response_timeout_duration(int response_timeout_duration) {
    _response_timeout_duration = response_timeout_duration;
}

int NodeDevMgr::response_timeout_duration() {
    return _response_timeout_duration;
}

int NodeDevMgr::discover() noexcept {
    ExtH9Frame frame("h9d", H9frame::Type::DISCOVER, H9frame::BROADCAST_ID, 0, {});

    return bus->send_frame(frame);
}

int NodeDevMgr::active_devices_count() noexcept {
    nodes_map_mtx.lock_shared();
    int ret = nodes_map.size();
    nodes_map_mtx.unlock_shared();
    return ret;
}

bool NodeDevMgr::is_node_exist(std::uint16_t node_id) noexcept {
    nodes_map_mtx.lock_shared();
    bool ret = nodes_map.count(node_id);
    nodes_map_mtx.unlock_shared();
    return ret;
}

std::vector<NodeDevMgr::NodeDsc> NodeDevMgr::get_nodes_list() noexcept {
    nodes_map_mtx.lock_shared();
    std::vector<NodeDevMgr::NodeDsc> ret;

    for (auto it : nodes_map) {
        ret.push_back({it.first, it.second->device_type(), it.second->device_version_major(), it.second->device_version_minor(), it.second->device_version_patch(), it.second->device_name()});
    }

    nodes_map_mtx.unlock_shared();
    return std::move(ret);
}

void NodeDevMgr::attach_node_state_observer(std::uint16_t node_id, Node::NodeStateObserver* obs) {
    nodes_map_mtx.lock_shared();
    if (nodes_map.count(node_id)) {
        nodes_map[node_id]->attach_node_state_observer(obs);
        nodes_map_mtx.unlock_shared();
        return;
    }
    nodes_map_mtx.unlock_shared();
    throw DeviceNotExistException();
}

void NodeDevMgr::detach_node_state_observer(std::uint16_t node_id, Node::NodeStateObserver* obs) {
    nodes_map_mtx.lock_shared();
    if (nodes_map.count(node_id)) {
        nodes_map[node_id]->detach_node_state_observer(obs);
        nodes_map_mtx.unlock_shared();
        return;
    }
    nodes_map_mtx.unlock_shared();
    throw DeviceNotExistException();
}

std::vector<Node::RegisterDsc> NodeDevMgr::get_registers_list(std::uint16_t node_id) noexcept {
    nodes_map_mtx.lock_shared();
    if (nodes_map.count(node_id)) {
        auto ret = nodes_map[node_id]->get_registers_list();
        nodes_map_mtx.unlock_shared();
        return ret;
    }
    nodes_map_mtx.unlock_shared();
    return std::vector<Node::RegisterDsc>();
}

int NodeDevMgr::get_node_info(std::uint16_t node_id, NodeDevMgr::NodeInfo& node_info) {
    nodes_map_mtx.lock_shared();
    if (nodes_map.count(node_id)) {
        int ret = 0;
        auto dev = nodes_map[node_id];
        node_info.id = node_id;
        node_info.type = dev->device_type();
        node_info.name = dev->device_name();
        node_info.version_major = dev->device_version_major();
        node_info.version_minor = dev->device_version_minor();
        node_info.version_patch = dev->device_version_patch();
        node_info.created_time = dev->device_created_time();
        node_info.last_seen_time = dev->device_last_seen_time();
        node_info.description = dev->device_description();

        nodes_map_mtx.unlock_shared();
        return ret;
    }
    nodes_map_mtx.unlock_shared();
    throw DeviceNotExistException();
}

void NodeDevMgr::node_reset(std::uint16_t node_id) {
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

Node::regvalue_t NodeDevMgr::set_register(std::uint16_t node_id, std::uint8_t reg, Node::regvalue_t value) {
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

Node::regvalue_t NodeDevMgr::get_register(std::uint16_t node_id, std::uint8_t reg) {
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

Node::regvalue_t NodeDevMgr::set_register_bit(std::uint16_t node_id, std::uint8_t reg, std::uint8_t bit_num) {
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

Node::regvalue_t NodeDevMgr::clear_register_bit(std::uint16_t node_id, std::uint8_t reg, std::uint8_t bit_num) {
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

Node::regvalue_t NodeDevMgr::toggle_register_bit(std::uint16_t node_id, std::uint8_t reg, std::uint8_t bit_num) {
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

std::vector<NodeDevMgr::DevDsc> NodeDevMgr::get_devs_list() noexcept {
    devs_map_mtx.lock_shared();
    std::vector<NodeDevMgr::DevDsc> ret;

    for (auto it : devs_map) {
        ret.push_back({it.first, it.second->type()});
    }

    devs_map_mtx.unlock_shared();
    return std::move(ret);
}

void NodeDevMgr::attach_dev_state_observer(std::string dev_id, DevStatusObserver* obs) {
    devs_map_mtx.lock_shared();
    if (devs_map.count(dev_id)) {
        devs_map[dev_id]->attach_dev_status_observer(obs);
        devs_map_mtx.unlock_shared();
        return;
    }
    devs_map_mtx.unlock_shared();
    throw DeviceNotExistException();
}

void NodeDevMgr::detach_dev_state_observer(std::string dev_id, DevStatusObserver* obs) {
    devs_map_mtx.lock_shared();
    if (devs_map.count(dev_id)) {
        devs_map[dev_id]->detach_dev_status_observer(obs);
        devs_map_mtx.unlock_shared();
        return;
    }
    devs_map_mtx.unlock_shared();
    throw DeviceNotExistException();
}
