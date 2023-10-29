/*
 * Created by crowx on 29/10/2023.
 *
 */

#include "antenna_switch_dev.h"

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

void AntennaSwitchDev::select_antenna(int antenna_number) {
    node_mgr->set_register(switch_node_id, 11, antenna_number);
}


