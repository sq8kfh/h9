/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-04-09.
 *
 * Copyright (C) 2019-2020 Kamil Palkowski. All rights reserved.
 *
 * ========================================================
 *
 * ip link set can0 type can bitrate 125000
 * ifconfig can0 up
 *
 */

#ifndef _SOCKETCAN_H_
#define _SOCKETCAN_H_

#include "h9bus/driver.h"


class SocketCAN: public Driver {
private:
    const std::string _interface;
public:
    explicit SocketCAN(BusMgr::EventCallback event_callback, const std::string& interface);
    void open();
private:
    void recv_data();
    void send_data(const H9frame& frame);
};


#endif //_SOCKETCAN_H_
