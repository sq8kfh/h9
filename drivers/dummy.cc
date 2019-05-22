/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-04-28.
 *
 * Copyright (C) 2019 Kamil Palkowski. All rights reserved.
 */

#include "dummy.h"

#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <system_error>

Dummy::Dummy(BusMgr::EventCallback event_callback):
        Driver(std::move(event_callback)) {
    bzero(&loopback_addr, sizeof(loopback_addr));
    loopback_addr.sin_family = AF_INET;
    loopback_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    loopback_addr.sin_port = htons(LOOPBACK_PORT);
}

void Dummy::open() {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (bind(sockfd, (const struct sockaddr *)&loopback_addr, sizeof(loopback_addr)) == -1) {
        throw std::system_error(errno, std::generic_category(), __FILE__ + std::string(":") + std::to_string(__LINE__));
    }

    set_socket(sockfd, true);

}

void Dummy::recv_data() {
    sockaddr_in tmp_addr;
    socklen_t len = sizeof(tmp_addr);
    bcopy(&loopback_addr, &tmp_addr, len);

    H9frame buf;
    if(recvfrom(get_socket(), &buf, sizeof(buf), 0, (struct sockaddr *)&tmp_addr, &len) == -1) {
        throw std::system_error(errno, std::generic_category(), __FILE__ + std::string(":") + std::to_string(__LINE__));
    }
}

void Dummy::send_data(const H9frame& frame) {
    on_frame_send(frame);
}
