/*
 * @author 	Alexander Rüedlinger <a.rueedlinger@gmail.com>
 * @date 	26.02.2015
 * 
 * Python bindings for the AM2315 driver written in C.
 * 
 */
 
#include <Python.h>
#include <structmember.h>
#include "am2315.h"

typedef struct {
	PyObject_HEAD
	void *am2315;
} AM2315_Object;



static void AM2315_dealloc(AM2315_Object *self) {
	am2315_close(self->am2315);
	self->am2315 = NULL;
	self->ob_type->tp_free((PyObject*)self);
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
		
	if(i2c_device) 
		self->am2315 = am2315_init(address, i2c_device);

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
	{"temperature", (PyCFunction) AM2315_temperature, METH_NOARGS, "Return the temperature value"},
	{"humidity", (PyCFunction) AM2315_humidity, METH_NOARGS, "Return the altitude value"},
	{"sense", (PyCFunction) AM2315_sense, METH_NOARGS, "Return the pressure value"},
	{NULL}  /* Sentinel */
};



static PyMemberDef AM2315_members[] = {
    {NULL}  /* Sentinel */
};



static PyTypeObject AM2315_Type = {
	PyObject_HEAD_INIT(NULL)
	0,                         /*ob_size*/
	"AM2315.AM2315",             /*tp_name*/
	sizeof(AM2315_Object),             /*tp_basicsize*/
	0,                         /*tp_itemsize*/
	(destructor)AM2315_dealloc, /*tp_dealloc*/
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
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
	"AM2315 objects",           /* tp_doc */
	0,		               /* tp_traverse */
	0,		               /* tp_clear */
	0,		               /* tp_richcompare */
	0,		               /* tp_weaklistoffset */
	0,		               /* tp_iter */
	0,		               /* tp_iternext */
	AM2315_methods,             /* tp_methods */
	AM2315_members,             /* tp_members */
	0,                         /* tp_getset */
	0,                         /* tp_base */
	0,                         /* tp_dict */
	0,                         /* tp_descr_get */
	0,                         /* tp_descr_set */
	0,                         /* tp_dictoffset */
	(initproc)AM2315_init,      /* tp_init */
	0,                         /* tp_alloc */
	AM2315_new,                 /* tp_new */
};



static PyMethodDef module_methods[] = {
	{NULL}  /* Sentinel */
};



PyMODINIT_FUNC initAM2315(void) {
	PyObject *m;
	
	if(PyType_Ready(&AM2315_Type) < 0)
		return;
		
	m = Py_InitModule3("AM2315", module_methods, "AM2315 extension module");
	
	if(m == NULL)
		return;
		
	Py_INCREF(&AM2315_Type);
	PyModule_AddObject(m, "AM2315", (PyObject *)&AM2315_Type);
}
