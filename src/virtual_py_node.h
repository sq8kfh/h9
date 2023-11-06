/*
 * H9 project
 *
 * Created by crowx on 2023-09-23.
 *
 * Copyright (C) 2023 Kamil Palkowski. All rights reserved.
 */

#pragma once

#include <Python.h>
#include <spdlog/spdlog.h>
#include <string>

#include "ext_h9frame.h"

class VirtualEndpoint;

class VirtualPyNode {
  private:
    static VirtualEndpoint* virtual_endpoint;

    std::shared_ptr<spdlog::logger> logger;

    const std::string py_module;
    PyObject* on_frame_func;

    const std::uint16_t node_id;
    const std::uint16_t node_type;
    int new_node_id;

    void send_turned_on_broadcast();
    void call_py_on_frame(const H9frame& frame);
    void reset();

  public:
    constexpr static std::uint16_t VERSION_MAJOR = 1;
    constexpr static std::uint16_t VERSION_MINOR = 0;
    constexpr static std::uint16_t VERSION_PATCH = 0;

    static bool send_frame(const H9frame& frame);

    VirtualPyNode(std::uint16_t node_id, std::uint16_t node_type, const std::string& py_path, const std::string& py_module, VirtualEndpoint* vendpoint);
    ~VirtualPyNode();

    void on_frame(const H9frame& frame);
};
