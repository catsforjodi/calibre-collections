#define UNICODE
#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <unicode/utypes.h>
#include <unicode/uclean.h>
#include <unicode/ucol.h>
#include <unicode/ustring.h>


// Collator object definition {{{
typedef struct {
    PyObject_HEAD
    // Type-specific fields go here.
    UCollator *collator;

} icu_Collator;

static void
icu_Collator_dealloc(icu_Collator* self)
{
    if (self->collator != NULL) ucol_close(self->collator);
    self->collator = NULL;
    self->ob_type->tp_free((PyObject*)self);
}

static PyObject *
icu_Collator_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    icu_Collator *self;
    const char *loc;
    UErrorCode status = U_ZERO_ERROR;

    if (!PyArg_ParseTuple(args, "s", &loc)) return NULL;

    self = (icu_Collator *)type->tp_alloc(type, 0);
    if (self != NULL) {
        self->collator = ucol_open(loc, &status);
        if (self->collator == NULL || U_FAILURE(status)) { 
            PyErr_SetString(PyExc_Exception, "Failed to create collator.");
            self->collator = NULL;
            Py_DECREF(self);
            return NULL;
        }
    }

    return (PyObject *)self;
}

// Collator.display_name {{{
static PyObject *
icu_Collator_display_name(icu_Collator *self, void *closure) {
    const char *loc = NULL;
    UErrorCode status = U_ZERO_ERROR;
    UChar dname[400];
    char buf[100];

    loc = ucol_getLocaleByType(self->collator, ULOC_ACTUAL_LOCALE, &status);
    if (loc == NULL || U_FAILURE(status)) {
        PyErr_SetString(PyExc_Exception, "Failed to get actual locale"); return NULL;
    }
    ucol_getDisplayName(loc, "en", dname, 100, &status);
    if (U_FAILURE(status)) return PyErr_NoMemory();

    u_strToUTF8(buf, 100, NULL, dname, -1, &status);
    if (U_FAILURE(status)) {
        PyErr_SetString(PyExc_Exception, "Failed ot convert dname to UTF-8"); return NULL;
    }
    return Py_BuildValue("s", buf);
}

// }}}

// Collator.actual_locale {{{
static PyObject *
icu_Collator_actual_locale(icu_Collator *self, void *closure) {
    const char *loc = NULL;
    UErrorCode status = U_ZERO_ERROR;

    loc = ucol_getLocaleByType(self->collator, ULOC_ACTUAL_LOCALE, &status);
    if (loc == NULL || U_FAILURE(status)) {
        PyErr_SetString(PyExc_Exception, "Failed to get actual locale"); return NULL;
    }
    return Py_BuildValue("s", loc);
}

// }}}

// Collator.sort_key {{{
static PyObject *
icu_Collator_sort_key(icu_Collator *self, PyObject *args, PyObject *kwargs) {
    char *input;
    Py_ssize_t sz;
    UChar *buf;
    uint8_t *buf2;
    PyObject *ans;
    int32_t key_size;
    UErrorCode status = U_ZERO_ERROR;
  
    if (!PyArg_ParseTuple(args, "es", "UTF-8", &input)) return NULL;

    sz = strlen(input);

    buf = (UChar*)calloc(sz*4 + 1, sizeof(UChar));

    if (buf == NULL) return PyErr_NoMemory();

    u_strFromUTF8(buf, sz*4 + 1, &key_size, input, sz, &status);
    PyMem_Free(input);

    if (U_SUCCESS(status)) {
        buf2 = (uint8_t*)calloc(7*sz+1, sizeof(uint8_t));
        if (buf2 == NULL) return PyErr_NoMemory();

        key_size = ucol_getSortKey(self->collator, buf, -1, buf2, 7*sz+1);

        if (key_size == 0) {
            ans = PyBytes_FromString("");
        } else {
            if (key_size >= 7*sz+1) {
                free(buf2);
                buf2 = (uint8_t*)calloc(key_size+1, sizeof(uint8_t));
                if (buf2 == NULL) return PyErr_NoMemory();
                ucol_getSortKey(self->collator, buf, -1, buf2, key_size+1);
            }
            ans = PyBytes_FromString((char *)buf2);
        }
        free(buf2);
    } else ans = PyBytes_FromString("");

    free(buf);
    if (ans == NULL) return PyErr_NoMemory();

    return ans;
}

static PyMethodDef icu_Collator_methods[] = {
    {"sort_key", (PyCFunction)icu_Collator_sort_key, METH_VARARGS,
     "sort_key(unicode object) -> Return a sort key for the given object as a bytestring. The idea is that these bytestring will sort using the builtin cmp function, just like the original unicode strings would sort in the current locale with ICU."
    },

    {NULL}  /* Sentinel */
};

static PyGetSetDef  icu_Collator_getsetters[] = {
    {(char *)"actual_locale", 
     (getter)icu_Collator_actual_locale, NULL,
     (char *)"Actual locale used by this collator.",
     NULL},

    {(char *)"display_name", 
     (getter)icu_Collator_display_name, NULL,
     (char *)"Display name of this collator in English. The name reflects the actual data source used.",
     NULL},

    {NULL}  /* Sentinel */
};

static PyTypeObject icu_CollatorType = { // {{{
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "icu.Collator",            /*tp_name*/
    sizeof(icu_Collator),      /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)icu_Collator_dealloc, /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    0,                         /*tp_repr*/
    0,                         /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    0,                         /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE,        /*tp_flags*/
    "Collator",                  /* tp_doc */
    0,		               /* tp_traverse */
    0,		               /* tp_clear */
    0,		               /* tp_richcompare */
    0,		               /* tp_weaklistoffset */
    0,		               /* tp_iter */
    0,		               /* tp_iternext */
    icu_Collator_methods,             /* tp_methods */
    0,             /* tp_members */
    icu_Collator_getsetters,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    0,      /* tp_init */
    0,                         /* tp_alloc */
    icu_Collator_new,                 /* tp_new */
}; // }}}

// }}


// }}}

// }}}

// Module initialization {{{

static PyMethodDef icu_methods[] = {
    {NULL}  /* Sentinel */
};


PyMODINIT_FUNC
initicu(void) 
{
    PyObject* m;
    UErrorCode status = U_ZERO_ERROR;

    u_init(&status);


    if (PyType_Ready(&icu_CollatorType) < 0)
        return;

    m = Py_InitModule3("icu", icu_methods,
                       "Wrapper for the ICU internationalization library");

    Py_INCREF(&icu_CollatorType);
    PyModule_AddObject(m, "Collator", (PyObject *)&icu_CollatorType);
    // uint8_t must be the same size as char
    PyModule_AddIntConstant(m, "ok", (U_SUCCESS(status) && sizeof(uint8_t) == sizeof(char)) ? 1 : 0);

}
// }}}
