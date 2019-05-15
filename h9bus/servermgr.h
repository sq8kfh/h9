#ifndef _H9_SERVERMGR_H_
#define _H9_SERVERMGR_H_

#include <map>
#include "socketmgr.h"
#include "common/ctx.h"

class TcpServer;
class TcpClient;

class ServerMgr {
public:
    class EventCallback {
    private:
        ServerMgr *const _server_mgr;
    public:
        explicit EventCallback(ServerMgr *const server_mgr): _server_mgr(server_mgr) {};
        void on_msg_recv();
        void on_msg_send();
        void on_new_connection(int client_socket, const std::string& remote_address, std::uint16_t remote_port);
        void on_server_close();
        void on_client_close(int client_socket);
    };
private:
    SocketMgr * const _socket_mgr;
    TcpServer *tcp_server;
    std::map<int, TcpClient*> tcp_clients;

    void new_connection_callback(int client_socket, const std::string& remote_address, std::uint16_t remote_port);
    void client_close_callback(int client_socket);
    void server_close_callback();
    EventCallback create_event_callback();
public:
    explicit ServerMgr(SocketMgr *socket_mgr);
    void load_config(Ctx *ctx);
    ~ServerMgr();
};


#endif //_H9_SERVERMGR_H_
