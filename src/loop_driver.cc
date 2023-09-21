/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-04-28.
 *
 * Copyright (C) 2019-2023 Kamil Palkowski. All rights reserved.
 */

#include "loop_driver.h"

#include <cstdlib>
#include <cstring>
#include <system_error>

int LoopDriver::instance_counter = 0;

LoopDriver::LoopDriver(const std::string& name):
    BusDriver(name, "loop") {
    bzero(&loopback_addr, sizeof(loopback_addr));
    loopback_addr.sin_family = AF_INET;
    loopback_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    loopback_addr.sin_port = htons(LOOPBACK_PORT + instance_counter);
    ++instance_counter;
}

int LoopDriver::open() {
    // TODO: rewite to socketpair
    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);

    if (bind(socket_fd, (const struct sockaddr*)&loopback_addr, sizeof(loopback_addr)) == -1) {
        throw std::system_error(errno, std::generic_category(), __FILE__ + std::string(":") + std::to_string(__LINE__));
    }

    return socket_fd;
}

int LoopDriver::recv_data(H9frame* frame) {
    sockaddr_in tmp_addr;
    socklen_t len = sizeof(tmp_addr);
    bcopy(&loopback_addr, &tmp_addr, len);

    return recvfrom(socket_fd, frame, sizeof(H9frame), 0, (struct sockaddr*)&tmp_addr, &len);
    /*
    if ( == -1) {
        throw std::system_error(errno, std::generic_category(), __FILE__ + std::string(":") + std::to_string(__LINE__));
    }
    else {
        on_frame_recv(buf);
    }*/
}

int LoopDriver::send_data(std::shared_ptr<BusFrame> busframe) {
    int ret = sendto(socket_fd, &busframe->frame(), sizeof(H9frame), 0, (const struct sockaddr*)&loopback_addr, sizeof(loopback_addr));

    if (ret == -1) {
        throw std::system_error(errno, std::generic_category(), __FILE__ + std::string(":") + std::to_string(__LINE__));
    }
    else {
        frame_sent_correctly(busframe);
    }

    return ret;
}
