/*********************************************************************\
 * Python Module 'rawDataProcessor' for the Computational Datalogger.
 *
 * The functions in this module are designed for processing raw data
 * from the magnetometer using the RawDataCollector program.
\*********************************************************************/

#include <Python.h>

volatile float pos_thresh = 1;				// These parameters are used in the default algorithm, and are adjustable
volatile float neg_thresh = -1;				// using the functions setHighThreshold(), setLowThreshold, and setAlpha()
volatile float a = 0.95;				// In the python module.

/** This is the default algorithm as implemented on the Residential Datalogger. **/
static PyObject* process(PyObject* self, PyObject* args)
{
	float x[] = {0, 0};				// Stores two samples of input data at a time for processing.
	float y[] = {0, 0};				// Stores two samples of input data without any DC offset for processing.
	uint8_t p = 0;					// Denotes when a pulse has been counted (1 = counted, 0 = not counted)
	unsigned int pulseCount = 0;			// Stores the total number of pulses which have been counted.
	uint8_t flag = 1;				// This flag keeps track of which threshold has been crossed for the schitt trigger.

	PyObject* X;					// magData is the input data to the signal processing algorithm
	PyObject* Y = PyList_New(0);			// Y is the output of the portion of the algorithm which removes DC Offset.
	PyObject* P = PyList_New(0);			// P simply marks when a pulse is counted.
	PyObject* PulseCount = PyList_New(0);		// PulseCount shows the number of pulses counted.
	PyObject* MasterCommander = PyList_New(4);	// MasterCommander is a list of the previous four lists.

	if(!PyArg_ParseTuple(args, "O", &X))
	{
		PyErr_SetString(PyExc_TypeError, "Input must be a list.");
		return Py_None;
	}

	PyObject* Iterator = PyObject_GetIter(X);
	if(!Iterator)
	{
		PyErr_SetString(PyExc_TypeError, "Error setting list iterator.");
		return Py_None;
	}

	PyObject* next = PyIter_Next(Iterator);

	while(next != NULL)	// Iterate over the entire list of input data and process it to count pulses.
	{
		/* Collect next data value */
		x[1] = (float)PyFloat_AsDouble(next);

		/* Remove DC Offset */
		y[1] = a*y[0] + x[1] - x[0];
		x[0] = x[1];
		y[0] = y[1];

		/* Schmitt Trigger */
		if(flag)
		{
			if(y[1] > pos_thresh)
			{
				flag = 0;
				p = 1;
				pulseCount += 1;
			}
		}
		else
		{
			if(y[1] < neg_thresh)
			{
				flag = 1;
				p = 0;
			}
		}

		PyList_Append(Y, Py_BuildValue("f", y[1]));			// Append data to the output data lists
		PyList_Append(P, Py_BuildValue("b", p));
		PyList_Append(PulseCount, Py_BuildValue("i", pulseCount));
		next = PyIter_Next(Iterator);					// Increment to the next piece of input data
	}

	PyList_SetItem(MasterCommander, 0, X);					// Tie the data lists together with the MasterCommander list
	PyList_SetItem(MasterCommander, 1, Y);
	PyList_SetItem(MasterCommander, 2, P);
	PyList_SetItem(MasterCommander, 3, PulseCount);

	return MasterCommander;
}

static PyObject* readData(PyObject* self, PyObject* args)
{
	char* filename;
	if(!PyArg_ParseTuple(args, "s", &filename))				// Pull filename from input arguments
	{
		PyErr_SetString(PyExc_TypeError, "Error, expected a string.");
		return Py_None;
	}

	FILE* dataIn;
	int xdata = 0;
	int ydata = 0;
	int zdata = 0;

	PyObject* X_Data = PyList_New(0);
	PyObject* Y_Data = PyList_New(0);
	PyObject* Z_Data = PyList_New(0);
	PyObject* InputData = PyList_New(3);

	if(!(dataIn = fopen(filename, "r")))					// Open raw data file
	{
		PyErr_SetString(PyExc_TypeError, "Could not find file.");
		return Py_None;
	}

	if(fscanf(dataIn, "%*[^\n]\n") == EOF)						// Skip header (one line)
	{
		PyErr_SetString(PyExc_TypeError, "No data in file.");
		return Py_None;
	}

	while(fscanf(dataIn, "%d%*c %d%*c %d", &xdata, &ydata, &zdata) != EOF)	// Read each line (skip commas) until the end of file is found
	{
		PyList_Append(X_Data, Py_BuildValue("i", xdata));			// Append new data to the Python data lists
		PyList_Append(Y_Data, Py_BuildValue("i", ydata));
		PyList_Append(Z_Data, Py_BuildValue("i", zdata));
	}

	PyList_SetItem(InputData, 0, X_Data);					// Tie the data lists together with the InputData list.
	PyList_SetItem(InputData, 1, Y_Data);
	PyList_SetItem(InputData, 2, Z_Data);

	fclose(dataIn);

	return InputData;
}

static PyObject* setHighThreshold(PyObject* self, PyObject* args)
{
	if(!PyArg_ParseTuple(args, "f", &pos_thresh))
	{
		PyErr_SetString(PyExc_TypeError, "Expected a float.");
	}

	return Py_None;
}

static PyObject* setLowThreshold(PyObject* self, PyObject* args)
{
	if(!PyArg_ParseTuple(args, "f", &neg_thresh))
	{
		PyErr_SetString(PyExc_TypeError, "Expected a float.");
	}

	return Py_None;
}

static PyObject* setAlpha(PyObject* self, PyObject* args)
{
	if(!PyArg_ParseTuple(args, "f", &a))
	{
		PyErr_SetString(PyExc_TypeError, "Expected a float.");
	}

	return Py_None;
}

static PyObject* writeData(PyObject* self, PyObject* args)
{
	PyObject* DataList;						// DataList will hold the 2D list of processed data.
	char* filename;							// filname will hold the name of the file to write DataList to.
	FILE* dataOut;							// Pointer to the output file.

	if(!PyArg_ParseTuple(args, "Os", &DataList, &filename))		// Retrieve the data and put it into DataList.
	{
		PyErr_SetString(PyExc_TypeError, "Expected a list and a string.");	// If the operation fails, set an error and return.
		return PyString_FromString("Bad arguments");
	}

	PyObject* RawDataList = PyList_GetItem(DataList, 0);		// Unpack the 2D list DataList into four separate lists
	PyObject* NoDC_DataList = PyList_GetItem(DataList, 1);
	PyObject* PulseDataList = PyList_GetItem(DataList, 2);
	PyObject* PulseCountList = PyList_GetItem(DataList, 3);

	PyObject* IteratorRaw = PyObject_GetIter(RawDataList);		// Create interators to loop through each list in DataList
	PyObject* IteratorNoDC = PyObject_GetIter(NoDC_DataList);
	PyObject* IteratorPulse = PyObject_GetIter(PulseDataList);
	PyObject* IteratorCount = PyObject_GetIter(PulseCountList);
 
	if(!IteratorRaw || !IteratorNoDC || !IteratorPulse || !IteratorCount)
	{
		PyErr_SetString(PyExc_TypeError, "Error setting list iterators.");
		return PyString_FromString("Error setting list iterators.");
	}

	dataOut = fopen(filename, "w");
	if(dataOut == NULL)						// Create new data file
	{
		PyErr_SetString(PyExc_TypeError, "Could not create file.");
		return PyString_FromString("Could not create file.");
	}

	fprintf(dataOut, "Raw_signal,Signal_with_offset_removed,Schmitt_trigger_output,Pulses_counted\n");	// Write a header to the file

	PyObject* nextRaw = PyIter_Next(IteratorRaw);			// Each next basically holds the current value in its respective
	PyObject* nextNoDC = PyIter_Next(IteratorNoDC);			// list.
	PyObject* nextPulse = PyIter_Next(IteratorPulse);
	PyObject* nextCount = PyIter_Next(IteratorCount);

	while(nextCount != NULL)					// Iterate over the entire DataList
	{
		fprintf(dataOut, "%f,%f,%f,%f\n", PyFloat_AsDouble(nextRaw), PyFloat_AsDouble(nextNoDC), PyFloat_AsDouble(nextPulse), PyFloat_AsDouble(nextCount));
		nextRaw = PyIter_Next(IteratorRaw);
		nextNoDC = PyIter_Next(IteratorNoDC);
		nextPulse = PyIter_Next(IteratorPulse);
		nextCount = PyIter_Next(IteratorCount);
	}
	fclose(dataOut);

	return PyString_FromString("writeData() Returned with no errors");
}

static PyMethodDef methods[] = {
	{ "process", process, METH_VARARGS, "Process Data as if it were the datalogger." },
	{ "readData", readData, METH_VARARGS, "Reads data from a .csv file generated by a datalogger running the rawDataCollector program."},
	{ "setHighThreshold", setHighThreshold, METH_VARARGS, "Sets high threshold for the default built-in data processing algorithm." },
	{ "setLowThreshold", setLowThreshold, METH_VARARGS, "Sets low threshold for the default built-in data processing algorithm." },
	{ "setAlpha", setAlpha, METH_VARARGS, "Sets the alpha parameter of the data processing algorithm." },
	{ "writeData", writeData, METH_VARARGS, "Writes processed data to an output file." },
	{ NULL, NULL, 0, NULL }
};

PyMODINIT_FUNC initrawDataProcessor(void)
{
        Py_InitModule3("rawDataProcessor", methods, "Python Module written in C for processing raw magnetometer data");
}
