/*
 * Created by crowx on 29/10/2023.
 *
 */

#include "dev.h"

#include <utility>
#include "dev_status_observer.h"

Dev::Dev(std::string type, std::string name, NodeDevMgr*node_mgr, std::vector<std::uint16_t> nodes):
    type(std::move(type)),
    name(std::move(name)),
    node_mgr(node_mgr),
    dependent_on_nodes(std::move(nodes)) {
}

Dev::~Dev() {
    SPDLOG_TRACE("~Dev() {}", fmt::ptr(this));
    for (auto node : dependent_on_nodes) {
        node_mgr->detach_node_state_observer(node, this);
    }
}

void Dev::emit_dev_state(const nlohmann::json& dev_status) {
    node_mgr->emit_dev_state(name, dev_status);
}

void Dev::activate() {
    for (auto node : dependent_on_nodes) {
        node_mgr->attach_node_state_observer(node, this);
    }
}

const std::vector<std::uint16_t>& Dev::get_nodes_id() {
    return dependent_on_nodes;
}

void Dev::attach_dev_status_observer(DevStatusObserver* obs) {

}

void Dev::detach_dev_status_observer(DevStatusObserver* obs) {

}

nlohmann::json Dev::dev_call(const TCPClientThread* client_thread, const jsonrpcpp::Id& id, const jsonrpcpp::Parameter& params) {
    throw jsonrpcpp::InvalidParamsException("Dev object does not provide method.", id);
}
