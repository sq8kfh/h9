/*
 * H9 project
 *
 * Created by crowx on 2023-09-19.
 *
 */

#include "node_mgr.h"

void NodeMgr::on_frame_recv(const ExtH9Frame& frame) {
    notify_frame_observer(frame);
}

NodeMgr::NodeMgr(Bus* bus):
    frame_obs(bus, this),
    bus(bus) {
}

Node NodeMgr::node(std::uint16_t node_id) {
    return Node(this, bus, node_id);
}

void NodeMgr::response_timeout_duration(int response_timeout_duration) {
    _response_timeout_duration = response_timeout_duration;
}

int NodeMgr::response_timeout_duration() {
    return _response_timeout_duration;
}
