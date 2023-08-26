/*
 * @author 	Alexander Rüedlinger <a.rueedlinger@gmail.com>
 * @date 	26.02.2015
 *
 * Python bindings for the AM2315 driver written in C.
 *
 */

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <structmember.h>
#include "am2315.h"

typedef struct {
	PyObject_HEAD
	void *am2315;
} AM2315_Object;



static void AM2315_dealloc(AM2315_Object *self) {
	am2315_close(self->am2315);
	Py_TYPE(self)->tp_free((PyObject*)self);
}


static PyObject *AM2315_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
	AM2315_Object *self;
	self = (AM2315_Object *) type->tp_alloc(type, 0);
	return (PyObject *) self;
}


static int AM2315_init(AM2315_Object *self, PyObject *args, PyObject *kwds) {
	int address;
	const char *i2c_device;
	static char *kwlist[] = {"address", "i2c_devcie", NULL};

	if(!PyArg_ParseTupleAndKeywords(args, kwds, "is", kwlist, &address, &i2c_device))
		return -1;

	if(i2c_device) {
		self->am2315 = am2315_init(address, i2c_device);
		if(self->am2315 == NULL) {
			PyErr_SetString(PyExc_RuntimeError, "Cannot initialize sensor. Run program as root and check i2c device / address.");
			return -1;
		}
	}

	return 0;
}



static PyObject *AM2315_temperature(AM2315_Object *self) {
	PyObject *result;
	double temperature = am2315_temperature(self->am2315);
	result = PyFloat_FromDouble(temperature);
	return result;
}


static PyObject *AM2315_humidity(AM2315_Object *self) {
	PyObject *result;
	double humidity = am2315_humidity(self->am2315);
	result = PyFloat_FromDouble(humidity);
	return result;
}



static PyObject *AM2315_sense(AM2315_Object *self) {
	float temperature = 0, humidity = 0;
	int crc_check = am2315_read_data(self->am2315, &temperature, &humidity);
	return Py_BuildValue("(ffi)", temperature, humidity, crc_check);
}



static PyMethodDef AM2315_methods[] = {
	{"temperature", (PyCFunction) AM2315_temperature, METH_NOARGS, "Returns a temperature value"},
	{"humidity", (PyCFunction) AM2315_humidity, METH_NOARGS, "Returns a humidity value"},
	{"sense", (PyCFunction) AM2315_sense, METH_NOARGS, "Returns a (temperature, humidity, crc_check) triple"},
	{NULL, NULL, 0, NULL}   /* Sentinel */
};



static PyTypeObject AM2315_Type = {
        PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "tentacle_pi.AM2315",
        .tp_basicsize = sizeof(AM2315_Object),
        .tp_doc = PyDoc_STR("AM2315 objects"),
        .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
        .tp_new = AM2315_new,
        .tp_init = (initproc)AM2315_init,
        .tp_dealloc = (destructor)AM2315_dealloc,
        .tp_methods = AM2315_methods,
};



static struct PyModuleDef AM2315_Module = {
        PyModuleDef_HEAD_INIT,
        "tentacle_pi",       /* m_name */
        "AM2315 com. module",/* m_doc */
        -1,                  /* m_size */
};



PyMODINIT_FUNC PyInit_AM2315(void)
{
	PyObject *m;

	if(PyType_Ready(&AM2315_Type) < 0)
		return NULL;

	m = PyModule_Create(&AM2315_Module);
	if(m == NULL)
		return NULL;

	Py_INCREF(&AM2315_Type);
	if (PyModule_AddObject(m, "AM2315", (PyObject *) &AM2315_Type) < 0) {
		Py_DECREF(&AM2315_Type);
		Py_DECREF(m);
		return NULL;
	}

	return m;
}
