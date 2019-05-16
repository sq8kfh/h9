#ifndef _H9_H9CONNECTOR_H_
#define _H9_H9CONNECTOR_H_


#include "genericmsg.h"

class H9Connector {
private:
    int sockfd;
    std::uint32_t recv_header();
    std::string recv_data(std::uint32_t data_to_read);
public:
    H9Connector(std::string hostname, std::string port);
    ~H9Connector();
    int connect();
    GenericMsg recv();
    void send(const GenericMsg& msg);
};


#endif //_H9_H9CONNECTOR_H_
