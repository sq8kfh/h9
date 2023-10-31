/*
 * Created by crowx on 29/10/2023.
 *
 */

#pragma once

#include "dev.h"

class AntennaSwitchDev: public Dev {
  private:
    const std::uint16_t switch_node_id;
    const std::uint16_t controller_node_id;
  public:
    AntennaSwitchDev(NodeDevMgr*node_mgr, std::uint16_t switch_node_id);
    AntennaSwitchDev(NodeDevMgr*node_mgr, std::uint16_t switch_node_id, std::uint16_t controller_node_id);
    void update_dev_state(std::uint16_t node_id, const ExtH9Frame& frame) override;
    void init() override;
    nlohmann::json dev_call(const TCPClientThread* client_thread, const jsonrpcpp::Id& id, const jsonrpcpp::Parameter& params) override;

    void select_antenna(int antenna_number);
};
