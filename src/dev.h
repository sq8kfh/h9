/*
 * Created by crowx on 29/10/2023.
 *
 */

#pragma once

#include "node.h"
#include "node_dev_mgr.h"
#include <jsonrpcpp/jsonrpcpp.hpp>

class DevStatusObserver;

class Dev: public Node::NodeStateObserver {
  private:
    std::vector<std::uint16_t> dependent_from_nodes;
    std::vector<DevStatusObserver*> dev_status_observer;
    const std::string _type;
  protected:
    NodeDevMgr*node_mgr;
    Dev(std::string type, NodeDevMgr*node_mgr, std::vector<std::uint16_t> nodes);
    virtual ~Dev();
    void emit_dev_state(const nlohmann::json& dev_status);
  public:
    std::string type();
    void attach_dev_status_observer(DevStatusObserver* obs);
    void detach_dev_status_observer(DevStatusObserver* obs);
    void update_dev_state(std::uint16_t node_id, const ExtH9Frame& frame) override = 0;
    virtual void init() = 0;
    virtual nlohmann::json dev_call(const TCPClientThread* client_thread, const jsonrpcpp::Id& id, const jsonrpcpp::Parameter& params);
};
