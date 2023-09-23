/*
* H9 project
*
* Created by crowx on 2023-09-23.
*
* Copyright (C) 2023 Kamil Palkowski. All rights reserved.
*/

#pragma once

#include <Python.h>
#include <string>
#include "ext_h9frame.h"

class VirtualEndpoint;

class VirtualPyNode {
  private:
    PyObject* on_frame_func;
  public:
    VirtualPyNode(const std::string& py_file, VirtualEndpoint* vendpoint);
    ~VirtualPyNode();

    void on_frame(const H9frame& frame);
};
