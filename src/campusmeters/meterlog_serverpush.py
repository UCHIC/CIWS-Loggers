# ----------------------------------------------------------------------------
# meterlog.py
# Author: Jeff Horsburgh
# Creation Date: 7/22/2016
# Development Environment:
#   PyCharm 5.0.5
#   Python 2.7
# Hardware Platform:
#   Raspberry Pi 3 Model B
#   Adafruit ADS1015 ADC
#
# This script makes high frequency measurements of flow through water meters
# installed on USU's student residence hall buildings. It measures the
# 4-20 mA output of the meters and converts it to gallons per minute. It
# outputs flow rate, the incremental flow for each time period, and totalized
# flow since the program started.
#
# The sampling rate and data recording rate are configurable
#
# TODO:
# 1. Build program for multi meter sites
# ----------------------------------------------------------------------------

import time
# Import the ADS1x15 module.
import Adafruit_ADS1x15
import sys
import protopushrpi

# Set all site specific configuration options for deployment
# ----------------------------------------------------------
siteCode = "LLC_BLDG_A_HotSupply"
# Set the scanning and recording intervals at which the program should run
scanInterval = 1.0          # Time between scans within the main loop in seconds
recordInterval = 30.0       # Time between recorded values in seconds
# Set meter Specific Values
maxFlowRate = 50.0          # Maximum flow rate of meter at 20 mA output (gal/min)
calibrationFactor = 1.0     # A value to scale the output voltages based on simple one point calibration

# Create an ADS1015 ADC (12-bit) instance with default address
# ------------------------------------------------------------
# Note: the I2C address can be changed from its default address (0x48), and/or
# the I2C bus can be changed by passing in these optional parameters:
# adc = Adafruit_ADS1x15.ADS1015(address=0x49, busnum=1)
adc = Adafruit_ADS1x15.ADS1015()

# Choose a gain setting for the ADC
# ---------------------------------
# This setting changes the range of voltages that are read
# See table 3 in the ADS1015/ADS1115 datasheet for more info on gain.
#  - 2/3 = +/-6.144V 1 bit = 3 mV
#  -   1 = +/-4.096V 1 bit = 2 mV
#  -   2 = +/-2.048V 1 bit = 1 mV
#  -   4 = +/-1.024V 1 bit = 0.5 mV
#  -   8 = +/-0.512V 1 bit = 0.25 mV
#  -  16 = +/-0.256V 1 bit = 0.125 mV
# GAIN = 2/3
# bitConversionFactor = 3  # mV per bit per the table above
GAIN = 1
bitConversionFactor = 2  # mV per bit per the table above

# Get the meter reading at the time of the deployment passed as a command line argument
initialMeterReading = sys.argv[1]

# Set up the data log file
# ------------------------
localTime = time.localtime(time.time())
outputFilePath = '/home/pi/CampusMeters/'
outputFileName = 'datalog_' + siteCode + '_' + str(localTime.tm_year) + '-' + str(localTime.tm_mon) \
                 + '-' + str(localTime.tm_mday) + '_' + str(localTime.tm_hour) + '-' \
                 + str(localTime.tm_min) + '-' + str(localTime.tm_sec) + '.csv'
# Data file header lines - these lines get written to the beginning of the file when it's created
loggerName = '"Campus Meter Data Log for ' + siteCode + '\"'
print loggerName
meterReading = '"Meter reading at time of deployment: ' + initialMeterReading + '\"'
print meterReading
# Data file column header
dataHeader = '"Date\",\"RecordNumber\",\"SensorVoltage\",\"FlowRate\", \"AvgFlowRate\", ' \
             '\"IncrementalVolume\", \"TotalizedVolume\"'
print dataHeader
# Open the file for writing to make sure the file gets created, write the header, then close it
#with open(outputFilePath + outputFileName, 'w') as outputFile:
#    outputFile.write(loggerName + '\n')
#    outputFile.write(meterReading + '\n')
#    outputFile.write(dataHeader + '\n')

# Initialize timing variables
# --------------------
sampleNum = 1             # Initialize the sample number to 1
currTime = 0.0            # Timing variable
previousTime = 0.0        # Timing variable
previousRecordTime = 0.0  # Timing variable
timeInterval = 0.0        # Timing Variable
serverpusharray = []
serverpushcount = 0

# Define sensor related values
# ----------------------------
flowPin = 0                 # Analog port on the ADC to which the meter is connected
resistorValue = 200.0       # The value of the shunt resistor in ohms
minVoltage = (0.004 * resistorValue)  # The minimum voltage value that can be measured based on Ohm's Law
maxVoltage = (0.020 * resistorValue)  # The maximum voltage that can be measured
voltageRange = maxVoltage - minVoltage  # Voltage range based on resistor value in volts
sensorVolts = 0.0           # Voltage of the analog input from the meter
flowRate = 0.0              # Instantaneous flow rate measured from the meter (gal/min)
avgFlowRate = 0.0           # Calculated average flow rate over the recording interval (gal/min)
flowSum = 0.0               # A sum variable for calculating the average flow
sampleCount = 0             # The number of samples collected within the recording interval
scanIntervalVolume = 0.0    # Total flow volume for the current scan interval (gal)
recordIntervalVolume = 0.0  # Total flow volume for the current recording interval (gal)
totalizedVolume = 0.0       # Total flow volume from the meter since the program started (gal)

#Set ADC in continuous mode
adc.start_adc(flowPin, gain=GAIN, data_rate=490)

# Delay the start of the program execution so the times recorded with
# measurements will be even divisions of minutes
# -------------------------------------------------------------------
now = time.localtime(time.time())
remainder = recordInterval - (now.tm_sec % recordInterval)
time.sleep(remainder)
previousTime = time.time()
previousRecordTime = previousTime

# Main program loop
# -----------------
while True:
    # Get the current time on the logger at the beginning of the measurement loop
    currTime = time.time()

    # Check timing to determine if the scan interval has been reached and measurements should be made
    if (currTime - previousTime) >= scanInterval:
        # Increment the sample counter
        sampleCount += 1

        # Get a correctly formatted date string
        now = time.strftime("\"%Y-%m-%d %H:%M:%S\"", time.localtime(currTime))

        # Perform Measurements
        # --------------------
        # Get the voltage measurement from the meter - divide by 1000 to convert from mV to V
        #Choose continuous or on request mode
        #sensorVolts = calibrationFactor * bitConversionFactor * adc.read_adc(flowPin, gain=GAIN, data_rate=3300) / 1000
        sensorVolts = calibrationFactor * bitConversionFactor * adc.get_last_result() / 1000
        # Check to see if the output voltage is greater than the minimum - if not, set flow to zero
        # Add an arbitrary buffer onto the minVoltage (0.0001 V) to avoid small errors when the flow is zero
        if sensorVolts > (minVoltage + 0.0001):
            flowRate = (maxFlowRate * (sensorVolts - minVoltage)) / voltageRange  # gallons per minute
        else:
            flowRate = 0.0

        #Rounding flow rate to the form x.yx to match with meter display
        flowRate = round(flowRate, 2)

        #Adding calibration, offset to match with meter display (location spectific)
        flowRate = flowRate + 0.04

        # Calculate the derived values
        timeInterval = (currTime - previousTime)               # Time of scan interval in seconds
        scanIntervalVolume = (flowRate * timeInterval) / 60.0  # Total flow for scan interval (gal)
        recordIntervalVolume += scanIntervalVolume             # Total flow for record interval (gal)
        totalizedVolume += scanIntervalVolume                  # Total flow since the current program started (gal)
        flowSum += flowRate                                    # Sum variable for calculating average flow
        avgFlowRate = flowSum / sampleCount                    # Average flow rate (gal/min)

        # Create a data record string
        outputList = [now, sampleNum, sensorVolts, flowRate, avgFlowRate, recordIntervalVolume, totalizedVolume]
        dataRecord = ','.join(map(str, outputList))

        # Print the current data record to the console for monitoring
        print dataRecord

        # Remember the time at which the current measurement started
        previousTime = currTime

        # Check to see if it's time to record data/ push to server and if so, write/ push data
        if (currTime - previousRecordTime) >= recordInterval:
            # Save the data record to the output file
            #with open(outputFilePath + outputFileName, 'a') as outputFile:
            #    outputFile.write(dataRecord + '\n')
		
            # Array to push datavalues to server
            if (serverpushcount == 0) or (serverpushcount == 1):
                serverpushcount += 1
                serverpusharray.append(now + "::" + str(avgFlowRate))
            
            if serverpushcount == 2:
                serverpusharray = [x.replace('"', "") for x in serverpusharray] #Bad hack to remove d-quotes from dateobj
                protopushrpi.serverpush(serverpusharray)
                serverpushcount = 0
                serverpusharray = []
            
            # Remember the time at which the record was written to the file
            previousRecordTime = currTime

            # Increment the recorded sample number and reset the average flow rate variables and record interval flow
            sampleNum += 1
            recordIntervalVolume = 0.0
            flowSum = 0.0
            avgFlowRate = 0.0
            sampleCount = 0.0

adc.stop_adc()
