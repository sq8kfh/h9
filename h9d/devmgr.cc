/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-11-19.
 *
 * Copyright (C) 2020 Kamil Palkowski. All rights reserved.
 */

#include "devmgr.h"
#include "bus.h"
#include "common/logger.h"
#include "tcpclientthread.h"


void DevMgr::on_frame_recv(H9frame frame) {
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
        else if(frame.type == H9frame::Type::REG_EXTERNALLY_CHANGED || frame.type == H9frame::Type::REG_INTERNALLY_CHANGED ||
                frame.type == H9frame::Type::REG_VALUE_BROADCAST || frame.type == H9frame::Type::REG_VALUE ||
                frame.type == H9frame::Type::ERROR || frame.type == H9frame::Type::NODE_HEARTBEAT ||
                frame.type == H9frame::Type::NODE_SPECIFIC_BULK0 || frame.type == H9frame::Type::NODE_SPECIFIC_BULK1 ||
                frame.type == H9frame::Type::NODE_SPECIFIC_BULK2 || frame.type == H9frame::Type::NODE_SPECIFIC_BULK3 ||
                frame.type == H9frame::Type::NODE_SPECIFIC_BULK4 || frame.type == H9frame::Type::NODE_SPECIFIC_BULK5 ||
                frame.type == H9frame::Type::NODE_SPECIFIC_BULK6 || frame.type == H9frame::Type::NODE_SPECIFIC_BULK7) {

            //devices_map_mtx.lock_shared();
            devices_map_mtx.lock();
            if (devices_map.count(frame.source_id)) {
                devices_map[frame.source_id]->update_device_state(frame);
                //devices_map_mtx.unlock_shared()
                devices_map_mtx.unlock();
            }
            else {
                //devices_map_mtx.unlock_shared()
                devices_map_mtx.unlock();
                h9_log_warn("Received frame from unknown device (id: %hu)", frame.source_id);
                auto seqnum = h9bus->get_next_seqnum(1);
                h9bus->send_node_discover(H9frame::Priority::LOW, seqnum, 1, frame.source_id);
            }
        }
    }
}

void DevMgr::add_device(std::uint16_t node_id, std::uint16_t node_type, std::uint16_t node_version) {
    devices_map_mtx.lock();
    if (devices_map.count(node_id)) {
        if (devices_map[node_id]->get_node_type() != node_type || devices_map[node_id]->get_node_version() != node_version) {
            h9_log_warn("Node %hu (type: %hu, version: %hhu.%hhu) exist, override by node type: %hu version: %hhu.%hhu",
                    node_id, devices_map[node_id]->get_node_type(),
                    devices_map[node_id]->get_node_version() >> 8, devices_map[node_id]->get_node_version(),
                    node_type, node_version >> 8, node_version);
            delete devices_map[node_id];
            devices_map[node_id] = new Device(h9bus, node_id, node_type, node_version);
        }
    }
    else {
        devices_map[node_id] = new Device(h9bus, node_id, node_type, node_version);
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

}

void DevMgr::discover() {
    auto seqnum = h9bus->get_next_seqnum(1);
    h9bus->send_node_discover(H9frame::Priority::LOW, seqnum, 1);
}

void DevMgr::attach_event_observer(TCPClientThread *observer, std::string event_name, std::uint16_t dev_id) {
    //devices_map_mtx.lock_shared();
    devices_map_mtx.lock();
    if (devices_map.count(dev_id)) {
        devices_map[dev_id]->attach_event_observer(observer, event_name);
    }
    else {
        h9_log_warn("Can not attach event observer %s:%s, device id: %hu does not exist", observer->get_remote_address().c_str(), observer->get_remote_port().c_str(), dev_id);
    }
    //devices_map_mtx.unlock_shared()
    devices_map_mtx.unlock();
}

void DevMgr::detach_event_observer(TCPClientThread *observer, std::string event_name, std::uint16_t dev_id) {
    //devices_map_mtx.lock_shared();
    devices_map_mtx.lock();
    if (devices_map.count(dev_id)) {
        devices_map[dev_id]->detach_event_observer(observer, event_name);
    }
    //devices_map_mtx.unlock_shared()
    devices_map_mtx.unlock();
}

int DevMgr::active_devices_count() {
    //devices_map_mtx.lock_shared();
    devices_map_mtx.lock();
    int ret = devices_map.size();
    //devices_map_mtx.unlock_shared()
    devices_map_mtx.unlock();
    return ret;
}
