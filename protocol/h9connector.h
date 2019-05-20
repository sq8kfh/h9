/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-05-16.
 *
 * Copyright (C) 2019 Kamil Palkowski. All rights reserved.
 */

#ifndef _H9_H9CONNECTOR_H_
#define _H9_H9CONNECTOR_H_

#include "config.h"
#include "genericmsg.h"


class H9Connector {
private:
    int sockfd;
    std::string _hostname;
    std::string _port;
    std::uint32_t recv_header();
    std::string recv_data(std::uint32_t data_to_read);
public:
    H9Connector(std::string hostname, std::string port) noexcept;
    ~H9Connector() noexcept;
    int connect() noexcept;
    GenericMsg recv();
    void send(const GenericMsg& msg);
};


#endif //_H9_H9CONNECTOR_H_
