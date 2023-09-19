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
#include "git_version.h"

nlohmann::json API::get_version(TCPClientThread* client_thread, const jsonrpcpp::Id& id, const jsonrpcpp::Parameter& params) {
    nlohmann::json r = {
        {"version", H9_VERSION},
#ifdef GITVERSION_COMMIT_SHA
        {"commit_sha", GITVERSION_COMMIT_SHA},
#ifdef GITVERSION_DIRTY
        {"dirty", true},
#endif
#endif
    };

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
    catch (std::out_of_range e) {
        SPDLOG_ERROR("Incorrect parameters during invoke '{}' by {} - {}", __FUNCTION__, client_thread->get_client_idstring(), e.what());
        SPDLOG_DEBUG("Dump '{}' calling params: {}.", __FUNCTION__, params.to_json().dump());
        throw jsonrpcpp::InvalidParamsException(e.what(), id);
    }
    catch (nlohmann::detail::type_error e) {
        SPDLOG_ERROR("Incorrect parameters during invoke '{}' by {} - {}", __FUNCTION__, client_thread->get_client_idstring(), e.what());
        SPDLOG_DEBUG("Dump '{}' calling params: {}.", __FUNCTION__, params.to_json().dump());
        throw jsonrpcpp::InvalidParamsException(e.what(), id);
    }
}

nlohmann::json API::send_frame(TCPClientThread* client_thread, const jsonrpcpp::Id& id, const jsonrpcpp::Parameter& params) {
    try {
        ExtH9Frame frame = params.param_map.at("frame").get<ExtH9Frame>();

        frame.origin(client_thread->get_client_idstring());
        int ret = bus->send_frame(std::move(frame));

        nlohmann::json r = {{"return", ret}};

        SPDLOG_DEBUG("send_frame call");
        return std::move(r);
    }
    catch (std::out_of_range e) {
        SPDLOG_ERROR("Incorrect parameters during invoke '{}' by {} - {}", __FUNCTION__, client_thread->get_client_idstring(), e.what());
        SPDLOG_DEBUG("Dump '{}' calling params: {}.", __FUNCTION__, params.to_json().dump());
        throw jsonrpcpp::InvalidParamsException(e.what(), id);
    }
    catch (nlohmann::detail::type_error e) {
        SPDLOG_ERROR("Incorrect parameters during invoke '{}' by {} - {}", __FUNCTION__, client_thread->get_client_idstring(), e.what());
        SPDLOG_DEBUG("Dump '{}' calling params: {}.", __FUNCTION__, params.to_json().dump());
        throw jsonrpcpp::InvalidParamsException(e.what(), id);
    }
}

nlohmann::json API::get_stats(TCPClientThread* client_thread, const jsonrpcpp::Id& id, const jsonrpcpp::Parameter& params) {
    return std::move(MetricsCollector::metrics_to_json());
}

API::API(Bus* bus): bus(bus) {
    api_methods["get_version"] = &API::get_version;
    api_methods["get_methods_list"] = &API::get_methods_list;
    api_methods["subscribe"] = &API::subscribe;
    api_methods["send_frame"] = &API::send_frame;
    api_methods["get_stats"] = &API::get_stats;
}

jsonrpcpp::Response API::call(TCPClientThread* client_thread, jsonrpcpp::request_ptr request) {
    if (api_methods.count(request->method())) {
        nlohmann::json r = (this->*api_methods[request->method()])(client_thread, request->id(), request->params());

        jsonrpcpp::Response res(std::move(request->id()), std::move(r));
        return std::move(res);
    }
    else {
        throw jsonrpcpp::MethodNotFoundException(std::move(request->id()));
    }
}
