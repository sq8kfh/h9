#include "servermgr.h"
#include "tcpserver.h"
#include "common/logger.h"

void ServerMgr::EventCallback::on_msg_recv() {

}

void ServerMgr::EventCallback::on_msg_send() {

}

void ServerMgr::EventCallback::on_new_connection(int socket, std::string address, std::uint16_t port) {
    _server_mgr->new_connection_callback(socket, std::move(address), port);
}

void ServerMgr::EventCallback::on_server_close() {

}

void ServerMgr::EventCallback::on_client_close() {

}

void ServerMgr::new_connection_callback(int socket, std::string address, std::uint16_t port) {
    h9_log_notice("Server: new connection from %s:%d on socket %d",
                  address.c_str(),
                  port,
                  socket);
}

ServerMgr::EventCallback ServerMgr::create_event_callback() {
    return ServerMgr::EventCallback(this);
}

ServerMgr::ServerMgr(SocketMgr *socket_mgr): _socket_mgr(socket_mgr), tcp_server(nullptr) {

}

void ServerMgr::load_config(Ctx *ctx) {
    tcp_server = new TcpServer(create_event_callback(), 7878);
    _socket_mgr->register_socket(tcp_server);

}

ServerMgr::~ServerMgr() {
    delete tcp_server;
}
