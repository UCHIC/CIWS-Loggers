import matplotlib.pyplot as plt				# This example uses matplotlib for plotting. This module is in no way tied to 
							# matplotlib. You may utilize whichever plotting tool you prefer.
import rawDataProcessor					# The module is called rawDataProcessor.

dataIn = rawDataProcessor.readData("RAW_DATA.CSV")	# Read in the magnetometer data
dataInX = dataIn[0]					# Split the results into X, Y, and Z axis data
dataInY = dataIn[1]
dataInZ = dataIn[2]

dataOutX = rawDataProcessor.process(dataInX)		# Run the X axis data through the processing algorithm. This is the algorithm used
							# on the Residential Datalogger. Feel free to try your own algorithms if needed.

plt.plot(dataOutX[0], '-b');				# Plot the data. List 0 of dataOutX is raw data from the magnetometer
plt.plot(dataOutX[1], '-r');				# List 1 of dataOutX is the raw data with all DC offset removed.
plt.plot(dataOutX[2], '-y');				# List 2 of dataOutX notes when a pulse is counted (1 or 0).
plt.plot(dataOutX[3], '-g');				# List 3 of dataOutX shows the total number of pulses detected.
plt.show()

rawDataProcessor.setHighThreshold(10)			# The algorithm has schmitt trigger thresholds for detecting peaks. Defaults are
rawDataProcessor.setLowThreshold(-10)			# +/- 1
rawDataProcessor.setAlpha(0.5)				# Alpha is a parameter in DC Offset removal. Default is 0.95.
							# The above parameters are also adjustable in the code.

dataOutX = rawDataProcessor.process(dataInX)		# Run the X axis data through the processing algorithm

plt.plot(dataOutX[0], '-b');				# Plot the data
plt.plot(dataOutX[1], '-r');
plt.plot(dataOutX[2], '-y');
plt.plot(dataOutX[3], '-g');
plt.show()
