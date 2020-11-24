/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-11-21.
 *
 * Copyright (C) 2020 Kamil Palkowski. All rights reserved.
 */

#include "executor.h"
#include "node.h"


Executor::Executor(DCtx *ctx, Bus *bus, DevMgr *devmgr) noexcept: ctx(ctx), bus(bus), devmgr(devmgr) {

}

void Executor::get_stat(std::string &version, std::time_t &uptime) {
    version = std::string(H9_VERSION);
    uptime = std::time(nullptr) - ctx->get_start_time();
}

int Executor::active_devices_count() {
    return devmgr->active_devices_count();
}

int Executor::attach_device_event_observer(TCPClientThread *client, std::uint16_t dev_id, std::string event_name) {
    devmgr->attach_event_observer(client, event_name, dev_id);
}

int Executor::detach_device_event_observer(TCPClientThread *client, std::uint16_t dev_id, std::string event_name) {
    devmgr->detach_event_observer(client, event_name, dev_id);
}

int Executor::execute_device_method(TCPClientThread *client, std::uint16_t device_id, std::string method_name) {

}

int Executor::execute_object_method(int a, TCPClientThread *client) {
    devmgr->attach_event_observer(client, "antenna-switch", 32);

    Node n(bus, 32);
    n.set_reg(10, a);

    return a;
}
