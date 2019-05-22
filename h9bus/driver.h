/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-04-28.
 *
 * Copyright (C) 2019 Kamil Palkowski. All rights reserved.
 */

#ifndef DRIVER_H
#define DRIVER_H

#include "config.h"
#include <queue>
#include <string>

#include "socketmgr.h"
#include "busmgr.h"
#include "bus/h9frame.h"


class Driver: public SocketMgr::Socket {
private:
    std::queue<H9frame> send_queue;
    std::uint8_t next_seqnum;

    BusMgr::EventCallback _event_callback;
protected:
    void on_frame_recv(const H9frame& frame);
    void on_frame_send(const H9frame& frame);

    virtual void recv_data() = 0;
    virtual void send_data(const H9frame& frame) = 0;
public:
    int retry_auto_connect;
    explicit Driver(BusMgr::EventCallback event_callback);
    virtual void open() = 0;
    void on_close() noexcept;

    void send_frame(const H9frame& frame);
    void on_select() ;
    ~Driver();
};


#endif //DRIVER_H
