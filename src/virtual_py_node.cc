/*
 * H9 project
 *
 * Created by crowx on 2023-09-23.
 *
 * Copyright (C) 2023 Kamil Palkowski. All rights reserved.
 */

#include "virtual_py_node.h"

#include <spdlog/spdlog.h>

#include "h9d_configurator.h"
#include "py_h9.h"
#include "virtual_endpoint.h"

namespace {

std::shared_ptr<spdlog::logger> py_logger;
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
            SPDLOG_LOGGER_DEBUG(py_logger, "* PyNode *  {}", h9log_buf);
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
    .m_base = PyModuleDef_HEAD_INIT,
    .m_name = "h9log",
    .m_doc = nullptr,
    .m_size = -1, /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
    .m_methods = h9log_methods,
    .m_slots = NULL,
    .m_traverse = NULL,
    .m_clear = NULL,
    .m_free = NULL};

} // namespace

VirtualEndpoint* VirtualPyNode::virtual_endpoint = nullptr;

void VirtualPyNode::send_turned_on_broadcast() {
    H9frame frame;
    frame.priority = H9frame::Priority::LOW;
    frame.type = H9frame::Type::NODE_TURNED_ON;
    frame.seqnum = 0;
    frame.source_id = node_id;
    frame.destination_id = H9frame::BROADCAST_ID;

    frame.dlc = 8;

    frame.data[0] = (NODE_TYPE >> 8) & 0xff;
    frame.data[1] = (NODE_TYPE)&0xff;
    frame.data[2] = (VERSION_MAJOR >> 8);
    frame.data[3] = VERSION_MAJOR & 0xff;
    frame.data[4] = (VERSION_MINOR >> 8) & 0xff;
    frame.data[5] = VERSION_MINOR & 0xff;
    frame.data[6] = (VERSION_PATCH >> 8) & 0xff;
    frame.data[7] = VERSION_PATCH & 0xff;
    send_frame(frame);
}

void VirtualPyNode::call_py_on_frame(const H9frame& frame) {
    if (on_frame_func) {
        PyObject* f = PyH9Frame_New(frame);

        PyObject* pArgs = PyTuple_New(1);
        PyTuple_SetItem(pArgs, 0, (PyObject*)f);

        PyObject* pValue = PyObject_CallObject(on_frame_func, pArgs);
        if (pValue != NULL) {
            // printf("Result of call: %ld\n", PyLong_AsLong(pValue));
            Py_DECREF(pValue);
        }
        else {
            //TODO: co robic jak sie funkcja pythona wysypie? zignorwac czy wywalic modul?
            //Py_DECREF(on_frame_func);
            //on_frame_func = nullptr;
            PyErr_Print();
            SPDLOG_LOGGER_WARN(logger, "Call 'on_frame' function failed.");
        }
    }
}

void VirtualPyNode::reset() {
    virtual_endpoint->reload_node(new_node_id, py_module);
}

bool VirtualPyNode::send_frame(const H9frame& frame) {
    if (VirtualPyNode::virtual_endpoint) {
        VirtualPyNode::virtual_endpoint->send_frame(frame);
    }
    return false;
}

VirtualPyNode::VirtualPyNode(int node_id, const std::string& py_path, const std::string& py_module, VirtualEndpoint* vendpoint):
    node_id(node_id),
    py_module(py_module),
    on_frame_func(nullptr) {
    virtual_endpoint = vendpoint;
    new_node_id = node_id;

    logger = spdlog::get(H9dConfigurator::vendpoint_logger_name);
    ::py_logger = spdlog::get(H9dConfigurator::vendpoint_logger_name);

    SPDLOG_LOGGER_INFO(logger, "Loading Python virtual node: '{}' with id: {}.", py_module, node_id);

    PyImport_AppendInittab("h9", &PyInit_h9);

    PyConfig config;

    // PyConfig_InitPythonConfig(&config);
    PyConfig_InitIsolatedConfig(&config);

    PyConfig_SetBytesString(&config, &config.program_name, "h9");

    Py_InitializeFromConfig(&config);

    PyConfig_Clear(&config);

    PyObject* sysPath = PySys_GetObject("path");
    PyList_Append(sysPath, PyUnicode_FromString(py_path.c_str()));
    //PySys_SetPath(L"/Users/crowx/projekty/h9/h9/virtual_node");

    PyObject* h9log = PyModule_Create(&h9log_module);
    PyObject* sys = PyImport_ImportModule("sys");
    PyObject_SetAttrString(sys, "stdout", h9log);
    PyObject_SetAttrString(sys, "stderr", h9log);
    PyObject_SetAttrString(sys, "stdin", nullptr);

    PyObject* pName = PyUnicode_DecodeFSDefault(py_module.c_str());

    PyObject* pModule = PyImport_Import(pName);

    Py_DECREF(h9log);
    Py_DECREF(sys);
    Py_DECREF(pName);

    if (pModule != NULL) {
        on_frame_func = PyObject_GetAttrString(pModule, "on_frame");
        if (on_frame_func && PyCallable_Check(on_frame_func)) {
            SPDLOG_LOGGER_INFO(logger, "Loaded 'on_frame' method for '{}' PyNode.", py_module);
        }
        else {
            if (PyErr_Occurred())
                PyErr_Print();
            SPDLOG_LOGGER_WARN(logger, "Cannot find 'on_frame' function in '{}' PyNode.", py_module);
        }
        Py_XDECREF(on_frame_func);
        Py_DECREF(pModule);
    }
    else {
        PyErr_Print();
        SPDLOG_LOGGER_ERROR(logger, "Failed to load '{}' PyNode module.", py_module.c_str());
    }

    send_turned_on_broadcast();
}

VirtualPyNode::~VirtualPyNode() {
    Py_XDECREF(on_frame_func);
    //    Py_FinalizeEx();
    SPDLOG_LOGGER_INFO(logger, "Unload Python virtual node: '{}' with id: {}.", py_module, node_id);
};

void VirtualPyNode::on_frame(const H9frame& frame) {
    if (frame.type == H9frame::Type::DISCOVER && (frame.destination_id == node_id || frame.destination_id == H9frame::BROADCAST_ID)) {
        H9frame res;
        res.priority = frame.priority;
        res.type = H9frame::Type::NODE_INFO;
        res.seqnum = frame.seqnum;
        res.source_id = node_id;
        res.destination_id = frame.source_id;

        res.dlc = 8;
        res.data[0] = (NODE_TYPE >> 8) & 0xff;
        res.data[1] = (NODE_TYPE)&0xff;
        res.data[2] = (VERSION_MAJOR >> 8);
        res.data[3] = VERSION_MAJOR & 0xff;
        res.data[4] = (VERSION_MINOR >> 8) & 0xff;
        res.data[5] = VERSION_MINOR & 0xff;
        res.data[6] = (VERSION_PATCH >> 8) & 0xff;
        res.data[7] = VERSION_PATCH & 0xff;
        send_frame(res);
    }
    else if (frame.type == H9frame::Type::NODE_RESET && (frame.destination_id == node_id || frame.destination_id == H9frame::BROADCAST_ID)) {
        reset();
        return;
    }
    else if (frame.type == H9frame::Type::SET_REG && frame.destination_id == node_id && frame.dlc > 1) {
        if (frame.data[0] >= 10) {
            call_py_on_frame(frame);
        }
        else {
            H9frame res;
            res.priority = frame.priority;
            res.seqnum = frame.seqnum;
            res.source_id = node_id;
            res.destination_id = frame.source_id;

            if (frame.data[0] == 4 && frame.dlc == 3) { // reg 4
                res.type = H9frame::Type::REG_EXTERNALLY_CHANGED;
                res.data[0] = frame.data[0];

                new_node_id = (frame.data[1] & 0x01) << 8 | frame.data[2];

                res.data[1] = frame.data[1];
                res.data[2] = frame.data[2];
                res.dlc = 3;
            }
            else {
                res.type = H9frame::Type::ERROR;
                res.data[0] = H9frame::to_underlying(H9frame::Error::INVALID_REGISTER);
                res.dlc = 1;
            }
            send_frame(res);
        }
    }
    else if (frame.type == H9frame::Type::GET_REG && frame.destination_id == node_id && frame.dlc == 1) {
        if (frame.data[0] >= 10) {
            call_py_on_frame(frame);
        }
        else {
            H9frame res;
            res.priority = frame.priority;
            res.type = H9frame::Type::REG_VALUE;
            res.seqnum = frame.seqnum;
            res.source_id = node_id;
            res.destination_id = frame.source_id;

            res.data[0] = frame.data[0];
            if (frame.data[0] == 1) { // type
                res.data[1] = (NODE_TYPE >> 8) & 0xff;
                res.data[2] = (NODE_TYPE ) & 0xff;
                res.dlc = 3;
            }
            else if (frame.data[0] == 2) {
                res.data[1] = (VERSION_MAJOR >> 8);
                res.data[2] = VERSION_MAJOR & 0xff;
                res.data[3] = (VERSION_MINOR >> 8) & 0xff;
                res.data[4] = VERSION_MINOR & 0xff;
                res.data[5] = (VERSION_PATCH >> 8) & 0xff;
                res.data[6] = VERSION_PATCH & 0xff;
                res.dlc = 7;
            }
            else if (frame.data[0] == 3) {
                int max = 6 < py_module.size() ? 6 : py_module.size();

                for (int i = 0; i < max; ++i){
                    res.data[i+1] = py_module.at(i);
                }
                res.dlc = max+1;
            }
            else if (frame.data[0] == 4) {
                res.data[1] = (new_node_id >> 8) & 0x01;
                res.data[2] = (new_node_id) & 0xff;
                res.dlc = 3;
            }
            else if (frame.data[0] == 5) { //cpu type
                res.data[1] = 0xff;
                res.dlc = 2;
            }
            else {
                res.type = H9frame::Type::ERROR;
                res.data[0] = H9frame::to_underlying(H9frame::Error::INVALID_REGISTER);
                res.dlc = 1;
            }
            send_frame(res);
        }
    }
    else if (frame.type == H9frame::Type::SET_BIT && frame.destination_id == node_id) {
        call_py_on_frame(frame);
    }
    else if (frame.type == H9frame::Type::CLEAR_BIT && frame.destination_id == node_id) {
        call_py_on_frame(frame);
    }
    else if (frame.type == H9frame::Type::TOGGLE_BIT && frame.destination_id == node_id) {
        call_py_on_frame(frame);
    }
    else if (frame.type == H9frame::Type::NODE_UPGRADE && frame.destination_id == node_id) {
        H9frame res;
        res.priority = frame.priority;
        res.type = H9frame::Type::ERROR;
        res.seqnum = frame.seqnum;
        res.source_id = node_id;
        res.destination_id = frame.source_id;
        res.dlc = 1;
        res.data[0] = H9frame::to_underlying(H9frame::Error::UNSUPPORTED_BOOTLOADER);
        send_frame(res);
    }
}
