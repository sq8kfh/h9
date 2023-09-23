/*
 * H9 project
 *
 * Created by crowx on 2023-09-23.
 *
 * Copyright (C) 2023 Kamil Palkowski. All rights reserved.
 */

#pragma once

#include "virtual_driver.h"
#include "virtual_py_node.h"

class VirtualEndpoint {
  private:
    int _socket;
    int _second_end_socket;

    VirtualPyNode* node;

    std::thread thread_desc;

    std::atomic_bool virtual_endpoint_thread_run;
    std::thread virtual_endpoint_thread_desc;
    void virtual_endpoint_thread();
    void load_nodes();
    void destroy_nodes();
  public:
    VirtualEndpoint();
    ~VirtualEndpoint();

    VirtualDriver* get_driver(const std::string& name);
    void activate();
};
