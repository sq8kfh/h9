/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-05-16.
 *
 * Copyright (C) 2019-2020 Kamil Palkowski. All rights reserved.
 */

#ifndef _H9_H9CONNECTOR_H_
#define _H9_H9CONNECTOR_H_

#include "config.h"
#include "genericmsg.h"
#include "h9socket.h"

class H9Connector: protected H9Socket {
public:
    H9Connector(std::string hostname, std::string port) noexcept;
    ~H9Connector() noexcept;
    int connect() noexcept;

    std::uint64_t get_next_id(void);
    GenericMsg recv(int timeout_in_seconds = 0);
    void send(GenericMsg msg, std::uint64_t msg_id = 0);
};


#endif //_H9_H9CONNECTOR_H_
