/*
 * H9 project
 *
 * Created by crowx on 2023-09-23.
 *
 * Copyright (C) 2023 Kamil Palkowski. All rights reserved.
 */

#include "py_h9.h"
#include <structmember.h>


// static void Custom_dealloc(CustomObject* self) {
//     Py_XDECREF(self->first);
//     Py_XDECREF(self->last);
//     Py_TYPE(self)->tp_free((PyObject*)self);
// }

// static PyObject* Custom_new(PyTypeObject* type, PyObject* args, PyObject* kwds) {
//     CustomObject* self;
//     self = (CustomObject*)type->tp_alloc(type, 0);
//     if (self != NULL) {
//         self->first = PyUnicode_FromString("");
//         if (self->first == NULL) {
//             Py_DECREF(self);
//             return NULL;
//         }
//         self->last = PyUnicode_FromString("");
//         if (self->last == NULL) {
//             Py_DECREF(self);
//             return NULL;
//         }
//         self->number = 0;
//     }
//     return (PyObject*)self;
// }

// Priority priority;
// Type type;
// std::uint8_t seqnum: H9FRAME_SEQNUM_BIT_LENGTH;
// std::uint16_t destination_id: H9FRAME_DESTINATION_ID_BIT_LENGTH;
// std::uint16_t source_id: H9FRAME_SOURCE_ID_BIT_LENGTH;
// std::uint8_t dlc;
// std::uint8_t data[8]{};

static int H9Frame_init(PyH9frame* self, PyObject* args, PyObject* kwds) {
    static char* kwlist[] = {"priority", "type", "seqnum", "destination_id", "source_id", "dlc", "data", NULL};

    //    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|UUi", kwlist,
    //                                     &first, &last,
    //                                     &self->number))
    //        return -1;
    //
    //    if (first) {
    //        tmp = self->first;
    //        Py_INCREF(first);
    //        self->first = first;
    //        Py_DECREF(tmp);
    //    }
    //    if (last) {
    //        tmp = self->last;
    //        Py_INCREF(last);
    //        self->last = last;
    //        Py_DECREF(tmp);
    //    }
    return 0;
}

// static PyMemberDef Custom_members[] = {
//     {"number", T_INT, offsetof(CustomObject, number), 0,
//      "custom number"},
//     {NULL} /* Sentinel */
// };

static PyObject* H9Frame_getfirst(PyH9frame* self, void* closure) {
    return PyLong_FromLong(self->frame.seqnum);
}

static int H9Frame_setfirst(PyH9frame* self, PyObject* value, void* closure) {
    if (value == NULL) {
        PyErr_SetString(PyExc_TypeError, "Cannot delete the 'seqnum' attribute");
        return -1;
    }

    if (!PyLong_Check(value)) {
        PyErr_SetString(PyExc_TypeError, "The 'seqnum' attribute value must be a int");
        return -1;
    }

    std::uint8_t tmp;
    PyArg_Parse(value, "B", &tmp);
    self->frame.seqnum = tmp;

    return 0;
}

// Priority priority;
// Type type;
// std::uint8_t seqnum: H9FRAME_SEQNUM_BIT_LENGTH;
// std::uint16_t destination_id: H9FRAME_DESTINATION_ID_BIT_LENGTH;
// std::uint16_t source_id: H9FRAME_SOURCE_ID_BIT_LENGTH;
// std::uint8_t dlc;
// std::uint8_t data[8]{};

static PyGetSetDef h9frame_getsetters[] = {
    {"priority", (getter)H9Frame_getfirst, (setter)H9Frame_setfirst, "priority", NULL},
    {"type", (getter)H9Frame_getfirst, (setter)H9Frame_setfirst, "type", NULL},
    {"seqnum", (getter)H9Frame_getfirst, (setter)H9Frame_setfirst, "seqnum", NULL},
    {"destination_id", (getter)H9Frame_getfirst, (setter)H9Frame_setfirst, "destination_id", NULL},
    {"source_id", (getter)H9Frame_getfirst, (setter)H9Frame_setfirst, "source_id", NULL},
    {"dlc", (getter)H9Frame_getfirst, (setter)H9Frame_setfirst, "dlc", NULL},
    {"data", (getter)H9Frame_getfirst, (setter)H9Frame_setfirst, "data", NULL},
    {NULL} /* Sentinel */
};

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
    PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "h9.H9Frame",
    .tp_doc = PyDoc_STR("H9Frame objects"),
    .tp_basicsize = sizeof(PyH9frame),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    .tp_new = PyType_GenericNew,
    .tp_init = (initproc)H9Frame_init,
    //.tp_dealloc = (destructor)Custom_dealloc,
    //.tp_members = Custom_members,
    //.tp_methods = Custom_methods,
    .tp_getset = h9frame_getsetters,
};

static PyModuleDef h9_mmodule = {
    PyModuleDef_HEAD_INIT,
    .m_name = "h9",
    .m_doc = "Example module that creates an extension type.",
    .m_size = -1,
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

    return m;
}

PyObject* PyH9Frame_New(const H9frame& frame) {
    PyH9frame *t = PyObject_NEW(PyH9frame, &h9frameType);
    t->frame = frame;
    return reinterpret_cast<PyObject*>(t);
}
