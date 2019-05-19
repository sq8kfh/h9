#include <system_error>
#include <cstdlib>
#include <cstring>
#include "loop.h"

Loop::Loop(BusMgr::EventCallback event_callback):
        Driver(std::move(event_callback)) {
    bzero(&loopback_addr, sizeof(loopback_addr));
    loopback_addr.sin_family = AF_INET;
    loopback_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    loopback_addr.sin_port = htons(LOOPBACK_PORT);
}

void Loop::open() {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (bind(sockfd, (const struct sockaddr *)&loopback_addr, sizeof(loopback_addr)) == -1) {
        throw std::system_error(errno, std::generic_category(), __FILE__ + std::string(":") + std::to_string(__LINE__));
    }

    set_socket(sockfd);
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
        on_frame_send(frame);
    }
}
