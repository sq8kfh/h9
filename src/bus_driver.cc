/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-05-10.
 *
 * Copyright (C) 2019-2023 Kamil Palkowski. All rights reserved.
 */

#include "bus_driver.h"

#include <utility>
#include <cstring>
#include <unistd.h>


void BusDriver::frame_sent_correctly(BusFrame *busframe) {
    busframe->inc_send_counter();
}

BusDriver::BusDriver(const std::string& name):
        name(name),
        sent_frames_counter(0),
        received_frames_counter(0),
        socket_fd(-1) {
}

BusDriver::~BusDriver() {
}

int BusDriver::get_scoket() {
    return socket_fd;
}

void BusDriver::close() {
    if (socket_fd >= 0) ::close(socket_fd);
}

int BusDriver::send_frame(BusFrame *busframe) {
    ++sent_frames_counter;
    return send_data(busframe);
}

int BusDriver::recv_frame(BusFrame *busframe) {
    H9frame frame;
    int ret = recv_data(&frame);

    *busframe = std::move(BusFrame(frame, name, 0, 0));

    ++received_frames_counter;
    return ret;
}
