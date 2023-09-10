/*
 * H9 project
 *
 * Created by SQ8KFH on 2018-07-23.
 *
 * Copyright (C) 2018-2023 Kamil Palkowski. All rights reserved.
 */

#ifndef H9_LOOP_DRIVER_H
#define H9_LOOP_DRIVER_H

#include <netinet/in.h>

#include "bus_driver.h"

class LoopDriver: public BusDriver {
  private:
    constexpr static std::uint16_t LOOPBACK_PORT = 61432;

    sockaddr_in loopback_addr;
    static int instance_counter;

  public:
    explicit LoopDriver(const std::string& name);
    int open();

  private:
    int recv_data(H9frame* frame);
    int send_data(BusFrame* busframe);
};

#endif // H9_LOOP_DRIVER_H
