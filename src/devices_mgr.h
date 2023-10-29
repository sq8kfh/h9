/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-11-19.
 *
 * Copyright (C) 2020-2023 Kamil Palkowski. All rights reserved.
 */

#pragma once

#include "config.h"

#include <atomic>
#include <map>
#include <queue>
#include <shared_mutex>
#include <thread>

#include "bus.h"
#include "node_mgr.h"
#include "device.h"
#include <spdlog/spdlog.h>

class TCPClientThread;

class DevicesMgr: public NodeMgr {
  private:
    std::shared_ptr<spdlog::logger> logger;

    virtual void on_frame_recv(const ExtH9Frame& frame) noexcept;

    std::shared_mutex devices_map_mtx;

    std::map<std::uint16_t, Device*> devices_map;

    std::mutex frame_queue_mtx;
    std::condition_variable frame_queue_cv; // TODO: counting_semaphore (C++20)?
    std::queue<ExtH9Frame> frame_queue;

    std::atomic_bool devices_update_thread_run;
    std::thread devices_update_thread_desc;
    void devices_update_thread();

    Device* build_device(std::uint16_t node_id, std::uint16_t node_type, std::uint64_t node_version) noexcept;
    void add_device(std::uint16_t node_id, std::uint16_t node_type, std::uint64_t node_version) noexcept;

  public:
    struct DeviceDsc {
        std::uint16_t id;
        std::uint16_t type;
        std::uint16_t version_major;
        std::uint16_t version_minor;
        std::uint16_t version_patch;
        std::string name;
    };

    struct DeviceInfo: public DeviceDsc {
        std::time_t created_time;
        std::time_t last_seen_time;
        std::string description;
    };

    explicit DevicesMgr(Bus* bus);
    DevicesMgr(const DevicesMgr& a) = delete;
    ~DevicesMgr();
    void load_devices_description(const std::string& devices_description_filename);

    int discover() noexcept;

    int active_devices_count() noexcept;
    bool is_device_exist(std::uint16_t node_id) noexcept;
    std::vector<DevicesMgr::DeviceDsc> get_devices_list() noexcept;

//    int attach_event_observer(TCPClientThread* observer, const std::string& event_name, std::uint16_t dev_id) noexcept;
//    int detach_event_observer(TCPClientThread* observer, const std::string& event_name, std::uint16_t dev_id) noexcept;
//    std::vector<std::string> get_events_list(std::uint16_t dev_id) noexcept;
//
//    std::vector<std::string> get_device_specific_methods(std::uint16_t dev_id) noexcept;
    //H9Value execute_device_specific_method(std::uint16_t dev_id, const std::string& method_name, const H9Tuple& tuple);

    std::vector<Device::RegisterDsc> get_registers_list(std::uint16_t node_id) noexcept;

    int get_device_info(std::uint16_t dev_id, DeviceInfo& device_info);
    void node_reset(std::uint16_t node_id);
    Device::regvalue_t set_register(std::uint16_t node_id, std::uint8_t reg, Device::regvalue_t value);
    Device::regvalue_t get_register(std::uint16_t node_id, std::uint8_t reg);
    Device::regvalue_t set_register_bit(std::uint16_t node_id, std::uint8_t reg, std::uint8_t bit_num);
    Device::regvalue_t clear_register_bit(std::uint16_t node_id, std::uint8_t reg, std::uint8_t bit_num);
    Device::regvalue_t toggle_register_bit(std::uint16_t node_id, std::uint8_t reg, std::uint8_t bit_num);
};
