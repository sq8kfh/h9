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
#include "executor.h"
#include "protocol/callmsg.h"
#include "protocol/responsemsg.h"


class TCPClientThread;
class TCPServer;

class ExecutorAdapter {
private:
    Executor * const executor;
    TCPServer * const tcpserver;
public:
    ExecutorAdapter(Executor *executor, TCPServer *tcpserver) noexcept;

    GenericMsg execute_method(CallMsg callmsg, TCPClientThread *client);
    void cleanup_connection(TCPClientThread *client);
};


#endif //H9_EXECUTORADAPTER_H
