/************************************************************\
 * Python Module 'Logger' for the Computational Datalogger.
 *
 * The functions in this module are designed for interacting
 * with the attached AVR based datalogger.
\************************************************************/

#include <Python.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>

#define ROM_BUSY   24
#define POWER_GOOD 25

static PyObject* init(PyObject* self, PyObject* args)
{
	wiringPiSetupGpio();			// Setup the wiringPi library to use Broadcom GPIO numbers.
	wiringPiSPISetup(0, 2000000);		// Setup the wiringPi SPI library to use CS 0 @ 2 MHz.

	pinMode(ROM_BUSY, OUTPUT);		// ROM_BUSY Pin output low
	digitalWrite(ROM_BUSY, LOW);

	pinMode(POWER_GOOD, OUTPUT);		// POWER_GOOD Pin output low
	digitalWrite(ROM_BUSY, LOW);

	return Py_None;
}

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
	{ "init", init, METH_NOARGS, "Initializes the Logger Python Module" },
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

