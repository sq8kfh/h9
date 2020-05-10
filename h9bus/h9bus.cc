/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-04-09.
 *
 * Copyright (C) 2019-2020 Kamil Palkowski. All rights reserved.
 */

#include "config.h"
#include <functional>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include "common/logger.h"
#include "busctx.h"
#include "socketmgr.h"
#include "eventmgr.h"
#include "busmgr.h"
#include "servermgr.h"

int main(int argc, char **argv) {
    BusCtx ctx;
    ctx.load(argc, argv);

    if (ctx.cfg_daemonize()) {
        if (daemon(1, 0) != 0) {
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

    SocketMgr socketmgr;

    BusMgr busmgr = BusMgr(&socketmgr);
    busmgr.load_config(&ctx);

    ServerMgr servermgr = ServerMgr(&socketmgr);
    servermgr.load_config(&ctx);

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

    EventMgr event_mgr = {&ctx, &busmgr, &servermgr};
    //busmgr.set_frame_recv_callback(std::bind(&EventMgr::on_fame_recv, &event_mgr, std::placeholders::_1, std::placeholders::_2));
    //servermgr.set_msg_recv_callback(std::bind(&EventMgr::on_msg_recv, &event_mgr, std::placeholders::_1, std::placeholders::_2));
//std::bind(&EventMgr::flush_all, &event_mgr);
    socketmgr.select_loop([&event_mgr, &busmgr, &servermgr]() {
        busmgr.flush_dev();
        servermgr.flush_clients();
        }, std::bind(&EventMgr::cron, &event_mgr));
    return EXIT_FAILURE;
}
