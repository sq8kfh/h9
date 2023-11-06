/*
 * Created by crowx on 29/10/2023.
 *
 */

#pragma once

#include <jsonrpcpp/jsonrpcpp.hpp>

#include "node.h"
#include "node_dev_mgr.h"

class DevStatusObserver;

class Dev {
  private:
    std::vector<std::uint16_t> dependent_on_nodes;
  protected:
    NodeDevMgr* node_mgr;
    Dev(std::string type, std::string name, NodeDevMgr*node_mgr, std::vector<std::uint16_t> nodes);
    virtual ~Dev();
    void emit_dev_state(const nlohmann::json& dev_status);
  public:
    const std::string type;
    const std::string name;

    void activate();
    const std::vector<std::uint16_t>& get_nodes_id();

    void attach_dev_status_observer(DevStatusObserver* obs);
    void detach_dev_status_observer(DevStatusObserver* obs);
    virtual void init() = 0;
    virtual void update_dev_state(std::uint16_t node_id, const ExtH9Frame& frame) = 0;
    virtual nlohmann::json dev_call(const TCPClientThread* client_thread, const jsonrpcpp::Id& id, const jsonrpcpp::Parameter& params);
};
