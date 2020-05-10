/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-04-28.
 *
 * Copyright (C) 2019-2020 Kamil Palkowski. All rights reserved.
 */

#include <system_error>
#include <cstdlib>
#include <cstring>
#include "loop.h"

Loop::Loop(const std::string& name, TRecvFrameCallback recv_frame_callback, TSendFrameCallback send_frame_callback):
        Driver(name, std::move(recv_frame_callback), std::move(send_frame_callback)) {
    bzero(&loopback_addr, sizeof(loopback_addr));
    loopback_addr.sin_family = AF_INET;
    loopback_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    loopback_addr.sin_port = htons(LOOPBACK_PORT);
}

void Loop::open() {
    //TODO: rewite to socketpair
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (bind(sockfd, (const struct sockaddr *)&loopback_addr, sizeof(loopback_addr)) == -1) {
        throw std::system_error(errno, std::generic_category(), __FILE__ + std::string(":") + std::to_string(__LINE__));
    }

    set_socket(sockfd, true);
}

void Loop::recv_data() {
    sockaddr_in tmp_addr;
    socklen_t len = sizeof(tmp_addr);
    bcopy(&loopback_addr, &tmp_addr, len);

    H9frame buf;
    if (recvfrom(get_socket(), &buf, sizeof(buf), 0, (struct sockaddr *)&tmp_addr, &len) == -1) {
        throw std::system_error(errno, std::generic_category(), __FILE__ + std::string(":") + std::to_string(__LINE__));
    }
    else {
        on_frame_recv(buf);
    }
}

void Loop::send_data(const H9frame& frame) {
    if (sendto(get_socket(), &frame, sizeof(frame), 0, (const struct sockaddr *)&loopback_addr, sizeof(loopback_addr)) == -1) {
        throw std::system_error(errno, std::generic_category(), __FILE__ + std::string(":") + std::to_string(__LINE__));
    }
    else {
        on_frame_send();
    }
}
