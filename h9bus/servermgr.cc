#include "servermgr.h"

ServerMgr::ServerMgr(SocketMgr *socket_mgr): _socket_mgr(socket_mgr), tcp_server(nullptr) {

}

void ServerMgr::load_config(Ctx *ctx) {
    tcp_server = new TcpServer(7878);
    _socket_mgr->register_socket(tcp_server);

}

ServerMgr::~ServerMgr() {
    delete tcp_server;
}
