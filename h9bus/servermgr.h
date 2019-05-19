#ifndef _H9_SERVERMGR_H_
#define _H9_SERVERMGR_H_

#include "config.h"
#include <map>
#include <queue>
#include <functional>
#include "socketmgr.h"
#include "common/ctx.h"
#include "protocol/genericmsg.h"
#include "busmgr.h"


class TcpServer;
class TcpClient;

class ServerMgr {
public:
    class EventCallback {
    private:
        ServerMgr *const _server_mgr;
    public:
        explicit EventCallback(ServerMgr *const server_mgr): _server_mgr(server_mgr) {};
        void on_msg_recv(int client_socket, GenericMsg& msg);
        void on_msg_send();
        void on_new_connection(int client_socket, const std::string& remote_address, std::uint16_t remote_port);
        void on_server_close();
        void on_client_close(int client_socket);
    };
private:
    SocketMgr* const _socket_mgr;
    std::queue<std::pair<int, GenericMsg>> recv_queue;

    TcpServer *tcp_server;
    std::map<int, TcpClient*> tcp_clients;
    Log msg_log;

    void recv_msg_callback(int client_socket, GenericMsg& msg);
    void new_connection_callback(int client_socket, const std::string& remote_address, std::uint16_t remote_port);
    void client_close_callback(int client_socket);
    void server_close_callback();
    EventCallback create_event_callback();
public:
    explicit ServerMgr(SocketMgr* socket_mgr);
    void load_config(Ctx *ctx);
    std::queue<std::pair<int, GenericMsg>>& get_recv_queue();
    TcpClient* get_cient(int client_socket);
    void send_msg(int client_socket, GenericMsg& msg);
    void send_msg_to_subscriber(GenericMsg& msg);
    ~ServerMgr();
};


#endif //_H9_SERVERMGR_H_
