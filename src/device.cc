/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-11-23.
 *
 * Copyright (C) 2020-2021 Kamil Palkowski. All rights reserved.
 */

#include "device.h"

#include <cassert>

#include "tcpclientthread.h"
#include "h9d_configurator.h"
#include "dev_node_exception.h"

DeviceDescLoader Device::devicedescloader;

// void Device::update_device_state(H9frame frame) noexcept {
////    update_device_last_seen_time();
////
////    if (frame.type == H9frame::Type::REG_EXTERNALLY_CHANGED || frame.type == H9frame::Type::REG_INTERNALLY_CHANGED ||
////        frame.type == H9frame::Type::REG_VALUE_BROADCAST) { // && frame.source_id == 32 && frame.data[0] == 10) {
////        DeviceEvent event_msg(node_id, "register_change");
////
////        event_msg.add_value("register_id", frame.data[0]);
////        event_msg.add_value("value_len", frame.dlc-1);
////        uint64_t tmp_value = 0;
////        for (int i=1; i < frame.dlc; ++i) {
////            tmp_value <<= 8;
////            tmp_value |= frame.data[i];
////        }
////        event_msg.add_value("value", tmp_value);
////
////        notify_event_observer("register_change", std::move(event_msg));
////    }
//}

 void Device::update_device_last_seen_time() noexcept {
     _last_seen_time = std::time(nullptr);
}

////void Device::attach_event_observer(TCPClientThread *observer, const std::string& event_name) noexcept {
////    event_name_mtx.lock();
////    event_observers[event_name].insert(observer);
////    event_name_mtx.unlock();
////    h9_log_info("Attach observer %s to '%s' event on device %hu", observer->get_client_idstring().c_str(), event_name.c_str(), get_node_id());
////}
////
////void Device::detach_event_observer(TCPClientThread *observer, const std::string& event_name) noexcept {
////    event_name_mtx.lock();
////    event_observers[event_name].erase(observer);
////    event_name_mtx.unlock();
////    h9_log_info("Detach observer %s from '%s' event on device %hu", observer->get_client_idstring().c_str(), event_name.c_str(), get_node_id());
////}
////
////void Device::notify_event_observer(std::string event_name, GenericMsg msg) noexcept {
////    event_name_mtx.lock();
////    for (auto observer : event_observers[event_name]) {
////        h9_log_debug("Notify observer %s, event '%s' on device %hu", observer->get_client_idstring().c_str(), event_name.c_str(), node_id);
////        try {
////            (*observer).send_msg(msg);
////        }
////        catch (std::runtime_error& e) {
////            h9_log_err("Notify observer %s error: %s", observer->get_client_idstring().c_str(), e.what());
////        }
////    }
////    event_name_mtx.unlock();
////}

 Device::Device(NodeMgr* node_mgr, Bus* bus, std::uint16_t node_id, std::uint16_t node_type, std::uint64_t node_version) noexcept:
    Node(node_mgr, bus, node_id),
    //Node(std::move(node)),
    _device_type(node_type),
    _device_version(node_version),
    _device_name("unknown"),
    _device_description(""),
    _created_time(std::time(nullptr)) {
    logger = spdlog::get(H9dConfigurator::devices_logger_name);

    SPDLOG_LOGGER_INFO(logger, "Create device descriptor: id: {} type: {} version: {}.{}.{}.", node_id, node_type,
                  device_version_major(), device_version_minor(), device_version_patch());

    _last_seen_time = _created_time;

    if (devicedescloader.get_device_name_by_type(node_type) != "") {
        _device_name = devicedescloader.get_device_name_by_type(node_type);
        _device_description = devicedescloader.get_device_description_by_type(node_type);
    }

    register_map[1] = {1, "Node type", "uint", 16, true, false, {}, ""};
    register_map[2] = {2, "Node version", "uint", 48, true, false, {}, ""};
    register_map[3] = {3, "Build metadata", "str", 48, true, false, {}, ""};
    register_map[4] = {4, "Node id", "uint", 9, true, true, {}, ""};
    register_map[5] = {5, "MCU type", "uint", 8, true, false, {}, ""};

    for (const auto &it: devicedescloader.get_device_register_by_type(node_type)) {
        register_map[it.first] = {it.second.number, it.second.name, it.second.type, it.second.size, it.second.readable, it.second.writable, it.second.bits_names, it.second.description};
    }
}

// std::vector<std::string> Device::get_events_list() noexcept {
//    std::vector<std::string> ret;
//    ret.push_back("register_change");
//
//    return ret;
//}

std::vector<Device::RegisterDsc> Device::get_registers_list() noexcept {
    std::vector<Device::RegisterDsc> ret;
    for (const auto& reg : register_map) {
        ret.push_back(reg.second);
    }
    return ret;
}

// std::vector<std::string> Device::get_device_specific_methods() noexcept {
//     return std::vector<std::string>();
// }
//
// H9Value Device::execute_device_specific_method(const std::string &method_name, const H9Tuple& tuple) {
//     assert(0);
// }
//
// std::uint16_t Device::get_device_id() const noexcept {
//     return node_id;
// }

std::uint16_t Device::device_type() const noexcept {
    return _device_type;
}

 std::uint64_t Device::device_version() const noexcept {
     return _device_version;
 }

std::uint16_t Device::device_version_major() const noexcept {
    return static_cast<std::uint16_t>(_device_version >> 32);
}

std::uint16_t Device::device_version_minor() const noexcept {
    return static_cast<std::uint16_t>(_device_version >> 16);
}

std::uint16_t Device::device_version_patch() const noexcept {
    return static_cast<std::uint16_t>(_device_version);
}

std::string Device::device_name() const noexcept {
    return _device_name;
}

std::time_t Device::device_created_time() const noexcept {
    return _created_time;
}

std::time_t Device::device_last_seen_time() const noexcept {
    return _last_seen_time;
}

std::string Device::device_description() const noexcept {
    return _device_description;
}

Device::regvalue_t Device::set_register(std::uint8_t reg, Device::regvalue_t value) {
    if (register_map.count(reg)) {
        if (register_map[reg].writable) {
            if (std::holds_alternative<std::int64_t>(value) && register_map[reg].type != "str") {
                auto v = std::get<std::int64_t>(value);
                if (register_map[reg].size <= 8) {
                    std::uint8_t tmp = v & ((1 << register_map[reg].size) - 1);
                    std::uint8_t val;
                    ssize_t ret;
                    if ((ret = set_reg("h9d", reg, tmp, &val)) < 0) {
                        if (ret == Node::TIMEOUT_ERROR) throw TimeoutException();
                        else if (ret == Node::MALFORMED_FRAME_ERROR) throw MalformedFrameException();
                        else throw NodeException(-ret);
                    }
                    else if (ret != sizeof(val)) {
                        throw SizeMismatchException();
                    }
                    return {val};
                }
                else if (register_map[reg].size <= 16) {
                    std::uint16_t tmp = v & ((1 << register_map[reg].size) - 1);
                    std::uint16_t val;
                    ssize_t ret;
                    if ((ret = set_reg("h9d", reg, tmp, &val)) < 0) {
                        if (ret == Node::TIMEOUT_ERROR) throw TimeoutException();
                        else if (ret == Node::MALFORMED_FRAME_ERROR) throw MalformedFrameException();
                        else throw NodeException(-ret);
                    }
                    else if (ret != sizeof(val)) {
                        throw SizeMismatchException();
                    }
                    return {val};
                }
                else if (register_map[reg].size <= 32) {
                    std::uint32_t tmp = v & ((1 << register_map[reg].size) - 1);
                    std::uint32_t val;
                    ssize_t ret;
                    if ((ret = set_reg("h9d", reg, tmp, &val)) < 0) {
                        if (ret == Node::TIMEOUT_ERROR) throw TimeoutException();
                        else if (ret == Node::MALFORMED_FRAME_ERROR) throw MalformedFrameException();
                        else throw NodeException(-ret);
                    }
                    else if (ret != sizeof(val)) {
                        throw SizeMismatchException();
                    }
                    return {val};
                }
            }
            else if (std::holds_alternative<std::string>(value) && register_map[reg].type == "str") {
                auto v = std::get<std::string>(value);
                size_t len = register_map[reg].size / 8;
                len = len < v.size() ? len : v.size();

                ssize_t ret_len = (register_map[reg].size + 7)/8 + 1;
                auto *ret_buf = new std::uint8_t[ret_len];

                ssize_t ret;
                if ((ret = set_reg("h9d", reg, len, reinterpret_cast<const std::uint8_t *>(v.c_str()), ret_buf, ret_len)) < 0) {
                    if (ret == Node::TIMEOUT_ERROR) throw TimeoutException();
                    else if (ret == Node::MALFORMED_FRAME_ERROR) throw MalformedFrameException();
                    else throw NodeException(-ret);
                }

                ret_buf[ret] = '\0';
                std::string ret_str = {reinterpret_cast<char*>(ret_buf)};
                delete [] ret_buf;

                return {ret_str};
            }
            else if (std::holds_alternative<std::vector<std::uint8_t>>(value)) {
                auto v = std::get<std::vector<std::uint8_t>>(value);
                size_t len = (register_map[reg].size + 7)/8;
                if (len == v.size()) {
                    auto *ret_buf = new std::uint8_t[len];

                    ssize_t ret;
                    if ((ret = set_reg("h9d", reg, len, v.data(), ret_buf)) < 0) {
                        delete [] ret_buf;
                        if (ret == Node::TIMEOUT_ERROR) throw TimeoutException();
                        else if (ret == Node::MALFORMED_FRAME_ERROR) throw MalformedFrameException();
                        else throw NodeException(-ret);
                    }
                    else if (ret != len) {
                        throw SizeMismatchException();
                    }

                    Device::regvalue_t ret_v = {std::vector<std::uint8_t> (ret_buf, ret_buf+ret)};
                    delete [] ret_buf;
                    return std::move(ret_v);
                }
            }
            throw UnsupportedRegisterDataConversionException(reg);
        }
        throw RegisterNotWritableException(reg);
    }
    else {
        throw RegisterNotExistException(reg);
    }
}

Device::regvalue_t Device::get_register(std::uint8_t reg) {
    if (register_map.count(reg)) {
        if (register_map[reg].readable) {
            if (register_map[reg].type != "str") {
                if (register_map[reg].size <= 8) {
                    std::uint8_t val;
                    ssize_t ret;
                    if ((ret = get_reg("h9d", reg, &val)) < 0) {
                        if (ret == Node::TIMEOUT_ERROR) throw TimeoutException();
                        else if (ret == Node::MALFORMED_FRAME_ERROR) throw MalformedFrameException();
                        else throw NodeException(-ret);
                    }
                    else if (ret != sizeof(val)) {
                        throw SizeMismatchException();
                    }
                    return {val};
                }
                else if (register_map[reg].size <= 16) {
                    std::uint16_t val;
                    ssize_t ret;
                    if ((ret = get_reg("h9d", reg, &val)) < 0) {
                        if (ret == Node::TIMEOUT_ERROR) throw TimeoutException();
                        else if (ret == Node::MALFORMED_FRAME_ERROR) throw MalformedFrameException();
                        else throw NodeException(-ret);
                    }
                    else if (ret != sizeof(val)) {
                        throw SizeMismatchException();
                    }
                    return {val};
                }
                else if (register_map[reg].size <= 32) {
                    std::uint32_t val;
                    ssize_t ret;
                    if ((ret = get_reg("h9d", reg, &val)) < 0) {
                        if (ret == Node::TIMEOUT_ERROR) throw TimeoutException();
                        else if (ret == Node::MALFORMED_FRAME_ERROR) throw MalformedFrameException();
                        else throw NodeException(-ret);
                    }
                    else if (ret != sizeof(val)) {
                        throw SizeMismatchException();
                    }
                    return {val};
                }
                else {
                    size_t len = (register_map[reg].size + 7) / 8;
                    auto *buf = new std::uint8_t[len];

                    ssize_t ret;
                    if ((ret = get_reg("h9d", reg, len, buf)) < 0) {
                        delete [] buf;
                        if (ret == Node::TIMEOUT_ERROR) throw TimeoutException();
                        else if (ret == Node::MALFORMED_FRAME_ERROR) throw MalformedFrameException();
                        else throw NodeException(-ret);
                    }
                    else if (ret != len) {
                        throw SizeMismatchException();
                    }
                    Device::regvalue_t ret_v = {std::vector<std::uint8_t> (buf, buf+ret)};
                    delete [] buf;
                    return std::move(ret_v);
                }
            }
            else if (register_map[reg].type == "str") {
                size_t len = (register_map[reg].size + 7)/8 + 1;
                auto *buf = new std::uint8_t[len];
                ssize_t ret;
                if ((ret = get_reg("h9d", reg, len - 1, buf)) < 0) {
                    delete [] buf;
                    if (ret == Node::TIMEOUT_ERROR) throw TimeoutException();
                    else if (ret == Node::MALFORMED_FRAME_ERROR) throw MalformedFrameException();
                    else throw NodeException(-ret);
                }
                buf[ret] = '\0';
                std::string ret_str = {reinterpret_cast<char*>(buf)};
                delete [] buf;
                return std::move(ret_str);
            }
            throw UnsupportedRegisterDataConversionException(reg);
        }
        throw RegisterNotReadableException(reg);
    }
    else {
        throw RegisterNotExistException(reg);
    }
}
