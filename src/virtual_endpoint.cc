/*
 * H9 project
 *
 * Created by crowx on 2023-09-23.
 *
 * Copyright (C) 2023 Kamil Palkowski. All rights reserved.
 */

#include "virtual_endpoint.h"

#include <spdlog/spdlog.h>
#include <sys/socket.h>
#include <utility>

#include "h9d_configurator.h"

void VirtualEndpoint::virtual_endpoint_thread() {
    // close(_second_end_socket); //kiedy zamkyem druga strone kqueue zwraca Bad file descriptor?

    create_node();

    while (virtual_endpoint_thread_run) {
        H9frame frame;
        ssize_t ret = recv(_socket, &frame, sizeof(H9frame), 0);
        if (ret == -1) {
            throw std::system_error(errno, std::generic_category(), __FILE__ + std::string(":") + std::to_string(__LINE__));
        }

        node->on_frame(frame);
    }

    destroy_nodes();
}

void VirtualEndpoint::create_node() {
    node = new VirtualPyNode(node_id, node_type, python_path, module_name, this);
}

void VirtualEndpoint::destroy_nodes() {
    delete node;
    node = nullptr;
}

void VirtualEndpoint::reload_node(int id, std::string py_module) {
    node_id = id;
    module_name = std::move(py_module);

    delete node;
    node = new VirtualPyNode(node_id, node_type, python_path, module_name, this);
}

VirtualEndpoint::VirtualEndpoint():
    node(nullptr) {
    virtual_endpoint_thread_run = true;

    logger = spdlog::get(H9dConfigurator::vendpoint_logger_name);

    node_id = 0;
    module_name = "";
    python_path = "";

    int sockets[2];

    if (socketpair(AF_UNIX, SOCK_DGRAM, 0, sockets) < 0) {
        SPDLOG_LOGGER_CRITICAL(logger, "Virtual Endpoint - unable to create socket: {}.", strerror(errno));
        exit(EXIT_FAILURE);
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

void VirtualEndpoint::set_python_path(const std::string& py_path) {
    python_path = py_path;
}

void VirtualEndpoint::add_node(int id, int type, const std::string& py_module_name) {
    node_id = id;
    node_type = type;
    module_name = py_module_name;
}

bool VirtualEndpoint::is_configured() {
    return node_id && !module_name.empty() && !python_path.empty();
}

void VirtualEndpoint::activate() {
    virtual_endpoint_thread_desc = std::thread([this]() {
        this->virtual_endpoint_thread();
    });
}

void VirtualEndpoint::send_frame(const H9frame& frame) {
    ssize_t ret = send(_socket, &frame, sizeof(H9frame), 0);
    if (ret == -1) {
        throw std::system_error(errno, std::generic_category(), __FILE__ + std::string(":") + std::to_string(__LINE__));
    }
}
