/*
 * H9 project
 *
 * Created by crowx on 2023-09-23.
 *
 * Copyright (C) 2023 Kamil Palkowski. All rights reserved.
 */

#include "virtual_endpoint.h"

#include <sys/socket.h>
#include <spdlog/spdlog.h>

void VirtualEndpoint::virtual_endpoint_thread() {
    //close(_second_end_socket); //kiedy zamkyem druga strone kqueue zwraca Bad file descriptor?
    load_nodes();

    while (virtual_endpoint_thread_run) {
        H9frame frame;
        int ret = recv(_socket, &frame, sizeof(H9frame), 0);
        if (ret == -1) {
            throw std::system_error(errno, std::generic_category(), __FILE__ + std::string(":") + std::to_string(__LINE__));
        }

        node->on_frame(frame);

        //send(_socket, &frame, sizeof(H9frame), 0);
        //SPDLOG_INFO("sock {} - {}",_socket, int(frame.seqnum));
    }

    destroy_nodes();
}

void VirtualEndpoint::load_nodes() {
    node = new VirtualPyNode("/Users/crowx/projekty/h9/h9/virtual_node/rpi_gpio_node.py", this);
}

void VirtualEndpoint::destroy_nodes() {
    delete node;
    node = nullptr;
}

VirtualEndpoint::VirtualEndpoint(): node(nullptr) {
    virtual_endpoint_thread_run = true;

    int sockets[2];

    if (socketpair(AF_UNIX, SOCK_DGRAM, 0, sockets) < 0) {
        perror("opening stream socket pair");
        exit(1);
    }

    _socket = sockets[0];
    _second_end_socket = sockets[1];
}

VirtualEndpoint::~VirtualEndpoint() {
    virtual_endpoint_thread_run = false;

    if (virtual_endpoint_thread_desc.joinable())
        virtual_endpoint_thread_desc.join();
}

VirtualDriver* VirtualEndpoint::get_driver(const std::string& name) {
    return new VirtualDriver(name, _second_end_socket, _socket);
}

void VirtualEndpoint::activate() {
    virtual_endpoint_thread_desc = std::thread([this]() {
        this->virtual_endpoint_thread();
    });
}
