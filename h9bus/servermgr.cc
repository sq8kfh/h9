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


void ServerMgr::on_new_connection(int client_socket, const std::string& remote_address, std::uint16_t remote_port) {
    //TODO:    msg_log.log(msg.serialize());
    TcpClient::TNewMsgCallback tmp_f = std::bind(&EventMgr::process_msg, eventmgr_handler, std::placeholders::_1, std::placeholders::_2);
    TcpClient *tmp = new TcpClient(tmp_f, client_socket);
    h9_log_notice("Server: new connection from %s:%d client: %p",
                  remote_address.c_str(),
                  remote_port,
                  tmp);
    tcp_clients[client_socket] = tmp;
    _socket_mgr->register_socket(tmp);
}

ServerMgr::ServerMgr(SocketMgr* socket_mgr):
        _socket_mgr(socket_mgr),
        tcp_server(nullptr),
        eventmgr_handler(nullptr) {
}

void ServerMgr::load_config(BusCtx *ctx) {
    TcpServer::TNewConnectionCallback tmp_f = std::bind(&ServerMgr::on_new_connection, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    tcp_server = new TcpServer(tmp_f, ctx->cfg_server_port());
    _socket_mgr->register_socket(tcp_server);
}

void ServerMgr::set_eventmgr_handler(EventMgr* handler) {
    eventmgr_handler = handler;
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

void ServerMgr::send_msg_to_subscriber(GenericMsg& msg, std::uint64_t orgin_client_id, std::uint64_t orgin_msg_id) {
    for (auto& it: tcp_clients) {
        if (it.first == orgin_client_id) {
            try {
                auto tmp_msg = msg;
                tmp_msg.set_request_id(orgin_msg_id);
                it.second->send(tmp_msg);
            }
            catch (SocketMgr::Socket::CloseSocketException& e) {
                h9_log_info("Close connection during send_msg_to_subscriber (client: %p)", it.second);
                it.second->on_close();
            }
        }
        else if (it.second->is_subscriber()) {
            try {
                it.second->send(msg);
            }
            catch (SocketMgr::Socket::CloseSocketException& e) {
                h9_log_info("Close connection during send_msg_to_subscriber (client: %p)", it.second);
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
            h9_log_notice("Server: flush connection from %s:%s on client: %p",
                          tmp->get_remote_address().c_str(),
                          tmp->get_remote_port().c_str(),
                          tmp);

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
