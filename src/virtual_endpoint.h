/*
 * H9 project
 *
 * Created by crowx on 2023-09-23.
 *
 * Copyright (C) 2023 Kamil Palkowski. All rights reserved.
 */

#pragma once

#include <spdlog/spdlog.h>

#include "virtual_driver.h"
#include "virtual_py_node.h"

class VirtualEndpoint {
  private:
    std::shared_ptr<spdlog::logger> logger;

    int _socket;
    int _second_end_socket;

    VirtualPyNode* node;

    std::thread thread_desc;

    std::atomic_bool virtual_endpoint_thread_run{};
    std::thread virtual_endpoint_thread_desc;
    void virtual_endpoint_thread();

    int node_id;
    std::string python_path;
    std::string module_name;

    friend VirtualPyNode;
    void create_node();
    void reload_node(int node_id, std::string py_module); // WARNING: used by VirtualPyNode to destroy HIMSELF!
    void destroy_nodes();

  public:
    VirtualEndpoint();
    ~VirtualEndpoint();

    VirtualDriver* get_driver(const std::string& name);

    void set_python_path(const std::string& py_path);
    void add_node(int id, const std::string& py_module_name);

    bool is_configured();

    void activate();
    void send_frame(const H9frame& frame);
};
