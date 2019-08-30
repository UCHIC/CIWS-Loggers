import matplotlib.pyplot as plt				# This example uses matplotlib for plotting. This module is in no way tied to 
							# matplotlib. You may utilize whichever plotting tool you prefer.
import rawDataProcessor					# The module is called rawDataProcessor.

print("==============================")
print("        Example Script        ")
print("==============================")
print("")
print(" Arguments to functions are not displayed.")
print(" See this file for example function usage.")
print("")
print(" 1) Read the data with rawDataProcessor.readData()")
print("    The input to this function is the csv file")
print("    generated by the datalogger running the")
print("    RawDataCollector program.")

dataIn = rawDataProcessor.readData("RAW_DATA.CSV")	# Read in the magnetometer data
dataInX = dataIn[0]					# Split the results into X, Y, and Z axis data
dataInY = dataIn[1]
dataInZ = dataIn[2]

print("")
print(" 2) Process the data with rawDataProcessor.process()")
print("    This processes the data in the same way that")
print("    the Residential Datalogger processes data.")

dataOutX = rawDataProcessor.process(dataInX)		# Run the X axis data through the processing algorithm. This is the algorithm used
							# on the Residential Datalogger. Feel free to try your own algorithms if needed.
print("")
print(" 3) Plot the data.")
print("    We'll be using matplotlib.pyplot. This is")
print("    not a requirement in any way. The blue line")
print("    is the raw unprocessed data.")
print("    The red line is the data after having its DC")
print("    offset removed.")
print("    The yellow line denotes when a pulse gets")
print("    counted.")
print("    The green line shows the total pulse count.")

plt.plot(dataOutX[0], '-b');				# Plot the data. List 0 of dataOutX is raw data from the magnetometer
plt.plot(dataOutX[1], '-r');				# List 1 of dataOutX is the raw data with all DC offset removed.
plt.plot(dataOutX[2], '-y');				# List 2 of dataOutX notes when a pulse is counted (1 or 0).
plt.plot(dataOutX[3], '-g');				# List 3 of dataOutX shows the total number of pulses detected.
plt.show()

print("")
print(" 4) Write the data to a file.")
print("    What good is data if you can't save it?")
print("    Save the data by calling rawDataProcessor.writeData()")

rawDataProcessor.writeData(dataOutX, "X_Data.csv");	# Write the processed data to a file.

print("")
print(" 5) Adjust parameters.")
print("    There are three parameters you can adjust.")
print("    The high and low thresholds are used for")
print("    the schmitt trigger. The red line must")
print("    cross both thresholds for a pulse to be")
print("    counted.")
print("    The parameter alpha is used in removing the")
print("    signal's DC offset. It determines how much")
print("    the previous output should affect the current")
print("    output.")
print("    To see how these change the results, we'll set")
print("    the high and low thresholds to +/-10, and alpha")
print("    to 0.5. The defaults were +/-1 and 0.95.")
rawDataProcessor.setHighThreshold(10)			# The algorithm has schmitt trigger thresholds for detecting peaks. Defaults are
rawDataProcessor.setLowThreshold(-10)			# +/- 1
rawDataProcessor.setAlpha(0.5)				# Alpha is a parameter in DC Offset removal. Default is 0.95.
							# The above parameters are also adjustable in the code.

dataOutX = rawDataProcessor.process(dataInX)		# Run the X axis data through the processing algorithm

print("")
print("    See the difference? Adjusting these parameters")
print("    may be helpful if you're datalogger isn't quite")
print("    reading the signal right.")

plt.plot(dataOutX[0], '-b');				# Plot the data
plt.plot(dataOutX[1], '-r');
plt.plot(dataOutX[2], '-y');
plt.plot(dataOutX[3], '-g');
plt.show()
