/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-04-09.
 *
 * Copyright (C) 2019 Kamil Palkowski. All rights reserved.
 */

#include "config.h"
#include <cstdlib>
#include <thread>
#include <unistd.h>
#include "common/logger.h"
#include "bus.h"
#include "dctx.h"
#include "devmgr.h"
#include "executor.h"
#include "node.h"
#include "tcpserver.h"


int main(int argc, char **argv) {
    DCtx ctx;
    ctx.load(argc, argv);

    if (ctx.cfg_daemonize()) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated"
        if (daemon(1, 0) != 0) {
#pragma GCC diagnostic pop
            h9_log_crit("Unable to daemonize: %s", strerror(errno));
            return EXIT_FAILURE;
        }
    }

    if (!ctx.cfg_pidfile().empty()) {
        std::ofstream pid_file;
        pid_file.open(ctx.cfg_pidfile(), std::ofstream::out | std::ofstream::trunc);
        pid_file << getpid();
        pid_file.close();
    }

    Bus bus = {};
    bus.load_config(&ctx);

    DevMgr devmgr(&bus);
    devmgr.load_config(&ctx);

    Executor executor(&ctx, &bus, &devmgr);

    TCPServer server(&executor);
    server.load_config(&ctx);

    Node n = {&bus, 2};

    //n.reset();

    int nt = n.get_node_type();
    h9_log_info("Node type: %d", nt);

    uint8_t major;
    uint8_t minor;
    nt = n.get_node_version(&major, &minor);
    h9_log_info("Node version: %hhu.%hhu %d", major, minor, nt);


    //std::thread t = std::thread(bus);
    //int ret = bus.set_reg(H9frame::Priority::LOW, 2, 2, 12, 0, nullptr);
    //h9_log_info("ret %d", ret);
    //t.join();


    if (ctx.cfg_drop_privileges() && getuid() == 0) {
        if (setgid(ctx.cfg_drop_privileges_gid()) != 0) {
            h9_log_crit("Unable to drop group privileges: %s", strerror(errno));
            return EXIT_FAILURE;
        }
        if (setuid(ctx.cfg_drop_privileges_uid()) != 0) {
            h9_log_crit("Unable to drop user privileges: %s", strerror(errno));
            return EXIT_FAILURE;
        }
    }

    devmgr.discover();
    server.run();

    return EXIT_SUCCESS;
}
