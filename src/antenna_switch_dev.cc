/*
 * Created by crowx on 29/10/2023.
 *
 */

#include "antenna_switch_dev.h"

#include "dev_node_exception.h"

AntennaSwitchDev::AntennaSwitchDev(NodeDevMgr*node_mgr, std::uint16_t switch_node_id):
    Dev("AntennaSwitchDev", node_mgr, {switch_node_id}),
    switch_node_id(switch_node_id),
    controller_node_id(0xffff) {
}

AntennaSwitchDev::AntennaSwitchDev(NodeDevMgr*node_mgr, std::uint16_t switch_node_id, std::uint16_t controller_node_id):
    Dev("AntennaSwitchDev", node_mgr, {switch_node_id, controller_node_id}),
    switch_node_id(switch_node_id),
    controller_node_id(controller_node_id) {
}

void AntennaSwitchDev::update_dev_state(std::uint16_t node_id, const ExtH9Frame& frame) {
    SPDLOG_INFO("update_dev_state");
    emit_dev_state({{"selected_antenna", frame.data()[1]}});
}

void AntennaSwitchDev::init() {

}

nlohmann::json AntennaSwitchDev::dev_call(const TCPClientThread* client_thread, const jsonrpcpp::Id& id, const jsonrpcpp::Parameter& params) {
    std::string method = params.param_map.at("method").get<std::string>();
    int antenna_number = params.param_map.at("antenna_number").get<int>();

    if(method == std::string("select_antenna")) {
        select_antenna(antenna_number);
    }
    else {
        throw jsonrpcpp::InvalidParamsException("Dev object does not provide '" + method + "' method.", id);
    }

    return {};
}

void AntennaSwitchDev::select_antenna(int antenna_number) {
    try {
        node_mgr->set_register(switch_node_id, 10, antenna_number);
    }
    catch (DevNodeException& e) {
        SPDLOG_ERROR(e.what());
    }
}
