/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-04-28.
 *
 * Copyright (C) 2019-2020 Kamil Palkowski. All rights reserved.
 */

#ifndef _H9_DUMMY_H_
#define _H9_DUMMY_H_

#include <netinet/in.h>

#include "h9bus/driver.h"


class Dummy: public Driver {
    constexpr static std::uint16_t LOOPBACK_PORT = 61433;

    sockaddr_in loopback_addr;
public:
    explicit Dummy(const std::string& name, TRecvFrameCallback recv_frame_callback, TSendFrameCallback send_frame_callback);
    void open();
private:
    void recv_data();
    void send_data(const H9frame& frame);
};


#endif //_H9_DUMMY_H_
