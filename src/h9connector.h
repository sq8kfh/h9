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
#include "h9msgsocket.h"

class H9Connector {
private:
    H9MsgSocket h9socket;
public:
    H9Connector(std::string hostname, std::string port) noexcept;
    ~H9Connector() noexcept;

    /*
     * throw: std::system_error
     */
    void connect(std::string entity);
    void close() noexcept;
    void shutdown_read() noexcept;

    /*
     * throw: std::system_error
     */
    GenericMsg recv();

    /*
     * throw: std::system_error
     * return: the id of the sent message
     */
    std::uint64_t send(GenericMsg msg);

    /*
     * throw: std::system_error
     * return: the id of the sent message
     */
    std::uint64_t send(GenericMsg msg, std::uint64_t id);

    std::uint64_t get_next_id() noexcept;
};


#endif //_H9_H9CONNECTOR_H_
