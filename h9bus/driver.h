/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-04-28.
 *
 * Copyright (C) 2019-2020 Kamil Palkowski. All rights reserved.
 */

#ifndef DRIVER_H
#define DRIVER_H

#include "config.h"
#include <queue>
#include <string>

#include "socketmgr.h"
#include "busmgr.h"
#include "busframe.h"
#include "bus/h9frame.h"


class Driver: public SocketMgr::Socket {
private:
    struct BusFrameLess {
        bool operator() (const BusFrame *x, const BusFrame *y) const {
            return x->get_frame().priority > y->get_frame().priority; //HIGH = 0, LOW = 1
        }
    };

    std::priority_queue<BusFrame*, std::vector<BusFrame*>, BusFrameLess> send_queue;

    BusMgr::EventCallback _event_callback;

    std::uint32_t sent_frames;
    std::uint32_t received_frames;
protected:
    void on_frame_recv(const H9frame& frame);
    void on_frame_send();

    virtual void recv_data() = 0;
    virtual void send_data(const H9frame& frame) = 0;
public:
    int retry_auto_connect;
    explicit Driver(BusMgr::EventCallback event_callback);
    virtual void open() = 0;
    void on_close() noexcept;

    std::uint32_t get_counter(BusMgr::CounterType counter);

    void send_frame(BusFrame *busframe);
    void on_select();
    ~Driver();
};


#endif //DRIVER_H
