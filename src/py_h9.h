/*
* H9 project
*
* Created by crowx on 2023-09-23.
*
* Copyright (C) 2023 Kamil Palkowski. All rights reserved.
*/

#pragma once

#include <Python.h>
#include "h9frame.h"

typedef struct {
    PyObject_HEAD
        H9frame frame;
    std::uint8_t LOW;
} PyH9frame;

PyMODINIT_FUNC PyInit_h9(void);
PyObject* PyH9Frame_New(const H9frame& frame);
