/*
 * H9 project
 *
 * Created by crowx on 2023-09-23.
 *
 * Copyright (C) 2023 Kamil Palkowski. All rights reserved.
 */

#include "py_h9.h"

#include <structmember.h>

#include "h9frame.h"
#include "virtual_py_node.h"

static int H9Frame_init(PyH9frame* self, PyObject* args, PyObject* kwds) {
    static char* kwlist[] = {"type", "destination_id", "data", "priority", "seqnum", "source_id", "dlc", NULL};

    std::uint8_t priority = H9frame::to_underlying(H9frame::Priority::LOW);
    std::uint8_t type = H9frame::to_underlying(H9frame::Type::NOP);
    std::uint8_t seqnum = 0;
    std::uint16_t destination_id = 0;
    std::uint16_t source_id = 0;
    std::uint8_t dlc = 0;
    PyObject* data = nullptr;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|BHOBBHB", kwlist, &type, &destination_id, &data, &priority, &seqnum, &source_id, &dlc))
        return -1;

    if (priority > H9frame::H9FRAME_PRIORITY_MAX_VALUE) {
        PyErr_SetString(PyExc_OverflowError, ("maximum allowed value is " + std::to_string(H9frame::H9FRAME_PRIORITY_MAX_VALUE)).c_str());
        return -1;
    }
    self->frame.priority = H9frame::from_underlying<H9frame::Priority>(priority);

    if (type > H9frame::H9FRAME_TYPE_MAX_VALUE) {
        PyErr_SetString(PyExc_OverflowError, ("maximum allowed value is " + std::to_string(H9frame::H9FRAME_TYPE_MAX_VALUE)).c_str());
        return -1;
    }
    self->frame.type = H9frame::from_underlying<H9frame::Type>(type);

    if (seqnum > H9frame::H9FRAME_SEQNUM_MAX_VALUE) {
        PyErr_SetString(PyExc_OverflowError, ("maximum allowed value is " + std::to_string(H9frame::H9FRAME_SEQNUM_MAX_VALUE)).c_str());
        return -1;
    }
    self->frame.seqnum = seqnum;

    if (destination_id > H9frame::H9FRAME_DESTINATION_ID_MAX_VALUE) {
        PyErr_SetString(PyExc_OverflowError, ("maximum allowed value is " + std::to_string(H9frame::H9FRAME_DESTINATION_ID_MAX_VALUE)).c_str());
        return -1;
    }
    self->frame.destination_id = destination_id;

    if (source_id > H9frame::H9FRAME_SOURCE_ID_MAX_VALUE) {
        PyErr_SetString(PyExc_OverflowError, ("maximum allowed value is " + std::to_string(H9frame::H9FRAME_SOURCE_ID_MAX_VALUE)).c_str());
        return -1;
    }
    self->frame.source_id = source_id;

    if (dlc > 8) {
        PyErr_SetString(PyExc_OverflowError, "maximum allowed value is 8");
        return -1;
    }
    self->frame.dlc = dlc;

    if (data) {
        if (!PySequence_Check(data)) {
            PyErr_SetString(PyExc_TypeError, "The 'data' attribute value must be a sequence object (e.g. tuple, list)");
            return -1;
        }

        int len = PySequence_Size(data);
        if (len > H9frame::H9FRAME_DATA_LENGTH) {
            PyErr_SetString(PyExc_OverflowError, "The length of the 'date' attribute must be less or equal than 8.");
            return -1;
        }
        for (int i = 0; i < len; ++i) {
            PyObject* item = PySequence_GetItem(data, i);
            unsigned int tmp = PyLong_AsUnsignedLong(item);
            if (PyErr_Occurred()) {
                self->frame.dlc = 0;
                return -1;
            }
            if (tmp > 255) {
                PyErr_SetString(PyExc_OverflowError, "maximum allowed value is 255");
                self->frame.dlc = 0;
                return -1;
            }
            self->frame.data[i] = tmp;
        }

        self->frame.dlc = len;
    }

    return 0;
}

static PyObject* H9Frame_get_priority(PyH9frame* self, void* closure) {
    return PyLong_FromUnsignedLong(H9frame::to_underlying(self->frame.priority));
}

static int H9Frame_set_priority(PyH9frame* self, PyObject* value, void* closure) {
    if (value == NULL) {
        PyErr_SetString(PyExc_TypeError, "Cannot delete the 'seqnum' attribute");
        return -1;
    }

    unsigned int tmp = PyLong_AsUnsignedLong(value);
    if (PyErr_Occurred()) {
        return -1;
    }
    if (tmp > H9frame::H9FRAME_PRIORITY_MAX_VALUE) {
        PyErr_SetString(PyExc_OverflowError, ("maximum allowed value is " + std::to_string(H9frame::H9FRAME_PRIORITY_MAX_VALUE)).c_str());
        return -1;
    }

    self->frame.priority = H9frame::from_underlying<H9frame::Priority>(tmp);

    return 0;
}

static PyObject* H9Frame_get_type(PyH9frame* self, void* closure) {
    return PyLong_FromUnsignedLong(H9frame::to_underlying(self->frame.type));
}

static int H9Frame_set_type(PyH9frame* self, PyObject* value, void* closure) {
    if (value == NULL) {
        PyErr_SetString(PyExc_TypeError, "Cannot delete the 'seqnum' attribute");
        return -1;
    }

    unsigned int tmp = PyLong_AsUnsignedLong(value);
    if (PyErr_Occurred()) {
        return -1;
    }
    if (tmp > H9frame::H9FRAME_TYPE_MAX_VALUE) {
        PyErr_SetString(PyExc_OverflowError, ("maximum allowed value is " + std::to_string(H9frame::H9FRAME_TYPE_MAX_VALUE)).c_str());
        return -1;
    }

    self->frame.set_type_from_underlying(tmp);
    return 0;
}

static PyObject* H9Frame_get_seqnum(PyH9frame* self, void* closure) {
    return PyLong_FromUnsignedLong(self->frame.seqnum);
}

static int H9Frame_set_seqnum(PyH9frame* self, PyObject* value, void* closure) {
    if (value == NULL) {
        PyErr_SetString(PyExc_TypeError, "Cannot delete the 'seqnum' attribute");
        return -1;
    }

    unsigned int tmp = PyLong_AsUnsignedLong(value);
    if (PyErr_Occurred()) {
        return -1;
    }
    if (tmp > H9frame::H9FRAME_SEQNUM_MAX_VALUE) {
        PyErr_SetString(PyExc_OverflowError, ("maximum allowed value is " + std::to_string(H9frame::H9FRAME_SEQNUM_MAX_VALUE)).c_str());
        return -1;
    }

    self->frame.seqnum = tmp;
    return 0;
}

static PyObject* H9Frame_get_destination_id(PyH9frame* self, void* closure) {
    return PyLong_FromUnsignedLong(self->frame.destination_id);
}

static int H9Frame_set_destination_id(PyH9frame* self, PyObject* value, void* closure) {
    if (value == NULL) {
        PyErr_SetString(PyExc_TypeError, "Cannot delete the 'destination_id' attribute");
        return -1;
    }

    unsigned int tmp = PyLong_AsUnsignedLong(value);
    if (PyErr_Occurred()) {
        return -1;
    }
    if (tmp > H9frame::H9FRAME_DESTINATION_ID_MAX_VALUE) {
        PyErr_SetString(PyExc_OverflowError, ("maximum allowed value is " + std::to_string(H9frame::H9FRAME_DESTINATION_ID_MAX_VALUE)).c_str());
        return -1;
    }

    self->frame.destination_id = tmp;
    return 0;
}

static PyObject* H9Frame_get_source_id(PyH9frame* self, void* closure) {
    return PyLong_FromUnsignedLong(self->frame.source_id);
}

static int H9Frame_set_source_id(PyH9frame* self, PyObject* value, void* closure) {
    if (value == NULL) {
        PyErr_SetString(PyExc_TypeError, "Cannot delete the 'source_id' attribute");
        return -1;
    }

    if (!PyLong_Check(value)) {
        PyErr_SetString(PyExc_TypeError, "The 'source_id' attribute value must be a int");
        return -1;
    }

    unsigned int tmp = PyLong_AsUnsignedLong(value);
    if (PyErr_Occurred()) {
        return -1;
    }
    if (tmp > H9frame::H9FRAME_SOURCE_ID_MAX_VALUE) {
        PyErr_SetString(PyExc_OverflowError, ("maximum allowed value is " + std::to_string(H9frame::H9FRAME_SOURCE_ID_MAX_VALUE)).c_str());
        return -1;
    }

    self->frame.source_id = tmp;
    return 0;
}

static PyObject* H9Frame_get_dlc(PyH9frame* self, void* closure) {
    return PyLong_FromUnsignedLong(self->frame.dlc);
}

static int H9Frame_set_dlc(PyH9frame* self, PyObject* value, void* closure) {
    if (value == NULL) {
        PyErr_SetString(PyExc_TypeError, "Cannot delete the 'dlc' attribute");
        return -1;
    }

    unsigned int tmp = PyLong_AsUnsignedLong(value);
    if (PyErr_Occurred()) {
        return -1;
    }
    if (tmp > 8) {
        PyErr_SetString(PyExc_OverflowError, "maximum allowed value is 8");
        return -1;
    }
    self->frame.dlc = tmp;

    return 0;
}

static PyObject* H9Frame_get_data(PyH9frame* self, void* closure) {
    PyObject* data = PyTuple_New(self->frame.dlc);
    for (int i = 0; i < self->frame.dlc; ++i) {
        PyObject* tmp = PyLong_FromLong(self->frame.data[i]);
        PyTuple_SetItem(data, i, tmp);
    }
    return data;
}

static int H9Frame_set_data(PyH9frame* self, PyObject* value, void* closure) {
    if (value == NULL) {
        PyErr_SetString(PyExc_TypeError, "Cannot delete the 'data' attribute");
        return -1;
    }

    if (!PySequence_Check(value)) {
        PyErr_SetString(PyExc_TypeError, "The 'data' attribute value must be a sequence object (e.g. tuple, list)");
        return -1;
    }

    int len = PySequence_Size(value);
    if (len > H9frame::H9FRAME_DATA_LENGTH) {
        PyErr_SetString(PyExc_OverflowError, "The length of the 'date' attribute must be less or equal than 8.");
        return -1;
    }
    for (int i = 0; i < len; ++i) {
        PyObject* item = PySequence_GetItem(value, i);
        unsigned int tmp = PyLong_AsUnsignedLong(item);
        if (PyErr_Occurred()) {
            self->frame.dlc = 0;
            return -1;
        }
        if (tmp > 255) {
            PyErr_SetString(PyExc_OverflowError, "maximum allowed value is 255");
            self->frame.dlc = 0;
            return -1;
        }
        self->frame.data[i] = tmp;
    }

    self->frame.dlc = len;
    return 0;
}

static PyGetSetDef h9frame_getsetters[] = {
    {"priority", (getter)H9Frame_get_priority, (setter)H9Frame_set_priority, "priority", NULL},
    {"type", (getter)H9Frame_get_type, (setter)H9Frame_set_type, "type", NULL},
    {"seqnum", (getter)H9Frame_get_seqnum, (setter)H9Frame_set_seqnum, "seqnum", NULL},
    {"destination_id", (getter)H9Frame_get_destination_id, (setter)H9Frame_set_destination_id, "destination_id", NULL},
    {"source_id", (getter)H9Frame_get_source_id, (setter)H9Frame_set_source_id, "source_id", NULL},
    {"dlc", (getter)H9Frame_get_dlc, (setter)H9Frame_set_dlc, "dlc", NULL},
    {"data", (getter)H9Frame_get_data, (setter)H9Frame_set_data, "data", NULL},
    {NULL} /* Sentinel */
};

//static PyMemberDef h9frame_members[] = {
//    {"LOW", T_UBYTE, offsetof(PyH9frame, LOW), READONLY, "LOW priority"},
//    {NULL} /* Sentinel */
//};

// static PyObject*
// Custom_name(CustomObject* self, PyObject* Py_UNUSED(ignored)) {
//     return PyUnicode_FromFormat("%S %S", self->first, self->last);
// }
//
// static PyMethodDef Custom_methods[] = {
//     {"name", (PyCFunction)Custom_name, METH_NOARGS,
//      "Return the name, combining the first and last name"},
//     {NULL} /* Sentinel */
// };

PyTypeObject h9frameType = {
    .ob_base = PyVarObject_HEAD_INIT(NULL, 0)
                   .tp_name = "h9.H9Frame",
    .tp_basicsize = sizeof(PyH9frame),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    .tp_doc = PyDoc_STR("H9Frame objects"),
    .tp_getset = h9frame_getsetters,
    .tp_init = (initproc)H9Frame_init,
    .tp_new = PyType_GenericNew,
    //.tp_dealloc = (destructor)Custom_dealloc,
//    .tp_members = h9frame_members,
    //.tp_methods = Custom_methods,
};

// #include <iostream>
static PyObject* h9_send_frame(PyObject* self, PyObject* args) {
    int ret = 0;

    PyH9frame* h9frame = nullptr;
    if (!PyArg_ParseTuple(args, "O!", &h9frameType, &h9frame))
        return NULL;

    VirtualPyNode::send_frame(h9frame->frame);

    return PyLong_FromLong(ret);
}

static PyMethodDef h9_methods[] = {
    {"send_frame", h9_send_frame, METH_VARARGS, "Send frame."},
    {NULL, NULL, 0, NULL} /* Sentinel */
};

static PyModuleDef h9_mmodule = {
    .m_base = PyModuleDef_HEAD_INIT,
    .m_name = "h9",
    .m_doc = "Example module that creates an extension type.",
    .m_size = -1,
    .m_methods = h9_methods,
};

PyMODINIT_FUNC PyInit_h9(void) {
    PyObject* m;
    if (PyType_Ready(&h9frameType) < 0)
        return NULL;

    m = PyModule_Create(&h9_mmodule);
    if (m == NULL)
        return NULL;

    Py_INCREF(&h9frameType);
    if (PyModule_AddObject(m, "H9Frame", (PyObject*)&h9frameType) < 0) {
        Py_DECREF(&h9frameType);
        Py_DECREF(m);
        return NULL;
    }

    PyDict_SetItemString(h9frameType.tp_dict, "PRIORITY_HIGH", PyLong_FromLong(H9frame::to_underlying(H9frame::Priority::HIGH)));
    PyDict_SetItemString(h9frameType.tp_dict, "PRIORITY_LOW", PyLong_FromLong(H9frame::to_underlying(H9frame::Priority::LOW)));

    for (std::underlying_type_t<H9frame::Error> i = 0; i <= H9frame::H9FRAME_TYPE_MAX_VALUE; ++i) {
        const char* tmp = H9frame::error_to_string(H9frame::from_underlying<H9frame::Error>(i));
        if (tmp) {
            std::string name = "ERROR_" + std::string(tmp);
            PyDict_SetItemString(h9frameType.tp_dict, name.c_str(), PyLong_FromLong(i));
        }
    }

    for (std::underlying_type_t<H9frame::Type> i = 0; i <= H9frame::H9FRAME_TYPE_MAX_VALUE; ++i) {
        const char* tmp = H9frame::type_to_string(H9frame::from_underlying<H9frame::Type>(i));
        if (tmp) {
            std::string name = "TYPE_" + std::string(tmp);
            PyDict_SetItemString(h9frameType.tp_dict, name.c_str(), PyLong_FromLong(i));
        }
    }

    return m;
}

PyObject* PyH9Frame_New(const H9frame& frame) {
    PyH9frame* t = PyObject_NEW(PyH9frame, &h9frameType);
    t->frame = frame;
    return reinterpret_cast<PyObject*>(t);
}
