/*
 * H9 project
 *
 * Created by crowx on 2023-09-11.
 *
 * Copyright (C) 2023 Kamil Palkowski. All rights reserved.
 */

#pragma once

#include <jsonrpcpp/jsonrpcpp.hpp>
#include <map>
#include <nlohmann/json.hpp>

#include "bus.h"
#include "node_mgr.h"
#include "tcpclientthread.h"

class TCPClientThread;

class API {
  private:
    using api_method = nlohmann::json (API::*)(TCPClientThread* client_thread, const jsonrpcpp::Id&, const jsonrpcpp::Parameter&);

    Bus* const bus;
    NodeMgr* const node_mgr;

    std::map<std::string, api_method> api_methods;

    nlohmann::json get_version(TCPClientThread* client_thread, const jsonrpcpp::Id& id, const jsonrpcpp::Parameter& params);
    nlohmann::json get_methods_list(TCPClientThread* client_thread, const jsonrpcpp::Id& id, const jsonrpcpp::Parameter& params);
    nlohmann::json subscribe(TCPClientThread* client_thread, const jsonrpcpp::Id& id, const jsonrpcpp::Parameter& params);
    nlohmann::json send_frame(TCPClientThread* client_thread, const jsonrpcpp::Id& id, const jsonrpcpp::Parameter& params);
    nlohmann::json get_stats(TCPClientThread* client_thread, const jsonrpcpp::Id& id, const jsonrpcpp::Parameter& params);
    nlohmann::json authenticate(TCPClientThread* client_thread, const jsonrpcpp::Id& id, const jsonrpcpp::Parameter& params);

  public:
    API(Bus* bus, NodeMgr* node_mgr);
    jsonrpcpp::Response call(TCPClientThread* client_thread, const jsonrpcpp::request_ptr& request);
};
