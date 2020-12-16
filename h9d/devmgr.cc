/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-11-19.
 *
 * Copyright (C) 2020 Kamil Palkowski. All rights reserved.
 */

#include "devmgr.h"
#include <cassert>
#include "bus.h"
#include "common/logger.h"
#include "tcpclientthread.h"


void DevMgr::on_frame_recv(H9frame frame) noexcept {
    frame_queue_mtx.lock();
    frame_queue.push(frame);
    frame_queue_mtx.unlock();
    frame_queue_cv.notify_one();
}

void DevMgr::devices_update_thread() {
    while (devices_update_thread_run) {
        std::unique_lock<std::mutex> lk(frame_queue_mtx);
        frame_queue_cv.wait(lk, [this](){ return !frame_queue.empty(); });
        H9frame frame = frame_queue.front();
        frame_queue.pop();
        int remained_frame = frame_queue.size();
        lk.unlock();

        h9_log_debug2("Devices update thread: pop frame (remained %d)", remained_frame);

        if (frame.type == H9frame::Type::NODE_INFO) {
            h9_log_notice("Dev discovered id: %d, type: %d, version: %d.%d", frame.source_id, frame.data[0] << 8 | frame.data[1], frame.data[2], frame.data[3]);
            add_device(frame.source_id, frame.data[0] << 8 | frame.data[1], frame.data[2] << 8 | frame.data[3]);
        }
        else if (frame.type == H9frame::Type::NODE_TURNED_ON) {
            h9_log_notice("Dev turned on id: %d, type: %d, version: %d.%d", frame.source_id, frame.data[0] << 8 | frame.data[1], frame.data[2], frame.data[3]);
            add_device(frame.source_id, frame.data[0] << 8 | frame.data[1], frame.data[2] << 8 | frame.data[3]);
        }
        //else if(frame.type >= H9frame::Type::SET_REG) { //SKIP BOOTLOADER FRAME
        else if(frame.type >= H9frame::Type::REG_EXTERNALLY_CHANGED) {
        /*else if(frame.type == H9frame::Type::REG_EXTERNALLY_CHANGED || frame.type == H9frame::Type::REG_INTERNALLY_CHANGED ||
                frame.type == H9frame::Type::REG_VALUE_BROADCAST || frame.type == H9frame::Type::REG_VALUE ||
                frame.type == H9frame::Type::ERROR || frame.type == H9frame::Type::NODE_HEARTBEAT ||
                frame.type == H9frame::Type::NODE_SPECIFIC_BULK0 || frame.type == H9frame::Type::NODE_SPECIFIC_BULK1 ||
                frame.type == H9frame::Type::NODE_SPECIFIC_BULK2 || frame.type == H9frame::Type::NODE_SPECIFIC_BULK3 ||
                frame.type == H9frame::Type::NODE_SPECIFIC_BULK4 || frame.type == H9frame::Type::NODE_SPECIFIC_BULK5 ||
                frame.type == H9frame::Type::NODE_SPECIFIC_BULK6 || frame.type == H9frame::Type::NODE_SPECIFIC_BULK7) {*/

            devices_map_mtx.lock_shared();
            if (devices_map.count(frame.source_id)) {
                devices_map[frame.source_id]->update_device_state(frame);
                devices_map_mtx.unlock_shared();
            }
            else {
                devices_map_mtx.unlock_shared();
                h9_log_warn("Received frame from unknown device (id: %hu)", frame.source_id);
                auto seqnum = h9bus->get_next_seqnum(1);
                h9bus->send_node_discover(H9frame::Priority::LOW, seqnum, 1, frame.source_id);
            }
        }
    }
}

void DevMgr::add_device(std::uint16_t node_id, std::uint16_t node_type, std::uint16_t node_version) noexcept {
    devices_map_mtx.lock();
    if (devices_map.count(node_id)) {
        if (devices_map[node_id]->get_device_type() != node_type || devices_map[node_id]->get_device_version() != node_version) {
            h9_log_warn("Node %hu (type: %hu, version: %hhu.%hhu) exist, override by node type: %hu version: %hhu.%hhu",
                    node_id, devices_map[node_id]->get_device_type(),
                    devices_map[node_id]->get_device_version_major(), devices_map[node_id]->get_device_version_minor(),
                    node_type, node_version >> 8, node_version);
            delete devices_map[node_id];
            devices_map[node_id] = Device::buildDevice(h9bus, node_id, node_type, node_version);
        }
        else {
            devices_map[node_id]->update_device_last_seen_time();
        }
    }
    else {
        devices_map[node_id] = Device::buildDevice(h9bus, node_id, node_type, node_version);
    }
    devices_map_mtx.unlock();
}

DevMgr::DevMgr(Bus *bus): FrameObserver(bus, H9FrameComparator()), h9bus(bus) {
    devices_update_thread_run = true;
    devices_update_thread_desc = std::thread([this]() {
        this->devices_update_thread();
    });
}

DevMgr::~DevMgr() {
    devices_update_thread_run = false;
    if (devices_update_thread_desc.joinable())
        devices_update_thread_desc.join();

    for (auto it = devices_map.begin(); it != devices_map.end();) {
        delete it->second;
        it = devices_map.erase(it);
    }
}

void DevMgr::load_config(DCtx *ctx) {
    h9_log_notice("Loading devices description file: %s", ctx->get_devices_description_filename().c_str());
    Device::devicedescloader.load_file(ctx->get_devices_description_filename());
}

int DevMgr::discover() noexcept {
    auto seqnum = h9bus->get_next_seqnum(1);
    h9bus->send_node_discover(H9frame::Priority::LOW, seqnum, 1);
    return 0;
}

int DevMgr::active_devices_count() noexcept {
    devices_map_mtx.lock_shared();
    int ret = devices_map.size();
    devices_map_mtx.unlock_shared();
    return ret;
}

bool DevMgr::is_device_exist(std::uint16_t dev_id) noexcept{
    devices_map_mtx.lock_shared();
    bool ret = devices_map.count(dev_id);
    devices_map_mtx.unlock_shared();
    return ret;
}

std::vector<DevMgr::DeviceDsc> DevMgr::get_devices_list() noexcept {
    devices_map_mtx.lock_shared();
    std::vector<DevMgr::DeviceDsc> ret;

    for (auto it: devices_map) {
        ret.push_back({it.first, it.second->get_device_type(), it.second->get_device_version_major(), it.second->get_device_version_minor(),  it.second->get_device_name()});
    }

    devices_map_mtx.unlock_shared();
    return std::move(ret);
}

int DevMgr::attach_event_observer(TCPClientThread *observer, const std::string& event_name, std::uint16_t dev_id) noexcept {
    devices_map_mtx.lock_shared();
    if (devices_map.count(dev_id)) {
        devices_map[dev_id]->attach_event_observer(observer, event_name);
    }
    else {
        h9_log_warn("Can not attach event observer %s:%s, device id: %hu does not exist", observer->get_remote_address().c_str(), observer->get_remote_port().c_str(), dev_id);
    }
    devices_map_mtx.unlock_shared();
    return 0;
}

int DevMgr::detach_event_observer(TCPClientThread *observer, const std::string& event_name, std::uint16_t dev_id) noexcept {
    devices_map_mtx.lock_shared();
    if (devices_map.count(dev_id)) {
        devices_map[dev_id]->detach_event_observer(observer, event_name);
    }
    devices_map_mtx.unlock_shared();
    return 0;
}

std::vector<std::string> DevMgr::get_events_list(std::uint16_t dev_id) noexcept {
    devices_map_mtx.lock_shared();
    if (devices_map.count(dev_id)) {
        auto ret = devices_map[dev_id]->get_events_list();
        devices_map_mtx.unlock_shared();
        return ret;
    }
    devices_map_mtx.unlock_shared();
    return std::vector<std::string>();
}

std::vector<std::string> DevMgr::get_device_specific_methods(std::uint16_t dev_id) noexcept {
    devices_map_mtx.lock_shared();
    if (devices_map.count(dev_id)) {
        auto ret = devices_map[dev_id]->get_device_specific_methods();
        devices_map_mtx.unlock_shared();
        return ret;
    }
    devices_map_mtx.unlock_shared();
    return std::vector<std::string>();
}

H9Value DevMgr::execute_device_specific_method(std::uint16_t dev_id, const std::string &method_name, const H9Tuple& tuple) {
    devices_map_mtx.lock_shared();
    if (devices_map.count(dev_id)) {
        try {
            auto ret = devices_map[dev_id]->execute_device_specific_method(method_name, tuple);
            devices_map_mtx.unlock_shared();
            return ret;
        }
        catch (...) {
            devices_map_mtx.unlock_shared();
            throw;
        }
    }
    devices_map_mtx.unlock_shared();
    assert(0);
}

std::vector<Device::RegisterDsc> DevMgr::get_registers_list(std::uint16_t dev_id) noexcept {
    devices_map_mtx.lock_shared();
    if (devices_map.count(dev_id)) {
        auto ret = devices_map[dev_id]->get_registers_list();
        devices_map_mtx.unlock_shared();
        return ret;
    }
    devices_map_mtx.unlock_shared();
    return std::vector<Device::RegisterDsc>();
}

ssize_t DevMgr::get_device_register(std::uint16_t dev_id, std::uint8_t reg, std::string &buf) noexcept {
    devices_map_mtx.lock_shared();
    if (devices_map.count(dev_id)) {
        auto ret = devices_map[dev_id]->get_register(reg, buf);
        devices_map_mtx.unlock_shared();
        return ret;
    }
    devices_map_mtx.unlock_shared();
    return -1;
}

ssize_t DevMgr::get_device_register(std::uint16_t dev_id, std::uint8_t reg, std::int64_t &buf) noexcept {
    devices_map_mtx.lock_shared();
    if (devices_map.count(dev_id)) {
        auto ret = devices_map[dev_id]->get_register(reg, buf);
        devices_map_mtx.unlock_shared();
        return ret;
    }
    devices_map_mtx.unlock_shared();
    return -1;
}

ssize_t DevMgr::set_device_register(std::uint16_t dev_id, std::uint8_t reg, const std::string& value) noexcept {
    devices_map_mtx.lock_shared();
    if (devices_map.count(dev_id)) {
        auto ret = devices_map[dev_id]->set_register(reg, value);
        devices_map_mtx.unlock_shared();
        return ret;
    }
    devices_map_mtx.unlock_shared();
    return -1;
}

ssize_t DevMgr::set_device_register(std::uint16_t dev_id, std::uint8_t reg, std::int64_t value, std::int64_t *setted) noexcept {
    devices_map_mtx.lock_shared();
    if (devices_map.count(dev_id)) {
        auto ret = devices_map[dev_id]->set_register(reg, value, setted);
        devices_map_mtx.unlock_shared();
        return ret;
    }
    devices_map_mtx.unlock_shared();
    return -1;
}

int DevMgr::get_device_info(std::uint16_t dev_id, DevMgr::DeviceInfo &device_info) noexcept {
    devices_map_mtx.lock_shared();
    if (devices_map.count(dev_id)) {
        int ret = 0;
        auto dev = devices_map[dev_id];
        device_info.id = dev_id;
        device_info.type = dev->get_device_type();
        device_info.name = dev->get_device_name();
        device_info.version_major = dev->get_device_version_major();
        device_info.version_minor = dev->get_device_version_minor();
        device_info.created_time = dev->get_device_created_time();
        device_info.last_seen_time = dev->get_device_last_seen_time();
        device_info.description = dev->get_device_description();

        devices_map_mtx.unlock_shared();
        return ret;
    }
    devices_map_mtx.unlock_shared();
    return -1;
}

int DevMgr::device_reset(std::uint16_t dev_id) noexcept {
    devices_map_mtx.lock_shared();
    if (devices_map.count(dev_id)) {
        auto ret = devices_map[dev_id]->reset();
        devices_map_mtx.unlock_shared();
        return ret;
    }
    devices_map_mtx.unlock_shared();
    return -1;
}
