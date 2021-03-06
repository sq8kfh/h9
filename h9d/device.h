/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-11-23.
 *
 * Copyright (C) 2020-2021 Kamil Palkowski. All rights reserved.
 */

#ifndef H9_DEVICE_H
#define H9_DEVICE_H

#include "config.h"
#include <set>
#include <map>
#include <mutex>
#include <vector>
#include "node.h"
#include "common/devicedescloader.h"
#include "common/h9tuple.h"
#include "common/h9value.h"
#include "protocol/genericmsg.h"


class DevMgr;
class TCPClientThread;

class Device: protected Node {
public:
    using RegisterDsc = DeviceDescLoader::RegisterDesc;
private:
    static DeviceDescLoader devicedescloader;

    const std::uint16_t device_type;
    const std::uint64_t device_version;

    const std::time_t created_time;
    std::time_t last_seen_time;

    std::map<std::uint8_t, RegisterDsc> register_map;

    friend class DevMgr;
    void update_device_state(H9frame frame) noexcept;
    void update_device_last_seen_time() noexcept;

    std::mutex event_name_mtx;
    std::map<std::string, std::set<TCPClientThread *>> event_observers;
    void attach_event_observer(TCPClientThread *observer, const std::string& event_name) noexcept;
    void detach_event_observer(TCPClientThread *observer, const std::string& event_name) noexcept;
protected:
    std::string device_name;
    std::string device_description;

    void notify_event_observer(std::string event_name, GenericMsg msg) noexcept;
    Device(Bus* bus, std::uint16_t node_id, std::uint16_t node_type, std::uint64_t node_version) noexcept;
public:
    static Device* buildDevice(Bus* bus, std::uint16_t node_id, std::uint16_t node_type, std::uint64_t node_version) noexcept;

    std::vector<std::string> get_events_list() noexcept;
    std::vector<RegisterDsc> get_registers_list() noexcept;
    virtual std::vector<std::string> get_device_specific_methods() noexcept;
    virtual H9Value execute_device_specific_method(const std::string &method_name, const H9Tuple& tuple);

    std::uint16_t get_device_id() const noexcept;
    std::uint16_t get_device_type() const noexcept;
    std::uint64_t get_device_version() const noexcept;
    std::uint16_t get_device_version_major() const noexcept;
    std::uint16_t get_device_version_minor() const noexcept;
    std::uint16_t get_device_version_patch() const noexcept;
    std::string get_device_name() const noexcept;
    std::time_t get_device_created_time() const noexcept;
    std::time_t get_device_last_seen_time() const noexcept;
    std::string get_device_description() const noexcept;

    ssize_t get_register(std::uint8_t reg, std::string &buf) noexcept;
    ssize_t get_register(std::uint8_t reg, std::int64_t &buf) noexcept;
    ssize_t set_register(std::uint8_t reg, const std::string& value) noexcept;
    ssize_t set_register(std::uint8_t reg, std::int64_t value, std::int64_t *setted = nullptr) noexcept;
};


#endif //H9_DEVICE_H
