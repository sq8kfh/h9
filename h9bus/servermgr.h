/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-05-14.
 *
 * Copyright (C) 2019-2020 Kamil Palkowski. All rights reserved.
 */

#ifndef _H9_SERVERMGR_H_
#define _H9_SERVERMGR_H_

#include "config.h"
#include <map>
#include <queue>
#include <functional>
#include "socketmgr.h"
#include "busctx.h"
#include "protocol/genericmsg.h"
#include "busmgr.h"

class TcpServer;
class TcpClient;
class EventMgr;

class ServerMgr {
private:
    SocketMgr* const _socket_mgr;
    EventMgr* eventmgr_handler;

    std::queue<std::pair<int, GenericMsg>> recv_queue;

    TcpServer *tcp_server;
    std::map<int, TcpClient*> tcp_clients;
    Log msg_log;

    void recv_msg_callback(int client_socket, GenericMsg& msg);
    void new_connection_callback(int client_socket, const std::string& remote_address, std::uint16_t remote_port);
    void server_close_callback();
public:
    explicit ServerMgr(SocketMgr* socket_mgr);
    void load_config(BusCtx *ctx);
    void set_eventmgr_handler(EventMgr* handler);

    std::queue<std::pair<int, GenericMsg>>& get_recv_queue();
    void client_subscription(int client_socket, int active);
    void send_msg(int client_socket, GenericMsg& msg);
    void send_msg_to_subscriber(GenericMsg& msg);

    int connected_clients_count();

    void cron();
    void flush_clients();
    ~ServerMgr();
};


#endif //_H9_SERVERMGR_H_
