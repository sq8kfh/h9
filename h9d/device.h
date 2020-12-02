/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-11-23.
 *
 * Copyright (C) 2020 Kamil Palkowski. All rights reserved.
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
#include "protocol/genericmsg.h"


class DevMgr;
class TCPClientThread;

class Device: protected Node {
public:
    using RegisterDsc = DeviceDescLoader::RegisterDesc;
private:
    static DeviceDescLoader devicedescloader;

    const std::uint16_t device_type;
    const std::uint16_t device_version;

    const std::time_t created_time;
    std::time_t last_seen_time;

    std::map<std::uint8_t, RegisterDsc> register_map;

    friend class DevMgr;
    void update_device_state(H9frame frame);
    void update_device_last_seen_time();

    std::mutex event_name_mtx;
    std::map<std::string, std::set<TCPClientThread *>> event_observers;
    void attach_event_observer(TCPClientThread *observer, std::string event_name);
    void detach_event_observer(TCPClientThread *observer, std::string event_name);
protected:
    std::string device_name;
    std::string device_description;

    void notify_event_observer(std::string event_name, GenericMsg msg);
public:
    Device(Bus* bus, std::uint16_t node_id, std::uint16_t node_type, std::uint16_t node_version) noexcept;

    std::vector<std::string> get_events_list();
    std::vector<RegisterDsc> get_registers_list();
    virtual std::vector<std::string> get_device_specific_methods();

    std::uint16_t get_device_id() noexcept;
    std::uint16_t get_device_type() noexcept;
    std::uint16_t get_device_version() noexcept;
    std::uint8_t get_device_version_major() noexcept;
    std::uint8_t get_device_version_minor() noexcept;
    std::string get_device_name() noexcept;
    std::time_t get_device_created_time() noexcept;
    std::time_t get_device_last_seen_time() noexcept;
    std::string get_device_description() noexcept;

    ssize_t get_register(std::uint8_t reg, std::string &buf);
    ssize_t get_register(std::uint8_t reg, std::int64_t &buf);
    ssize_t set_register(std::uint8_t reg, std::string value);
    ssize_t set_register(std::uint8_t reg, std::int64_t value, std::int64_t *setted = nullptr);
};


#endif //H9_DEVICE_H
