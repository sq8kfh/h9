/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-11-21.
 *
 * Copyright (C) 2020 Kamil Palkowski. All rights reserved.
 */

#ifndef H9_EXECUTORADAPTER_H
#define H9_EXECUTORADAPTER_H

#include "config.h"
#include <list>
#include <tuple>
#include "executor.h"
#include "protocol/devicemethodresponsemsg.h"
#include "protocol/executemethodmsg.h"
#include "protocol/methodresponsemsg.h"


class TCPClientThread;
class TCPServer;

class ExecutorAdapter {
private:
    Executor * const executor;
    TCPServer * const tcpserver;
    TCPClientThread *_client;

    std::list<std::tuple<std::uint16_t, std::string>> attach_device_event_observer_memento;
    int attach_device_event_observer(std::uint16_t dev_id, std::string event_name);
    int detach_device_event_observer(std::uint16_t dev_id, std::string event_name);
public:
    ExecutorAdapter(Executor *executor, TCPServer *tcpserver) noexcept;
    void set_client(TCPClientThread *client);

    GenericMsg execute_method(ExecuteMethodMsg execmsg);
    GenericMsg execute_device_method(DeviceMethodResponseMsg execmsg);

    void cleanup_connection();
};


#endif //H9_EXECUTORADAPTER_H