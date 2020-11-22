/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-11-19.
 *
 * Copyright (C) 2020 Kamil Palkowski. All rights reserved.
 */

#include "devmgr.h"
#include "bus.h"
#include "common/logger.h"
#include "tcpclientthread.h"


void DevMgr::on_frame_recv(H9frame frame) {
    if (frame.type == H9frame::Type::NODE_INFO) {
        h9_log_notice("Dev discovered id: %d, type: %d, version: %d.%d", frame.source_id, frame.data[0] << 8 | frame.data[1], frame.data[2], frame.data[3]);
    }
    else if (frame.type == H9frame::Type::NODE_TURNED_ON) {
        h9_log_notice("Dev turned on id: %d, type: %d, version: %d.%d", frame.source_id, frame.data[0] << 8 | frame.data[1], frame.data[2], frame.data[3]);
    }
    else if (frame.type == H9frame::Type::REG_EXTERNALLY_CHANGED && frame.source_id == 32 && frame.data[0] == 10) {
        if (_client) {
            ResponseMsg res("dev");
            res.add_value("object", "antenna-switch");
            res.add_value("id", 8);
            res.add_value("method", "switch");
            res.add_value("response", frame.data[1]);
            _client->send_msg(std::move(res));
        }
    }
}

DevMgr::DevMgr(Bus *bus): BusObserver(bus, H9FrameComparator()) {
    _client = nullptr;
}

DevMgr::~DevMgr() {

}

void DevMgr::load_config(DCtx *ctx) {

}

void DevMgr::discover() {
    auto seqnum = h9bus->get_next_seqnum(1);
    h9bus->send_node_discover(H9frame::Priority::LOW, seqnum, 1);
}

void DevMgr::add_dev_observer(TCPClientThread *client) {
    _client = client;
}
