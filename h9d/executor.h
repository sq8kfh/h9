/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-11-21.
 *
 * Copyright (C) 2020 Kamil Palkowski. All rights reserved.
 */

#ifndef H9_EXECUTOR_H
#define H9_EXECUTOR_H

#include "config.h"
#include <ctime>
#include <string>
#include "bus.h"
#include "dctx.h"
#include "devmgr.h"


class Executor {
private:
    DCtx * const ctx;
    Bus * const bus;
    DevMgr * const devmgr;
public:
    explicit Executor(DCtx *ctx, Bus *bus, DevMgr *devmgr) noexcept;

    void get_stat(std::string &version, std::time_t &uptime);
    int execute_object_method(int a, TCPClientThread *client);
};


#endif //H9_EXECUTOR_H
