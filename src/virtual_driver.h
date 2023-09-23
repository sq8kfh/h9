/*
 * H9 project
 *
 * Created by crowx on 2023-09-23.
 *
 * Copyright (C) 2023 Kamil Palkowski. All rights reserved.
 */

#pragma once

#include "bus_driver.h"

class VirtualDriver: public BusDriver {
  private:
    int second_end_socket;
  public:
    explicit VirtualDriver(const std::string& name, int socket, int second_end_socket);
    int open();

  private:
    int recv_data(H9frame* frame);
    int send_data(std::shared_ptr<BusFrame> busframe);
};
