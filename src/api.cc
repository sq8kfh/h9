/*
 * H9 project
 *
 * Created by crowx on 2023-09-11.
 *
 * Copyright (C) 2023 Kamil Palkowski. All rights reserved.
 */

#include "api.h"

#include "config.h"

#include <spdlog/spdlog.h>

#include "dev_node_exception.h"
#include "h9d_configurator.h"

nlohmann::json API::get_version(TCPClientThread* client_thread, const jsonrpcpp::Id& id, const jsonrpcpp::Parameter& params) {
    nlohmann::json r({{"version", H9dConfigurator::version()}});

    if (!H9dConfigurator::version_commit_sha().empty())
        r["commit_sha"] = H9dConfigurator::version_commit_sha();
    if (H9dConfigurator::version_dirty())
        r["dirty"] = H9dConfigurator::version_dirty();
    return std::move(r);
}

nlohmann::json API::get_methods_list(TCPClientThread* client_thread, const jsonrpcpp::Id& id, const jsonrpcpp::Parameter& params) {
    nlohmann::json methods;
    for (const auto& [method, v] : api_methods) {
        methods.push_back(method);
    }
    nlohmann::json r = {
        {"methods_list", std::move(methods)},
    };

    return std::move(r);
}

nlohmann::json API::subscribe(TCPClientThread* client_thread, const jsonrpcpp::Id& id, const jsonrpcpp::Parameter& params) {
    try {
        std::string event_name = params.param_map.at("event").get<std::string>();

        if (event_name == "frame") {
            client_thread->set_frame_observer(new ClientFrameObs(client_thread, bus, H9FrameComparator()));
        }

        nlohmann::json r = {{"return", "ok"}};

        SPDLOG_DEBUG("subscribe: {}", event_name);
        return std::move(r);
    }
    catch (std::out_of_range& e) {
        SPDLOG_ERROR("Incorrect parameters during invoke '{}' by {} - {}", __FUNCTION__, client_thread->get_client_idstring(), e.what());
        SPDLOG_DEBUG("Dump '{}' calling params: {}.", __FUNCTION__, params.to_json().dump());
        throw jsonrpcpp::InvalidParamsException(e.what(), id);
    }
    catch (nlohmann::detail::type_error& e) {
        SPDLOG_ERROR("Incorrect parameters during invoke '{}' by {} - {}", __FUNCTION__, client_thread->get_client_idstring(), e.what());
        SPDLOG_DEBUG("Dump '{}' calling params: {}.", __FUNCTION__, params.to_json().dump());
        throw jsonrpcpp::InvalidParamsException(e.what(), id);
    }
}

nlohmann::json API::send_frame(TCPClientThread* client_thread, const jsonrpcpp::Id& id, const jsonrpcpp::Parameter& params) {
    try {
        bool raw = false;
        if (params.param_map.count("raw") && params.param_map.at("raw").get<bool>()) {
            raw = true;
        }
        ExtH9Frame frame = params.param_map.at("frame").get<ExtH9Frame>(); // nie zlapiemy wyjatkow! gdzies po drodze jest noexcept

        if (raw && frame.invalid_member() & ~ExtH9Frame::VALID_ORIGIN) {
            SPDLOG_ERROR("Incorrect raw frame during invoke 'send_frame' by {}", __FUNCTION__, client_thread->get_client_idstring());
            SPDLOG_DEBUG("Raw 'send_frame' dump frame params: {}.", __FUNCTION__, params.param_map.at("frame").dump());
            throw jsonrpcpp::InvalidParamsException("Incorrect raw frame during invoke 'send_frame'", id);
        }
        else if (frame.invalid_member() & ~(ExtH9Frame::VALID_ORIGIN | ExtH9Frame::VALID_SEQNUM | ExtH9Frame::VALID_SOURCE_ID)) {
            SPDLOG_ERROR("Incorrect frame invoke 'send_frame' by {}", __FUNCTION__, client_thread->get_client_idstring());
            SPDLOG_DEBUG("'send_frame' dump frame params: {}.", __FUNCTION__, params.param_map.at("frame").dump());
            throw jsonrpcpp::InvalidParamsException("Incorrect frame during invoke 'send_frame'", id);
        }

        frame.origin(client_thread->get_client_idstring());
        int ret = bus->send_frame(std::move(frame), raw);

        nlohmann::json r = {{"seqnum", ret}};

        SPDLOG_DEBUG("Call send_frame - raw: {}, result seqnum: {}", raw, ret);
        return std::move(r);
    }
    catch (std::out_of_range& e) {
        SPDLOG_ERROR("Incorrect parameters during invoke '{}' by {} - {}", __FUNCTION__, client_thread->get_client_idstring(), e.what());
        SPDLOG_DEBUG("Dump '{}' calling params: {}.", __FUNCTION__, params.to_json().dump());
        throw jsonrpcpp::InvalidParamsException(e.what(), id);
    }
    catch (nlohmann::detail::type_error& e) {
        SPDLOG_ERROR("Incorrect parameters during invoke '{}' by {} - {}", __FUNCTION__, client_thread->get_client_idstring(), e.what());
        SPDLOG_DEBUG("Dump '{}' calling params: {}.", __FUNCTION__, params.to_json().dump());
        throw jsonrpcpp::InvalidParamsException(e.what(), id);
    }
}

nlohmann::json API::get_stats(TCPClientThread* client_thread, const jsonrpcpp::Id& id, const jsonrpcpp::Parameter& params) {
    // std::uint32_t d = 256;
    // node_mgr->get_node(16).reset(client_thread->get_client_idstring());
    // node_mgr->get_node(16).set_reg(client_thread->get_client_idstring(), 12, d);
    return std::move(MetricsCollector::metrics_to_json());
}

nlohmann::json API::authenticate(TCPClientThread* client_thread, const jsonrpcpp::Id& id, const jsonrpcpp::Parameter& params) {
    try {
        std::string entity = params.param_map.at("entity").get<std::string>();
        client_thread->entity(entity);
        nlohmann::json r = {{"authentication", true}};
        return std::move(r);
    }
    catch (std::out_of_range& e) {
        SPDLOG_ERROR("Incorrect parameters during invoke '{}' by {} - {}", __FUNCTION__, client_thread->get_client_idstring(), e.what());
        SPDLOG_DEBUG("Dump '{}' calling params: {}.", __FUNCTION__, params.to_json().dump());
        throw jsonrpcpp::InvalidParamsException(e.what(), id);
    }
    catch (nlohmann::detail::type_error& e) {
        SPDLOG_ERROR("Incorrect parameters during invoke '{}' by {} - {}", __FUNCTION__, client_thread->get_client_idstring(), e.what());
        SPDLOG_DEBUG("Dump '{}' calling params: {}.", __FUNCTION__, params.to_json().dump());
        throw jsonrpcpp::InvalidParamsException(e.what(), id);
    }
}

nlohmann::json API::get_devices_list(TCPClientThread* client_thread, const jsonrpcpp::Id& id, const jsonrpcpp::Parameter& params) {
    nlohmann::json r = nlohmann::json::array();
    for (auto& d : dev_mgr->get_devices_list()) {
        r.push_back({{"id", d.id}, {"type", d.type}, {"name", d.name}});
    }
    return std::move(r);
}

nlohmann::json API::get_device_info(TCPClientThread* client_thread, const jsonrpcpp::Id& id, const jsonrpcpp::Parameter& params) {
    std::uint16_t dev_id = 0xffff;
    try {
        dev_id = params.param_map.at("dev_id").get<std::uint16_t>();
    }
    catch (std::out_of_range& e) {
        SPDLOG_ERROR("Incorrect parameters during invoke '{}' by {} - {}", __FUNCTION__, client_thread->get_client_idstring(), e.what());
        SPDLOG_DEBUG("Dump '{}' calling params: {}.", __FUNCTION__, params.to_json().dump());
        throw jsonrpcpp::InvalidParamsException(e.what(), id);
    }
    catch (nlohmann::detail::type_error& e) {
        SPDLOG_ERROR("Incorrect parameters during invoke '{}' by {} - {}", __FUNCTION__, client_thread->get_client_idstring(), e.what());
        SPDLOG_DEBUG("Dump '{}' calling params: {}.", __FUNCTION__, params.to_json().dump());
        throw jsonrpcpp::InvalidParamsException(e.what(), id);
    }
    DevicesMgr::DeviceInfo device_info;
    if (dev_mgr->get_device_info(dev_id, device_info) < 0) {
        throw jsonrpcpp::RequestException(jsonrpcpp::Error("Device " + std::to_string(dev_id) + "does not exist.", DEVICE_DOES_NOT_EXIST), id);
    }

    //     struct DeviceDsc {
    //        std::uint16_t id;
    //        std::uint16_t type;
    //        std::uint16_t version_major;
    //        std::uint16_t version_minor;
    //        std::uint16_t version_patch;
    //        std::string name;
    //    };
    //
    //    struct DeviceInfo: public DeviceDsc {
    //        std::time_t created_time;
    //        std::time_t last_seen_time;
    //        std::string description;
    //    };

    nlohmann::json r = nlohmann::json({
        {"id", device_info.id},
        {"type", device_info.type},
        {"version_major", device_info.version_major},
        {"version_minor", device_info.version_minor},
        {"version_patch", device_info.version_patch},
        {"name", device_info.name},
        {"created_time", device_info.created_time},
        {"last_seen_time", device_info.last_seen_time},
        {"description", device_info.description},
    });
    return std::move(r);
}

nlohmann::json API::get_registers_list(TCPClientThread* client_thread, const jsonrpcpp::Id& id, const jsonrpcpp::Parameter& params) {
    std::uint16_t dev_id = 0xffff;
    try {
        dev_id = params.param_map.at("dev_id").get<std::uint16_t>();
    }
    catch (std::out_of_range& e) {
        SPDLOG_ERROR("Incorrect parameters during invoke '{}' by {} - {}", __FUNCTION__, client_thread->get_client_idstring(), e.what());
        SPDLOG_DEBUG("Dump '{}' calling params: {}.", __FUNCTION__, params.to_json().dump());
        throw jsonrpcpp::InvalidParamsException(e.what(), id);
    }
    catch (nlohmann::detail::type_error& e) {
        SPDLOG_ERROR("Incorrect parameters during invoke '{}' by {} - {}", __FUNCTION__, client_thread->get_client_idstring(), e.what());
        SPDLOG_DEBUG("Dump '{}' calling params: {}.", __FUNCTION__, params.to_json().dump());
        throw jsonrpcpp::InvalidParamsException(e.what(), id);
    }
    if (dev_mgr->is_device_exist(dev_id)) {
        nlohmann::json r = nlohmann::json::array();

        for (auto& reg : dev_mgr->get_registers_list(dev_id)) {
            r.push_back({{"number", reg.number},
                         {"name", reg.name},
                         {"type", reg.type},
                         {"size", reg.size},
                         {"readable", reg.readable},
                         {"writable", reg.writable},
                         {"bits_names", reg.bits_names},
                         {"description", reg.description}});
        }
        return std::move(r);
    }
    throw jsonrpcpp::RequestException(jsonrpcpp::Error("Device " + std::to_string(dev_id) + "does not exist.", DEVICE_DOES_NOT_EXIST), id);
}

nlohmann::json API::get_register_value(TCPClientThread* client_thread, const jsonrpcpp::Id& id, const jsonrpcpp::Parameter& params) {
    std::uint16_t dev_id = 0xffff;
    std::uint8_t reg = 0;
    try {
        dev_id = params.param_map.at("dev_id").get<std::uint16_t>();
        reg = params.param_map.at("reg").get<std::uint8_t>();
    }
    catch (std::out_of_range& e) {
        SPDLOG_ERROR("Incorrect parameters during invoke '{}' by {} - {}", __FUNCTION__, client_thread->get_client_idstring(), e.what());
        SPDLOG_DEBUG("Dump '{}' calling params: {}.", __FUNCTION__, params.to_json().dump());
        throw jsonrpcpp::InvalidParamsException(e.what(), id);
    }
    catch (nlohmann::detail::type_error& e) {
        SPDLOG_ERROR("Incorrect parameters during invoke '{}' by {} - {}", __FUNCTION__, client_thread->get_client_idstring(), e.what());
        SPDLOG_DEBUG("Dump '{}' calling params: {}.", __FUNCTION__, params.to_json().dump());
        throw jsonrpcpp::InvalidParamsException(e.what(), id);
    }

    nlohmann::json r;

    try {
        auto res = dev_mgr->get_register(dev_id, reg);
        if (std::holds_alternative<std::int64_t>(res)) {
            r = std::get<std::int64_t>(res);
        }
        else if (std::holds_alternative<std::string>(res)) {
            r = std::get<std::string>(res);
        }
        else if (std::holds_alternative<std::vector<std::uint8_t>>(res)) {
            r = std::get<std::vector<std::uint8_t>>(res);
        }
    }
    catch (DevNodeException& e) {
        throw jsonrpcpp::RequestException(jsonrpcpp::Error(e.what(), -1), id);
    }

    return std::move(r);
}

nlohmann::json API::set_register_value(TCPClientThread* client_thread, const jsonrpcpp::Id& id, const jsonrpcpp::Parameter& params) {
    std::uint16_t dev_id = 0xffff;
    std::uint8_t reg = 0;
    Device::regvalue_t val;
    try {
        dev_id = params.param_map.at("dev_id").get<std::uint16_t>();
        reg = params.param_map.at("reg").get<std::uint8_t>();
        if (params.param_map.at("value").type() == nlohmann::json::value_t::string) {
            val = params.param_map.at("value").get<std::string>();
        }
        else if (params.param_map.at("value").type() == nlohmann::json::value_t::number_integer || params.param_map.at("value").type() == nlohmann::json::value_t::number_unsigned) {
            val = params.param_map.at("value").get<std::int64_t>();
        }
        else if (params.param_map.at("value").type() == nlohmann::json::value_t::array) {
            val = params.param_map.at("value").get<std::vector<std::uint8_t>>();
        }
        else {
            SPDLOG_ERROR("Incorrect parameters during invoke '{}' by {} - Unsupported JSON ({}) as a value", __FUNCTION__, client_thread->get_client_idstring(), params.param_map.at("value").type_name());
            SPDLOG_DEBUG("Dump '{}' calling params: {}.", __FUNCTION__, params.to_json().dump());
            throw jsonrpcpp::InvalidParamsException("Unsupported JSON type as a value", id);
        }
    }
    catch (std::out_of_range& e) {
        SPDLOG_ERROR("Incorrect parameters during invoke '{}' by {} - {}", __FUNCTION__, client_thread->get_client_idstring(), e.what());
        SPDLOG_DEBUG("Dump '{}' calling params: {}.", __FUNCTION__, params.to_json().dump());
        throw jsonrpcpp::InvalidParamsException(e.what(), id);
    }
    catch (nlohmann::detail::type_error& e) {
        SPDLOG_ERROR("Incorrect parameters during invoke '{}' by {} - {}", __FUNCTION__, client_thread->get_client_idstring(), e.what());
        SPDLOG_DEBUG("Dump '{}' calling params: {}.", __FUNCTION__, params.to_json().dump());
        throw jsonrpcpp::InvalidParamsException(e.what(), id);
    }

    nlohmann::json r;

    try {
        auto res = dev_mgr->set_register(dev_id, reg, val);
        if (std::holds_alternative<std::int64_t>(res)) {
            r = std::get<std::int64_t>(res);
        }
        else if (std::holds_alternative<std::string>(res)) {
            r = std::get<std::string>(res);
        }
        else if (std::holds_alternative<std::vector<std::uint8_t>>(res)) {
            r = std::get<std::vector<std::uint8_t>>(res);
        }
    }
    catch (DevNodeException& e) {
        throw jsonrpcpp::RequestException(jsonrpcpp::Error(e.what(), -1), id);
    }

    return std::move(r);
}

API::API(Bus* bus, DevicesMgr* dev_mgr):
    bus(bus),
    dev_mgr(dev_mgr) {
    api_methods["get_version"] = &API::get_version;
    api_methods["get_methods_list"] = &API::get_methods_list;
    api_methods["subscribe"] = &API::subscribe;
    api_methods["send_frame"] = &API::send_frame;
    api_methods["get_stats"] = &API::get_stats;
    api_methods["authenticate"] = &API::authenticate;
    api_methods["get_devices_list"] = &API::get_devices_list;
    api_methods["get_device_info"] = &API::get_device_info;
    api_methods["get_registers_list"] = &API::get_registers_list;
    api_methods["get_register_value"] = &API::get_register_value;
    api_methods["set_register_value"] = &API::set_register_value;
}

jsonrpcpp::Response API::call(TCPClientThread* client_thread, const jsonrpcpp::request_ptr& request) {
    if (api_methods.count(request->method())) {
        nlohmann::json r = (this->*api_methods[request->method()])(client_thread, request->id(), request->params());

        jsonrpcpp::Response res(request->id(), r);
        return std::move(res);
    }
    else {
        throw jsonrpcpp::MethodNotFoundException(request->id());
    }
}
