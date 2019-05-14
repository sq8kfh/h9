#include <cstdlib>

#include "common/daemonctx.h"
#include "common/logger.h"
#include "socketmgr.h"
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

    h9_log_notice("start");
    socketmgr.select_loop();

    return EXIT_SUCCESS;
}
