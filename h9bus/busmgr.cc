#include "busmgr.h"

#include "drivers/loop.h"
#include "drivers/dummy.h"
#include "drivers/slcan.h"
#include "drivers/socketcan.h"

BusMgr::BusMgr(SocketMgr *socket_mgr): _socket_mgr(socket_mgr) {
}

void BusMgr::load_config(Ctx *ctx) {
    Loop *loop = new Loop("can0");
    Dummy *dummy = new Dummy("can1");

    loop->open();
    _socket_mgr->register_socket(loop);
    dummy->open();
    _socket_mgr->register_socket(dummy);

    H9frame tmp;
    loop->send_frame(tmp);
    dummy->send_frame(tmp);
}
