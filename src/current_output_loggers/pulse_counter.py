# ----------------------------------------------------------------------------
# pulse_counter.py
# Author: Jeff Horsburgh
# Creation Date: 7/25/2016
# Development Environment:
#   PyCharm 5.0.5
#   Python 2.7
# Hardware Platform:
#   Raspberry Pi 3 Model B
#
# This script makes high frequency measurements of flow through a pulse
# counting water meter. It measures the pulsed output from the meter and
# converts it to gallons per minute. It outputs flow rate, the incremental
# flow for each time period, and totalized flow since the program started.
#
# The sampling rate and data recording rate are configurable
#
# ----------------------------------------------------------------------------
import time
import sys
import RPi.GPIO as GPIO

# Set all site specific configuration options for deployment
# ----------------------------------------------------------
siteCode = "LLC_BLDG_A_HotReturn"
# Set the scanning and recording intervals at which the program should run
scanInterval = 1.0          # Time between scans within the main loop in seconds
recordInterval = 10.0       # Time between recorded values in seconds

# Set up the GPIO pin for the pulse counting meter
# ------------------------------------------------
GPIOPin = 26
GPIO.setmode(GPIO.BCM)
GPIO.setup(GPIOPin, GPIO.IN)

# Get the meter reading at the time of the deployment passed as a command line argument
initialMeterReading = sys.argv[1]

# Set up the data log file
# ------------------------
localTime = time.localtime(time.time())
outputFilePath = '/home/pi/CampusMeters/'
outputFileName = 'pulse_datalog_' + siteCode + '_' + str(localTime.tm_year) + '-' + str(localTime.tm_mon) \
                 + '-' + str(localTime.tm_mday) + '_' + str(localTime.tm_hour) + '-' \
                 + str(localTime.tm_min) + '-' + str(localTime.tm_sec) + '.csv'
# Data file header lines - these lines get written to the beginning of the file when it's created
loggerName = '"Campus Meter Data Log for ' + siteCode + '\"'
print loggerName
meterReading = '"Meter reading at time of deployment: ' + initialMeterReading + '\"'
print meterReading
# Data file column header
dataHeader = '"Date\",\"RecordNumber\",\"pulseCount\",\"FlowRate\", \"AvgFlowRate\", ' \
             '\"IncrementalVolume\", \"TotalizedVolume\"'
print dataHeader
# Open the file for writing to make sure the file gets created, write the header, then close it
with open(outputFilePath + outputFileName, 'w') as outputFile:
    outputFile.write(loggerName + '\n')
    outputFile.write(meterReading + '\n')
    outputFile.write(dataHeader + '\n')

# Initialize timing variables
# --------------------
sampleNum = 1             # Initialize the sample number to 1
currTime = 0.0            # Timing variable
previousTime = 0.0        # Timing variable
previousRecordTime = 0.0  # Timing variable
timeInterval = 0.0        # Timing Variable

# Define sensor related values
# ----------------------------
flowPin = 0                 # I2C port on the Raspberry Pi to which the meter is connected
flowRate = 0.0              # Instantaneous flow rate measured from the meter (gal/min)
avgFlowRate = 0.0           # Calculated average flow rate over the recording interval (gal/min)
flowSum = 0.0               # A sum variable for calculating the average flow
sampleCount = 0             # The number of samples collected within the recording interval
scanIntervalVolume = 0.0    # Total flow volume for the current scan interval (gal)
recordIntervalVolume = 0.0  # Total flow volume for the current recording interval (gal)
totalizedVolume = 0.0       # Total flow volume from the meter since the program started (gal)
global pulseCount
pulseCount = 0.0            # Number of pulses detected by the meter during a scan interval
totalPulseCount = 0.0       # Total number of pulses detected during a recording interval
pulseVolume = 0.1           # gallons

# Delay the start of the program execution so the times recorded with
# measurements will be even divisions of minutes
# -------------------------------------------------------------------
now = time.localtime(time.time())
remainder = recordInterval - (now.tm_sec % recordInterval)
time.sleep(remainder)
previousTime = time.time()
previousRecordTime = previousTime


# Define the function that will increment the pulse counter when a pulse from the multi-jet meter is detected
def gpio_callback(channel):
    global pulseCount
    pulseCount += 1

# Set up pulse detection on the GPIO pin connected to the multi-jet meter
GPIO.add_event_detect(GPIOPin, GPIO.RISING, callback=gpio_callback)

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

        # Get the time interval
        timeInterval = (currTime - previousTime)  # Time of scan interval in seconds

        # Perform Measurements
        # --------------------
        # Get the flow rate from the multi-jet meter
        flowRate = 60 * (pulseCount * pulseVolume) / timeInterval  # Flow rate (gal/min)
        totalPulseCount += pulseCount
        pulseCount = 0  # Set the pulse count back to 0 immediately after reading for the current scan

        # Calculate the derived values
        scanIntervalVolume = (flowRate * timeInterval) / 60.0  # Total flow for scan interval (gal)
        recordIntervalVolume += scanIntervalVolume             # Total flow for record interval (gal)
        totalizedVolume += scanIntervalVolume                  # Total flow since the current program started (gal)
        flowSum += flowRate                                    # Sum variable for calculating average flow
        avgFlowRate = flowSum / sampleCount                    # Average flow rate (gal/min)

        # Create a data record string
        outputList = [now, sampleNum, totalPulseCount, flowRate, avgFlowRate, recordIntervalVolume, totalizedVolume]
        dataRecord = ','.join(map(str, outputList))

        # Print the current data record to the console for monitoring
        print dataRecord

        # Remember the time at which the current measurement started
        previousTime = currTime

        # Check to see if it's time to record data and if so, write data
        if (currTime - previousRecordTime) >= recordInterval:
            # Save the data record to the output file
            with open(outputFilePath + outputFileName, 'a') as outputFile:
                outputFile.write(dataRecord + '\n')

            # Remember the time at which the record was written to the file
            previousRecordTime = currTime

            # Increment the recorded sample number and reset the average flow rate variables and record interval flow
            sampleNum += 1
            recordIntervalVolume = 0.0
            flowSum = 0.0
            avgFlowRate = 0.0
            sampleCount = 0.0
            totalPulseCount = 0.0
