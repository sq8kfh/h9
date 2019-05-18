#include "servermgr.h"

#include <iostream>
#include "tcpserver.h"
#include "common/logger.h"
#include "tcpclient.h"
#include "socketmgr.h"


void ServerMgr::EventCallback::on_msg_recv(int client_socket, GenericMsg& msg) {
    _server_mgr->recv_msg_callback(client_socket, msg);
}

void ServerMgr::EventCallback::on_msg_send() {

}

void ServerMgr::EventCallback::on_new_connection(int client_socket, const std::string& remote_address, std::uint16_t remote_port) {
    _server_mgr->new_connection_callback(client_socket, remote_address, remote_port);
}

void ServerMgr::EventCallback::on_server_close() {
    _server_mgr->server_close_callback();
}

void ServerMgr::EventCallback::on_client_close(int client_socket) {
    _server_mgr->client_close_callback(client_socket);
}


void ServerMgr::recv_msg_callback(int client_socket, GenericMsg& msg) {
    msg_log.log(msg.serialize());
    recv_queue.push(std::make_pair(client_socket, std::move(msg)));
}

void ServerMgr::new_connection_callback(int client_socket, const std::string& remote_address, std::uint16_t remote_port) {
    h9_log_notice("Server: new connection from %s:%d on socket %d",
                  remote_address.c_str(),
                  remote_port,
                  client_socket);

    TcpClient *tmp = new TcpClient(create_event_callback(), client_socket, remote_address, remote_port);
    tcp_clients[client_socket] = tmp;
    _socket_mgr->register_socket(tmp);
}

void ServerMgr::client_close_callback(int client_socket) {
    TcpClient *tmp = tcp_clients[client_socket];
    h9_log_notice("Server: close connection from %s:%d on socket %d",
                  tmp->remote_address.c_str(),
                  tmp->remote_port,
                  client_socket);

    _socket_mgr->unregister_socket(tmp);
    tcp_clients.erase(client_socket);
    delete tmp;
}

void ServerMgr::server_close_callback() {
    //TODO: handle a server close
}

ServerMgr::EventCallback ServerMgr::create_event_callback() {
    return ServerMgr::EventCallback(this);
}

ServerMgr::ServerMgr(SocketMgr* socket_mgr):
        _socket_mgr(socket_mgr),
        tcp_server(nullptr) {
}

void ServerMgr::load_config(Ctx *ctx) {
    tcp_server = new TcpServer(create_event_callback(), 7878);
    _socket_mgr->register_socket(tcp_server);

    msg_log = ctx->log("h9msg");
}

std::queue<std::pair<int, GenericMsg>>& ServerMgr::get_recv_queue() {
    return recv_queue;
}

void ServerMgr::send_msg(int client_socket, GenericMsg& msg) {
    if (tcp_clients.count(client_socket)) {
        try {
            tcp_clients[client_socket]->send(msg);
        }
        catch (SocketMgr::Socket::CloseSocketException& e) {
            tcp_clients[client_socket]->close();
        }
    }
}

void ServerMgr::send_msg_to_subscriber(GenericMsg& msg) {
    for (auto it = tcp_clients.begin(); it != tcp_clients.end();) {
        auto it_local = it;
        ++it;
        if (it_local->second->is_subscriber()) {
            try {
                it_local->second->send(msg);
            }
            catch (SocketMgr::Socket::CloseSocketException& e) {
                it_local->second->close();
            }
        }
    }
}

ServerMgr::~ServerMgr() {
    delete tcp_server;
    for (auto& it: tcp_clients) {
        delete it.second;
    }
    tcp_clients.clear();
}


