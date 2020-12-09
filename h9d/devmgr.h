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
#if (defined(__APPLE__) && defined(__MACH__))
#include "sharedmutex.h"
#endif
#include "common/h9tuple.h"
#include "common/h9value.h"


class TCPClientThread;

class DevMgr: public FrameObserver {
private:
    Bus* const h9bus;
    void on_frame_recv(H9frame frame) noexcept override;

#if (defined(__APPLE__) && defined(__MACH__))
    SharedMutex devices_map_mtx;
#else
    std::shared_mutex devices_map_mtx;
#endif

    std::map<std::uint16_t, Device*> devices_map;

    std::mutex frame_queue_mtx;
    std::condition_variable frame_queue_cv; //TODO: counting_semaphore (C++20)?
    std::queue<H9frame> frame_queue;

    std::atomic_bool devices_update_thread_run;
    std::thread devices_update_thread_desc;
    void devices_update_thread();

    void add_device(std::uint16_t node_id, std::uint16_t node_type, std::uint16_t node_version) noexcept;
public:
    struct DeviceDsc {
        std::uint16_t id;
        std::uint16_t type;
        std::uint8_t version_major;
        std::uint8_t version_minor;
        std::string name;
    };
    struct DeviceInfo: public DeviceDsc {
        std::time_t created_time;
        std::time_t last_seen_time;
        std::string description;
    };

    explicit DevMgr(Bus *bus);
    DevMgr(const DevMgr &a) = delete;
    ~DevMgr();
    void load_config(DCtx *ctx);
    int discover() noexcept;

    int active_devices_count() noexcept;
    bool is_device_exist(std::uint16_t dev_id) noexcept;
    std::vector<DevMgr::DeviceDsc> get_devices_list() noexcept;

    int attach_event_observer(TCPClientThread *observer, const std::string& event_name, std::uint16_t dev_id) noexcept;
    int detach_event_observer(TCPClientThread *observer, const std::string& event_name, std::uint16_t dev_id) noexcept;
    std::vector<std::string> get_events_list(std::uint16_t dev_id) noexcept;

    std::vector<std::string> get_device_specific_methods(std::uint16_t dev_id) noexcept;
    H9Value execute_device_specific_method(std::uint16_t dev_id, const std::string &method_name, const H9Tuple& tuple);

    std::vector<Device::RegisterDsc> get_registers_list(std::uint16_t dev_id) noexcept;

    ssize_t get_device_register(std::uint16_t dev_id, std::uint8_t reg, std::string &buf) noexcept;
    ssize_t get_device_register(std::uint16_t dev_id, std::uint8_t reg, std::int64_t &buf) noexcept;
    ssize_t set_device_register(std::uint16_t dev_id, std::uint8_t reg, const std::string& value) noexcept;
    ssize_t set_device_register(std::uint16_t dev_id, std::uint8_t reg, std::int64_t value, std::int64_t *setted = nullptr) noexcept;

    int get_device_info(std::uint16_t dev_id, DeviceInfo &device_info) noexcept;
    int device_reset(std::uint16_t dev_id) noexcept;
};


#endif //H9_DEVMGR_H
