#ifndef _H9_TCPCLIENT_H_
#define _H9_TCPCLIENT_H_


#include "socketmgr.h"
#include "servermgr.h"

class TcpClient: public SocketMgr::Socket {
private:
    ServerMgr::EventCallback _event_callback;
    size_t data_to_read;
    std::string recv_data_buf;

    void recv();
    void recv_header();
    void recv_data();
    void recv_msg(const std::string& msg_str);
public:
    const std::string remote_address;
    const std::uint16_t remote_port;
    TcpClient(ServerMgr::EventCallback event_callback, int sockfd, std::string remote_address, std::uint16_t remote_port);
    ~TcpClient();
    void on_select();
    void close();
};


#endif //_H9_TCPCLIENT_H_
