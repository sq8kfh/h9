#include <functional>
#include <cstdlib>

#include "common/daemonctx.h"
#include "common/logger.h"
#include "socketmgr.h"
#include "eventmgr.h"
#include "busmgr.h"
#include "servermgr.h"

int main(int argc, char **argv) {
    DaemonCtx ctx("h9bus", "H9 Bus daemon.");
    ctx.load_configuration(argc, argv);

    SocketMgr socketmgr;

    BusMgr busmgr = BusMgr(&socketmgr);
    busmgr.load_config(&ctx);

    ServerMgr servermgr = ServerMgr(&socketmgr);
    servermgr.load_config(&ctx);

    EventMgr event_mgr = {&ctx, &busmgr, &servermgr};
    //busmgr.set_frame_recv_callback(std::bind(&EventMgr::on_fame_recv, &event_mgr, std::placeholders::_1, std::placeholders::_2));
    //servermgr.set_msg_recv_callback(std::bind(&EventMgr::on_msg_recv, &event_mgr, std::placeholders::_1, std::placeholders::_2));

    socketmgr.select_loop(std::bind(&EventMgr::flush_all, &event_mgr));
    return EXIT_FAILURE;
}
