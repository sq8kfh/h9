/*
 * H9 project
 *
 * Created by crowx on 2023-09-23.
 *
 * Copyright (C) 2023 Kamil Palkowski. All rights reserved.
 */

#include "virtual_py_node.h"

#include <spdlog/spdlog.h>

#include "py_h9.h"

namespace {

std::string h9log_buf;

static PyObject* h9log_write(PyObject* self, PyObject* args) {
    const char* data;
    int ret = 0;

    if (!PyArg_ParseTuple(args, "s", &data))
        return NULL;

    while (*data) {
        if (*data != '\n') {
            h9log_buf.append(1, *data);
        }
        else {
            SPDLOG_DEBUG("PyNode: {}", h9log_buf);
            h9log_buf.clear();
        }
        ++ret;
        ++data;
    }

    return PyLong_FromLong(ret);
}

static PyObject* h9log_flush(PyObject* self, PyObject* args) {
    return Py_None;
}

static PyMethodDef h9log_methods[] = {
    {"write", h9log_write, METH_VARARGS, "Write to logger."},
    {"flush", h9log_flush, METH_VARARGS, "Flush logger."},
    {NULL, NULL, 0, NULL} /* Sentinel */
};

static struct PyModuleDef h9log_module = {
    PyModuleDef_HEAD_INIT,
    "h9log", /* name of module */
    nullptr, /* module documentation, may be NULL */
    -1,      /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
    h9log_methods,
    NULL, NULL, NULL, NULL};

/*static PyObject* PyInit_h9logger(void) {
    return PyModule_Create(&h9logger_module);
}*/

} // namespace

VirtualPyNode::VirtualPyNode(const std::string& py_file, VirtualEndpoint* vendpoint):
    on_frame_func(nullptr) {
    PyImport_AppendInittab("h9", &PyInit_h9);
    Py_SetProgramName(L"VirtualPyNode");
    Py_Initialize();
    PySys_SetPath(L"/Users/crowx/projekty/h9/h9/virtual_node/");

    PyObject* h9log = PyModule_Create(&h9log_module);
    PyObject* sys = PyImport_ImportModule("sys");
    PyObject_SetAttrString(sys, "stdout", h9log);
    PyObject_SetAttrString(sys, "stderr", h9log);
    PyObject_SetAttrString(sys, "stdin", nullptr);

    PyObject* pName = PyUnicode_DecodeFSDefault("rpi_gpio_node");

    PyObject* pModule = PyImport_Import(pName);

    Py_DECREF(h9log);
    Py_DECREF(sys);
    Py_DECREF(pName);

    if (pModule != NULL) {
        on_frame_func = PyObject_GetAttrString(pModule, "on_frame");
        if (on_frame_func && PyCallable_Check(on_frame_func)) {
            SPDLOG_INFO("Loaded on_frame method for PyNode");
        }
        else {
            if (PyErr_Occurred())
                PyErr_Print();
            printf("Cannot find function \"on_frame\"\n");
        }
        Py_XDECREF(on_frame_func);
        Py_DECREF(pModule);
    }
    else {
        PyErr_Print();
        fprintf(stderr, "Failed to load \"%s\"\n", py_file.c_str());
    }
}

VirtualPyNode::~VirtualPyNode() {
    Py_XDECREF(on_frame_func);
    Py_FinalizeEx();
};

void VirtualPyNode::on_frame(const H9frame& frame) {
    if (on_frame_func) {

        //PyH9frame t;
        //t.frame = frame;

        PyObject *f = PyH9Frame_New(frame);

        PyObject* pArgs = PyTuple_New(1);
        PyTuple_SetItem(pArgs, 0, (PyObject*)f);

        PyObject* pValue = PyObject_CallObject(on_frame_func, pArgs);
        if (pValue != NULL) {
            // printf("Result of call: %ld\n", PyLong_AsLong(pValue));
            Py_DECREF(pValue);
        }
        else {
            Py_DECREF(on_frame_func);
            PyErr_Print();
            printf("Call failed\n");
        }
    }
}
