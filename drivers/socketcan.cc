/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-04-09.
 *
 * Copyright (C) 2019 Kamil Palkowski. All rights reserved.
 */

#include "socketcan.h"

SocketCAN::SocketCAN(BusMgr::EventCallback event_callback):
        Driver(std::move(event_callback)) {

}

void SocketCAN::open() {

}

void SocketCAN::recv_data() {

}

void SocketCAN::send_data(const H9frame& frame) {

}
