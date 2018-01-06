// python includes
#include <Python.h>

#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include "numpy/arrayobject.h"

// local includes
#include "pyref.h"

// k273 includes
#include <k273/util.h>
#include <k273/logging.h>
#include <k273/strutils.h>
#include <k273/exception.h>

// ggplib imports
#include <statemachine/goalless_sm.h>
#include <statemachine/combined.h>
#include <statemachine/statemachine.h>
#include <statemachine/propagate.h>
#include <statemachine/legalstate.h>

// std c++ includes
#include <string>

///////////////////////////////////////////////////////////////////////////////
// global variable

PyObject* ggpzero_interface_error;

///////////////////////////////////////////////////////////////////////////////

class Abc {
public:
    Abc() {
        K273::l_warning("Abc");
    }
};

///////////////////////////////////////////////////////////////////////////////

struct PyObject_AbcWrap {
    PyObject_HEAD
    Abc* impl;
};

///////////////////////////////////////////////////////////////////////////////

static PyObject* AbcWrap_hello(PyObject_AbcWrap* self, PyObject* args) {
    const char* name = nullptr;
    if (! ::PyArg_ParseTuple(args, "s", &name)) {
        return nullptr;
    }

    std::string msg = K273::fmtString("Hello world %s", name);

    return ::Py_BuildValue("s", msg.c_str());
}

static PyObject* AbcWrap_add(PyObject_AbcWrap* self, PyObject* args) {
    static long data[4] = {0,1,2,3};

    const int ND = 2;
    const int SIZE = 2;
    npy_intp dims[2]{SIZE, SIZE};

    return PyArray_SimpleNewFromData(ND, dims, NPY_INT64, &data);
}

static PyObject* AbcWrap_test_sm(PyObject_AbcWrap* self, PyObject* args) {
    ssize_t ptr = 0;

    if (! ::PyArg_ParseTuple(args, "n", &ptr)) {
        return nullptr;
    }

    GGPLib::StateMachine* sm = reinterpret_cast<GGPLib::StateMachine*> (ptr);
    return PyString_FromString(sm->getGDL(42));
}

static struct PyMethodDef AbcWrap_methods[] = {
    {"hello", (PyCFunction) AbcWrap_hello, METH_VARARGS, "blablabla"},
    {"add", (PyCFunction) AbcWrap_add, METH_VARARGS, "blablabla"},
    {"test_sm", (PyCFunction) AbcWrap_test_sm, METH_VARARGS, "blablabla"},
    {nullptr, nullptr}            /* Sentinel */
};

static void AbcWrap_dealloc(PyObject* ptr);


static PyTypeObject PyType_AbcWrap = {
    PyVarObject_HEAD_INIT(nullptr, 0)
    "AbcWrap",                /*tp_name*/
    sizeof(PyObject_AbcWrap), /*tp_size*/
    0,                        /*tp_itemsize*/

    /* methods */
    AbcWrap_dealloc,    /*tp_dealloc*/
    0,                  /*tp_print*/
    0,                  /*tp_getattr*/
    0,                  /*tp_setattr*/
    0,                  /*tp_compare*/
    0,                  /*tp_repr*/
    0,                  /*tp_as_number*/
    0,                  /*tp_as_sequence*/
    0,                  /*tp_as_mapping*/
    0,                  /*tp_hash*/
    0,                  /*tp_call*/
    0,                  /*tp_str*/
    0,                  /*tp_getattro*/
    0,                  /*tp_setattro*/
    0,                  /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT, /*tp_flags*/
    0,                  /*tp_doc*/
    0,                  /*tp_traverse*/
    0,                  /*tp_clear*/
    0,                  /*tp_richcompare*/
    0,                  /*tp_weaklistoffset*/
    0,                  /*tp_iter*/
    0,                  /*tp_iternext*/
    AbcWrap_methods,    /* tp_methods */
    0,                  /* tp_members */
    0,                  /* tp_getset */
};


static PyObject_AbcWrap* PyType_AbcWrap_new(Abc* impl) {
    PyObject_AbcWrap* res = PyObject_New(PyObject_AbcWrap, &PyType_AbcWrap);
    res->impl = impl;
    return res;
}


static void AbcWrap_dealloc(PyObject* ptr) {
    K273::l_debug("--> AbcWrap_dealloc");
    ::PyObject_Del(ptr);
}


///////////////////////////////////////////////////////////////////////////////

static PyObject* ggpzero_interface_start(PyObject* self, PyObject* args) {
    int x = 0;
    PyObject* obj = nullptr;

    if (! ::PyArg_ParseTuple(args, "iO", &x, &obj)) {
        return nullptr;
    }

    K273::loggerSetup("XX", K273::Logger::LOG_VERBOSE);

    Abc* abc_test = new Abc;
    return (PyObject *) PyType_AbcWrap_new(abc_test);
}

///////////////////////////////////////////////////////////////////////////////

PyMethodDef ggpzero_interface_functions[] = {
    {"start", ggpzero_interface_start, METH_VARARGS, "start blabla"},
    {nullptr, nullptr, 0, nullptr}
};


///////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif

    PyMODINIT_FUNC initggpzero_interface() {
        Py_TYPE(&PyType_AbcWrap) = &PyType_Type;

        if (::PyType_Ready(&PyType_AbcWrap) < 0) {
            return;
        }

        PyObject* m = ::Py_InitModule("ggpzero_interface", ggpzero_interface_functions);
        if (m == nullptr) {
            return;
        }

        char error_name[] = "ggpzero_interface.error";
        ggpzero_interface_error = ::PyErr_NewException(error_name, nullptr, nullptr);
        Py_INCREF(ggpzero_interface_error);
        ::PyModule_AddObject(m, "AbcModuleError", ggpzero_interface_error);

        import_array();
    }
}

