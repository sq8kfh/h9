/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-05-14.
 *
 * Copyright (C) 2019-2020 Kamil Palkowski. All rights reserved.
 */

#include "servermgr.h"

#include <iostream>
#include "tcpserver.h"
#include "common/logger.h"
#include "tcpclient.h"
#include "socketmgr.h"
#include "eventmgr.h"


void ServerMgr::recv_msg_callback(int client_socket, GenericMsg& msg) {
    msg_log.log(msg.serialize());
    recv_queue.push(std::make_pair(client_socket, std::move(msg)));
}

void ServerMgr::new_connection_callback(int client_socket, const std::string& remote_address, std::uint16_t remote_port) {
    h9_log_notice("Server: new connection from %s:%d on socket %d",
                  remote_address.c_str(),
                  remote_port,
                  client_socket);

    TcpClient::TNewMsgCallback tmp_f = std::bind(&EventMgr::process_msg, eventmgr_handler, std::placeholders::_1, std::placeholders::_2);
    TcpClient *tmp = new TcpClient(tmp_f, client_socket);
    tcp_clients[client_socket] = tmp;
    _socket_mgr->register_socket(tmp);
}

ServerMgr::ServerMgr(SocketMgr* socket_mgr):
        _socket_mgr(socket_mgr),
        tcp_server(nullptr),
        eventmgr_handler(nullptr) {
}

void ServerMgr::load_config(BusCtx *ctx) {
    TcpServer::TNewConnectionCallback tmp_f = std::bind(&ServerMgr::new_connection_callback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    tcp_server = new TcpServer(tmp_f, ctx->cfg_server_port());
    _socket_mgr->register_socket(tcp_server);

    msg_log = ctx->logger();
}

void ServerMgr::set_eventmgr_handler(EventMgr* handler) {
    eventmgr_handler = handler;
}

std::queue<std::pair<int, GenericMsg>>& ServerMgr::get_recv_queue() {
    return recv_queue;
}

void ServerMgr::client_subscription(int client_socket, int active) {
    tcp_clients[client_socket]->subscriber(active);
}

void ServerMgr::send_msg(int client_socket, GenericMsg& msg) {
    if (tcp_clients.count(client_socket)) {
        try {
            tcp_clients[client_socket]->send(msg);
        }
        catch (SocketMgr::Socket::CloseSocketException& e) {
            tcp_clients[client_socket]->on_close();
        }
    }
}

void ServerMgr::send_msg_to_subscriber(GenericMsg& msg) {
    for (auto& it: tcp_clients) {
        if (it.second->is_subscriber()) {
            try {
                it.second->send(msg);
            }
            catch (SocketMgr::Socket::CloseSocketException& e) {
                h9_log_info("Close connection during send_msg_to_subscriber (socket: %d)", it.first);
                it.second->on_close();
            }
        }
    }
}

void ServerMgr::cron() {

}

void ServerMgr::flush_clients() {
    for (auto it = tcp_clients.cbegin(); it != tcp_clients.cend();) {
        if (!it->second->is_connected()) {
            TcpClient *tmp = it->second;
            h9_log_notice("Server: flush connection from %s:%s on socket %d",
                          tmp->get_remote_address().c_str(),
                          tmp->get_remote_port().c_str(),
                          tmp->get_socket());

            _socket_mgr->unregister_socket(tmp);
            it = tcp_clients.erase(it);
            delete tmp;
        }
        else {
            ++it;
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

int ServerMgr::connected_clients_count() {
    return tcp_clients.size();
}
