/*
 * H9 project
 *
 * Created by SQ8KFH on 2018-07-23.
 *
 * Copyright (C) 2018-2023 Kamil Palkowski. All rights reserved.
 */

#pragma once

#include <netinet/in.h>

#include "bus_driver.h"

class LoopDriver: public BusDriver {
  private:
    sockaddr_in loopback_addr;

  public:
    explicit LoopDriver(const std::string& name);
    int open();

  private:
    int recv_data(H9frame* frame);
    int send_data(std::shared_ptr<BusFrame> busframe);
};
