#ifndef _H9_SERVERMGR_H_
#define _H9_SERVERMGR_H_


#include "socketmgr.h"
#include "common/ctx.h"

class TcpServer;

class ServerMgr {
public:
    class EventCallback {
    private:
        ServerMgr *const _server_mgr;
    public:
        EventCallback(ServerMgr *const server_mgr): _server_mgr(server_mgr) {};
        void on_msg_recv();
        void on_msg_send();
        void on_new_connection(int socket, std::string address, std::uint16_t port);
        void on_server_close();
        void on_client_close();
    };
private:
    SocketMgr * const _socket_mgr;
    TcpServer *tcp_server;

    void new_connection_callback(int socket, std::string address, std::uint16_t port);
    EventCallback create_event_callback();
public:
    ServerMgr(SocketMgr *socket_mgr);
    void load_config(Ctx *ctx);
    ~ServerMgr();
};


#endif //_H9_SERVERMGR_H_
