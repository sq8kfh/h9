#include "busmgr.h"

#include "drivers/loop.h"

BusMgr::BusMgr(SocketMgr *socket_mgr): _socket_mgr(socket_mgr) {
}

void BusMgr::load_config(Ctx *ctx) {
    Loop *loop = new Loop();

    loop->open();
    _socket_mgr->register_socket(loop);

    H9frame tmp;
    loop->send(tmp);
}
