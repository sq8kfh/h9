/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-04-09.
 *
 * Copyright (C) 2019-2023 Kamil Palkowski. All rights reserved.
 *
 * ========================================================
 *
 * ip link set can0 type can bitrate 125000
 * ip link set can0 txqueuelen 1000
 * ifconfig can0 up
 *
 */

#pragma once

#include "bus_driver.h"

class SocketCANDriver: public BusDriver {
  private:
    const std::string _interface;

  public:
    explicit SocketCANDriver(const std::string& name, const std::string& interface);
    int open();

  private:
    int recv_data(H9frame* frame);
    int send_data(BusFrame* busframe);
};
