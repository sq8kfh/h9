/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-11-19.
 *
 * Copyright (C) 2020 Kamil Palkowski. All rights reserved.
 */

#ifndef H9_DEVMGR_H
#define H9_DEVMGR_H

#include "config.h"
#include <atomic>
#include <thread>
#include <map>
#include <queue>
#include <shared_mutex>
#include "bus.h"
#include "bus/h9frame.h"
#include "dctx.h"
#include "device.h"
#include "frameobserver.h"


class TCPClientThread;

class DevMgr: public FrameObserver {
private:
    Bus* const h9bus;
    void on_frame_recv(H9frame frame) override;

    //std::shared_mutex devices_map_mtx;
    std::mutex devices_map_mtx;             //TODO: shared_mutex (C++17)

    std::map<std::uint16_t, Device*> devices_map;

    std::mutex frame_queue_mtx;
    std::condition_variable frame_queue_cv; //TODO: counting_semaphore (C++20)?
    std::queue<H9frame> frame_queue;

    std::atomic_bool devices_update_thread_run;
    std::thread devices_update_thread_desc;
    void devices_update_thread();

    void add_device(std::uint16_t node_id, std::uint16_t node_type, std::uint16_t node_version);
public:
    typedef struct {
        std::uint16_t id;
        std::uint16_t type;
        std::uint8_t version_major;
        std::uint8_t version_minor;
        std::string type_name;
    } DeviceDsc;

    explicit DevMgr(Bus *bus);
    DevMgr(const DevMgr &a) = delete;
    ~DevMgr();
    void load_config(DCtx *ctx);
    void discover();

    int active_devices_count();
    bool is_device_exist(std::uint16_t dev_id);
    std::vector<DevMgr::DeviceDsc> get_devices_list();

    void attach_event_observer(TCPClientThread *observer, std::string event_name, std::uint16_t dev_id);
    void detach_event_observer(TCPClientThread *observer, std::string event_name, std::uint16_t dev_id);
    std::vector<std::string> get_events_list(std::uint16_t dev_id);

    std::vector<std::string> get_device_specific_methods(std::uint16_t dev_id);

    std::vector<Device::RegisterDsc> get_registers_list(std::uint16_t dev_id);

    ssize_t get_device_register(std::uint16_t dev_id, std::uint8_t reg, std::string &buf);
    ssize_t get_device_register(std::uint16_t dev_id, std::uint8_t reg, std::int64_t &buf);
    ssize_t set_device_register(std::uint16_t dev_id, std::uint8_t reg, std::string value);
    ssize_t set_device_register(std::uint16_t dev_id, std::uint8_t reg, std::int64_t value);
};


#endif //H9_DEVMGR_H
