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

    TcpServer *tcp_server;
    std::map<int, TcpClient*> tcp_clients;

    void on_new_connection(int client_socket, const std::string& remote_address, std::uint16_t remote_port);
    void server_close_callback();
public:
    explicit ServerMgr(SocketMgr* socket_mgr);
    void load_config(BusCtx *ctx);
    void set_eventmgr_handler(EventMgr* handler);

    void client_subscription(int client_socket, int active);
    void send_msg(int client_socket, GenericMsg& msg);
    void send_msg_to_subscriber(GenericMsg& msg, std::uint64_t orgin_client_id, std::uint64_t orgin_msg_id);

    int connected_clients_count();

    void cron();
    void flush_clients();
    ~ServerMgr();
};


#endif //_H9_SERVERMGR_H_
