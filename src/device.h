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

#include <exception>
#include <map>
#include <mutex>
#include <set>
#include <variant>
#include <vector>

#include "devicedescloader.h"
#include "node.h"

class DevicesMgr;
class TCPClientThread;

class Device: protected Node {
  public:
    using RegisterDsc = DeviceDescLoader::RegisterDesc;
    using regvalue_t = std::variant<std::string, std::int64_t, std::vector<std::uint8_t>>;

  private:
    std::shared_ptr<spdlog::logger> logger;
    static DeviceDescLoader devicedescloader;

    const std::uint16_t _device_type;
    const std::uint64_t _device_version;
    std::string _device_name;
    std::string _device_description;
    const std::time_t _created_time;
    std::time_t _last_seen_time;

    std::map<std::uint8_t, RegisterDsc> register_map;

    friend class DevicesMgr;
    //    void update_device_state(H9frame frame) noexcept;
    void update_device_last_seen_time() noexcept;
    //
    //    std::mutex event_name_mtx;
    //    std::map<std::string, std::set<TCPClientThread *>> event_observers;
    //    void attach_event_observer(TCPClientThread *observer, const std::string& event_name) noexcept;
    //    void detach_event_observer(TCPClientThread *observer, const std::string& event_name) noexcept;
  protected:
    ////    void notify_event_observer(std::string event_name, GenericMsg msg) noexcept;
    Device(NodeMgr* node_mgr, Bus* bus, std::uint16_t node_id, std::uint16_t node_type, std::uint64_t node_version) noexcept;

  public:
    //
    //    std::vector<std::string> get_events_list() noexcept;
    std::vector<RegisterDsc> get_registers_list() noexcept;
    //    virtual std::vector<std::string> get_device_specific_methods() noexcept;
    //    virtual H9Value execute_device_specific_method(const std::string &method_name, const H9Tuple& tuple);
    //
    //    std::uint16_t get_device_id() const noexcept;
    [[nodiscard]] std::uint16_t device_type() const noexcept;
    [[nodiscard]] std::uint64_t device_version() const noexcept;
    [[nodiscard]] std::uint16_t device_version_major() const noexcept;
    [[nodiscard]] std::uint16_t device_version_minor() const noexcept;
    [[nodiscard]] std::uint16_t device_version_patch() const noexcept;
    [[nodiscard]] std::string device_name() const noexcept;
    [[nodiscard]] std::time_t device_created_time() const noexcept;
    [[nodiscard]] std::time_t device_last_seen_time() const noexcept;
    [[nodiscard]] std::string device_description() const noexcept;

    void node_reset();
    regvalue_t set_register(std::uint8_t reg, regvalue_t value);
    regvalue_t get_register(std::uint8_t reg);
    regvalue_t set_register_bit(std::uint8_t reg, std::uint8_t bit_num);
    regvalue_t clear_register_bit(std::uint8_t reg, std::uint8_t bit_num);
    regvalue_t toggle_register_bit(std::uint8_t reg, std::uint8_t bit_num);
};

#endif // H9_DEVICE_H
