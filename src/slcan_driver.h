/*
 * H9 project
 *
 * Created by SQ8KFH on 2018-07-23.
 *
 * Copyright (C) 2018-2020 Kamil Palkowski. All rights reserved.
 */

#ifndef H9_SLCAN_DRIVER_H
#define H9_SLCAN_DRIVER_H

#include "bus_driver.h"


class SlcanDriver: public BusDriver {
private:
    const std::string _tty;
    bool noblock;
    std::string recv_buf;

    //TODO: Zmienic na kolejke
    BusFrame* last_send;
public:
    SlcanDriver(const std::string& name, const std::string& tty);
    int open();

    static std::string build_slcan_msg(const H9frame& frame);
    static H9frame parse_slcan_msg(const std::string& slcan_data);
private:
    int recv_data(H9frame *frame);
    int send_data(BusFrame *busframe);
    void parse_response(const std::string& response);
    void send_ack();
};


#endif //H9_SLCAN_DRIVER_H
