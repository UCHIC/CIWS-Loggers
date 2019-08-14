/************************************************************\
 * Python Module 'Logger' for the Computational Datalogger.
 *
 * The functions in this module are designed for interacting
 * with the attached AVR based datalogger.
\************************************************************/

#include <Python.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>

#define ROM_BUSY    24
#define POWER_GOOD  25
#define BUFFER_MAX  21600
#define HEADER_SIZE 9

static PyObject* init(PyObject* self, PyObject* args)
{
	wiringPiSetupGpio();			// Setup the wiringPi library to use Broadcom GPIO numbers.
	wiringPiSPISetup(0, 2000000);		// Setup the wiringPi SPI library to use CS 0 @ 2 MHz.

	pinMode(ROM_BUSY, OUTPUT);		// ROM_BUSY Pin output low
	digitalWrite(ROM_BUSY, LOW);

	pinMode(POWER_GOOD, OUTPUT);		// POWER_GOOD Pin output low
	digitalWrite(POWER_GOOD, LOW);

	return Py_None;
}

static PyObject* bufferMax(PyObject* self, PyObject* args)
{
	return Py_BuildValue("i", BUFFER_MAX);
}

static PyObject* setRomBusy(PyObject* self, PyObject* args)
{
	digitalWrite(ROM_BUSY, HIGH);

	return Py_None;
}

static PyObject* setPowerGood(PyObject* self, PyObject* args)
{
	digitalWrite(POWER_GOOD, HIGH);

	return Py_None;
}

static PyObject* loadData(PyObject* self, PyObject* args)
{
	unsigned int dataSize = BUFFER_MAX + HEADER_SIZE + 4;
	unsigned char data[dataSize];
	data[0] = 0x03;
	data[1] = 0x00;
	data[2] = 0x00;
	data[3] = 0x00;

	wiringPiSPIDataRW(0, data, dataSize);

	unsigned int recordNum = (data[4] << 16) + (data[5] << 8) + data[6];

	unsigned char yearStart = data[7];
	unsigned char monthStart = data[8];
	unsigned char dayStart = data[9];
	unsigned char hourStart = data[10];
	unsigned char minuteStart = data[11];
	unsigned char secondStart = data[12];

	unsigned int lastIndex = recordNum + 13;

	PyObject* dataTuple = PyTuple_New(recordNum);

	PyTuple_SetItem(dataTuple, 0, recordNum);

	unsigned int i;
	for(i = 7; i < lastIndex; i++)
	{
		PyTuple_SetItem(dataTuple, i - 6, data[i]);
	}

	return dataTuple;
}

static PyObject* setRomFree(PyObject* self, PyObject* args)
{
	digitalWrite(ROM_BUSY, LOW);

	return Py_None;
}

static PyObject* setPowerOff(PyObject* self, PyObject* args)
{
	digitalWrite(POWER_GOOD, LOW);

	return Py_None;
}

static PyMethodDef methods[] = {
	{ "init", init, METH_NOARGS, "Initializes the Logger Python Module" },
	{ "bufferMax", bufferMax, METH_NOARGS, "Returns the maximum buffer size from the datalogger" },
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

