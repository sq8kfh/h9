/*
 * H9 project
 *
 * Created by SQ8KFH on 2018-07-23.
 *
 * Copyright (C) 2018-2019 Kamil Palkowski. All rights reserved.
 */

#ifndef _H9_SLCAN_H_
#define _H9_SLCAN_H_

#include "h9bus/driver.h"


class Slcan: public Driver {
private:
    const std::string _tty;
    bool noblock;
    std::string recv_buf;

    const H9frame* last_send;
public:
    Slcan(BusMgr::EventCallback event_callback, const std::string& tty);
    void open();
    static std::string build_slcan_msg(const H9frame& frame);
    static H9frame parse_slcan_msg(const std::string& slcan_data);
private:
    void recv_data();
    void send_data(const H9frame& frame);
    void parse_response(const std::string& response);
    void send_ack();
};


#endif //_H9_SLCAN_H_
