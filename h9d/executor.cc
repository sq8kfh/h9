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

int Executor::execute_object_method(int a, TCPClientThread *client) {
    devmgr->add_dev_observer(client);

    Node n(bus, 32);
    n.set_reg(10, a);

    return a;
}
