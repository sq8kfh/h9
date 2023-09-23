/*
 * H9 project
 *
 * Created by crowx on 2023-09-23.
 *
 * Copyright (C) 2023 Kamil Palkowski. All rights reserved.
 */

#include "virtual_driver.h"

#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <fcntl.h>

VirtualDriver::VirtualDriver(const std::string& name, int socket, int second_end_socket):
    BusDriver(name, "virtual"),
    second_end_socket(second_end_socket) {
    socket_fd = socket;
}

int VirtualDriver::open() {
    //::close(second_end_socket); //kiedy zamkyem druga strone kqueue zwraca Bad file descriptor?
    return socket_fd;
}

int VirtualDriver::recv_data(H9frame* frame) {
    int ret = recv(socket_fd, frame, sizeof(H9frame), 0);
    if (ret == -1) {
        throw std::system_error(errno, std::generic_category(), __FILE__ + std::string(":") + std::to_string(__LINE__));
    }
    //    else {
    //        on_frame_recv(buf);
    //    }
    return ret;
}

int VirtualDriver::send_data(std::shared_ptr<BusFrame> busframe) {
    int ret = send(socket_fd, &busframe->frame(), sizeof(H9frame), 0);
    if (ret == -1) {
        throw std::system_error(errno, std::generic_category(), __FILE__ + std::string(":") + std::to_string(__LINE__));
    }
    else {
        frame_sent_correctly(busframe);
    }

    return ret;
}