/*
 * Created by crowx on 29/10/2023.
 *
 */

#include "dev.h"

#include <utility>
#include "dev_status_observer.h"

Dev::Dev(std::string type, NodeDevMgr*node_mgr, std::vector<std::uint16_t> nodes):
    _type(std::move(type)),
    node_mgr(node_mgr),
    dependent_from_nodes(std::move(nodes)) {

    for (auto node : dependent_from_nodes) {
        node_mgr->attach_node_state_observer(node, this);
    }
}

Dev::~Dev() {
    SPDLOG_TRACE("~Dev() {}", fmt::ptr(this));
    for (auto node : dependent_from_nodes) {
        node_mgr->detach_node_state_observer(node, this);
    }
}

void Dev::emit_dev_state(const nlohmann::json& dev_status) {
    for (auto obs : dev_status_observer) {
        obs->on_dev_state_update(dev_status);
    }
}

std::string Dev::type() {
    return _type;
}

void Dev::attach_dev_status_observer(DevStatusObserver* obs) {
    dev_status_observer.push_back(obs);
}

void Dev::detach_dev_status_observer(DevStatusObserver* obs) {
    dev_status_observer.erase(std::remove(dev_status_observer.begin(), dev_status_observer.end(), obs), dev_status_observer.end());
}
