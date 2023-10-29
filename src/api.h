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
#include "node_dev_mgr.h"
#include "tcpclientthread.h"

class TCPClientThread;

class API {
  public:
    constexpr static int NODE_DOES_NOT_EXIST = -1;
  private:
    using api_method = nlohmann::json (API::*)(TCPClientThread* client_thread, const jsonrpcpp::Id&, const jsonrpcpp::Parameter&);

    Bus* const bus;
    NodeDevMgr* const node_dev_mgr;

    std::map<std::string, api_method> api_methods;

    nlohmann::json get_version(TCPClientThread* client_thread, const jsonrpcpp::Id& id, const jsonrpcpp::Parameter& params);
    nlohmann::json get_methods_list(TCPClientThread* client_thread, const jsonrpcpp::Id& id, const jsonrpcpp::Parameter& params);
    nlohmann::json subscribe(TCPClientThread* client_thread, const jsonrpcpp::Id& id, const jsonrpcpp::Parameter& params);
    nlohmann::json send_frame(TCPClientThread* client_thread, const jsonrpcpp::Id& id, const jsonrpcpp::Parameter& params);
    nlohmann::json get_stats(TCPClientThread* client_thread, const jsonrpcpp::Id& id, const jsonrpcpp::Parameter& params);
    nlohmann::json authenticate(TCPClientThread* client_thread, const jsonrpcpp::Id& id, const jsonrpcpp::Parameter& params);
    nlohmann::json get_nodes_list(TCPClientThread* client_thread, const jsonrpcpp::Id& id, const jsonrpcpp::Parameter& params);
    nlohmann::json get_node_info(TCPClientThread* client_thread, const jsonrpcpp::Id& id, const jsonrpcpp::Parameter& params);
    nlohmann::json discover_nodes(TCPClientThread* client_thread, const jsonrpcpp::Id& id, const jsonrpcpp::Parameter& params);
    nlohmann::json node_reset(TCPClientThread* client_thread, const jsonrpcpp::Id& id, const jsonrpcpp::Parameter& params);
    nlohmann::json get_registers_list(TCPClientThread* client_thread, const jsonrpcpp::Id& id, const jsonrpcpp::Parameter& params);
    nlohmann::json get_register_value(TCPClientThread* client_thread, const jsonrpcpp::Id& id, const jsonrpcpp::Parameter& params);
    nlohmann::json set_register_value(TCPClientThread* client_thread, const jsonrpcpp::Id& id, const jsonrpcpp::Parameter& params);
    nlohmann::json set_register_bit(TCPClientThread* client_thread, const jsonrpcpp::Id& id, const jsonrpcpp::Parameter& params);
    nlohmann::json clear_register_bit(TCPClientThread* client_thread, const jsonrpcpp::Id& id, const jsonrpcpp::Parameter& params);
    nlohmann::json toggle_register_bit(TCPClientThread* client_thread, const jsonrpcpp::Id& id, const jsonrpcpp::Parameter& params);
    nlohmann::json get_devs_list(TCPClientThread* client_thread, const jsonrpcpp::Id& id, const jsonrpcpp::Parameter& params);
  public:
    API(Bus* bus, NodeDevMgr* dev_mgr);
    jsonrpcpp::Response call(TCPClientThread* client_thread, const jsonrpcpp::request_ptr& request);
};
