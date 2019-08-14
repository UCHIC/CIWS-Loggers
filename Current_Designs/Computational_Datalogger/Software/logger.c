/************************************************************\
 * Python Module 'Logger' for the Computational Datalogger.
 *
 * The functions in this module are designed for interacting
 * with the attached AVR based datalogger.
\************************************************************/

#include <Python.h>

static PyObject* setRomBusy(PyObject* self, PyObject* args)
{

}

static PyObject* setPowerGood(PyObject* self, PyObject* args)
{

}

static PyObject* loadData(PyObject* self, PyObject* args)
{

}

static PyObject* setRomFree(PyObject* self, PyObject* args)
{

}

static PyObject* setPowerOff(PyObject* self, PyObject* args)
{

}

static PyMethodDef methods[] = {
        { "setRomBusy", setRomBusy, METH_NOARGS, "Sends a signal to the datalogger that the EEPROM chip is in use" },
        { "setPowerGood", setPowerGood, METH_NOARGS, "Sends a signal to the datalogger that the Pi booted succesfully" },
        { "loadData", loadData, METH_NOARGS, "Reads data from the EEPROM chip and returns a tuple" },
        { "setRomFree", setRomFree, METH_NOARGS, "Sends a signal to the datalogger that the EEPROM chip is not in use" },
        { "setPowerOff", setPowerOff, METH_NOARGS, "Sends a signal to the datalogger that the Pi is shutting down" },
        { NULL, NULL, 0, NULL }
};


PyMODINIT_FUNC initLogger(void)
{
        Py_InitModule3("Logger", methods, "Python Module written in C for interacting with specific external hardware");
}

