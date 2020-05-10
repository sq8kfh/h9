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
#include <functional>
#include <queue>
#include <string>
#include "socketmgr.h"
#include "busmgr.h"
#include "busframe.h"
#include "bus/h9frame.h"


class Driver: public SocketMgr::Socket {
public:
    using TRecvFrameCallback = std::function<void(Driver*, const H9frame&)>;
    using TSendFrameCallback = std::function<void(Driver*, BusFrame*)>;
private:
    struct BusFrameLess {
        bool operator() (const BusFrame *x, const BusFrame *y) const {
            return x->get_frame().priority > y->get_frame().priority; //HIGH = 0, LOW = 1
        }
    };

    TRecvFrameCallback recv_frame_callback;
    TSendFrameCallback send_frame_callback;

    std::priority_queue<BusFrame*, std::vector<BusFrame*>, BusFrameLess> send_queue;

    std::uint32_t sent_frames_counter;
    std::uint32_t received_frames_counter;
protected:
    void on_frame_recv(const H9frame& frame);
    void on_frame_send();

    virtual void recv_data() = 0;
    virtual void send_data(const H9frame& frame) = 0;
public:
    const std::string name;
    int retry_auto_connect;
    explicit Driver(const std::string& name, TRecvFrameCallback recv_frame_callback, TSendFrameCallback send_frame_callback);
    virtual void open() = 0;
    void on_close() noexcept;

    std::uint32_t get_counter(BusMgr::CounterType counter);

    void send_frame(BusFrame *busframe);
    void on_select();
    ~Driver();
};


#endif //DRIVER_H
