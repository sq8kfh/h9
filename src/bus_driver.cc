/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-05-10.
 *
 * Copyright (C) 2019-2023 Kamil Palkowski. All rights reserved.
 */

#include "bus_driver.h"

#include <cstring>
#include <unistd.h>
#include <utility>

void BusDriver::frame_sent_correctly(BusFrame* busframe) {
    busframe->inc_send_counter();
}

BusDriver::BusDriver(const std::string& name, const std::string& driver_name):
    name(name),
    driver_name(driver_name),
    sent_frames_counter(MetricsCollector::make_counter("bus.endpoints[name=" + name + "].send_frames")),
    received_frames_counter(MetricsCollector::make_counter("bus.endpoints[name=" + name + "].received_frames")),
    socket_fd(-1) {

    logger = spdlog::get("bus");
}

BusDriver::~BusDriver() {
}

int BusDriver::get_scoket() {
    return socket_fd;
}

void BusDriver::close() {
    if (socket_fd >= 0)
        ::close(socket_fd);
}

int BusDriver::send_frame(BusFrame* busframe) {
    ++sent_frames_counter;
    return send_data(busframe);
}

int BusDriver::recv_frame(BusFrame* busframe) {
    H9frame frame;
    int ret = recv_data(&frame);

    *busframe = std::move(BusFrame(frame, name, 0, 0));

    ++received_frames_counter;
    return ret;
}
