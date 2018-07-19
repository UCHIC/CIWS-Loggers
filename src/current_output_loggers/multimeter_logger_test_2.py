# ----------------------------------------------------------------------------
# multi_meterlog.py
# Author: Jeff Horsburgh
# Creation Date: 7/25/2016
# Development Environment:
#   PyCharm 5.0.5
#   Python 2.7
# Hardware Platform:
#   Raspberry Pi 3 Model B
#   Adafruit ADS1015 ADC
#        -- OR --
#   Adafruit ADS1115 ADC
#
# This script makes high frequency measurements of flow through water meters
# installed on USU's student residence hall buildings at the Living Learning
# Center. It measures the 4-20 mA output of the Octave meters and the pulsed
# output from the multi-jet meters and converts them to gallons per minute.
# It then does the math to add the hot water inflow to the cold water inflow
# and then subtracts the hot water outflow. For each meter, it outputs flow
# rate, the incremental flow for each time period, and totalized flow since
# the program started.
#
# The sampling rate and data recording rate are configurable
#
# TODO:
#
# ----------------------------------------------------------------------------
import time
# Import the ADS1x15 module.
import Adafruit_ADS1x15
import sys
import RPi.GPIO as GPIO

# Set all site specific configuration options for deployment
# ----------------------------------------------------------
siteCode = "UWRL_Test_Facility"
# Set the scanning and recording intervals at which the program should run
scanInterval = 1.0          # Time between scans within the main loop in seconds
#recordInterval = 30.0       # Time between recorded values in seconds
recordInterval = 1.0        # Time between recorded values in seconds
# Set meter Specific Values
coldInMaxFlowRate = 200.0         # Maximum flow rate of the cold water inflow meter at 20 mA output (gal/min)
coldInCalibrationFactor = 1.0     # One point calibration value to scale the cold water meter output voltages
coldInCalibration = 0.0042			 # Arithmetic Calibration value to adjust the cold water meter output voltages
hotInMaxFlowRate = 50.0           # Maximum flow rate of the hot water inflow meter at 20 mA output (gal/min)
hotInCalibrationFactor = 1.0      # One point calibration value to scale the hot water meter output voltages
hotInCalibration = 0.0042			 # Arithmetic Calibration value to adjust the hot water meter output voltages

# Set up the GPIO pin for the pulse counting meter
# ------------------------------------------------
GPIOPin = 23
GPIO.setmode(GPIO.BCM)
GPIO.setup(GPIOPin, GPIO.IN)

# Create an ADS1015 ADC (12-bit) instance with default address
# ------------------------------------------------------------
# Note: the I2C address can be changed from its default address (0x48), and/or
# the I2C bus can be changed by passing in these optional parameters:
# adc = Adafruit_ADS1x15.ADS1015(address=0x49, busnum=1)
#adc = Adafruit_ADS1x15.ADS1015()

# Create an ADS1115 ADC (16-bit) instance with default address
# ------------------------------------------------------------
# Note: the I2C address can be changed from its default address (0x48), and/or
# the I2C bus can be changed by passing in these optional parameters:
# adc = Adafruit_ADS1x15.ADS1115(address=0x49, busnum=1)
adc = Adafruit_ADS1x15.ADS1115()

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
#bitConversionFactor = 2  # mV per bit per the table above
bitConversionFactor = 0.125 # Proper bit conversion factor for ADS1115

# Set up the data log file
# ------------------------
localTime = time.localtime(time.time())
outputFilePath = '/home/pi/CampusMeter/'
outputFileName = 'multi_meter_datalog_' + siteCode + '_' + str(localTime.tm_year) + '-' + str(localTime.tm_mon) \
                 + '-' + str(localTime.tm_mday) + '_' + str(localTime.tm_hour) + '-' \
                 + str(localTime.tm_min) + '-' + str(localTime.tm_sec) + '.csv'
# Data file header lines - these lines get written to the beginning of the file when it's created
loggerName = '"Campus Meter Data Log for ' + siteCode + '\"'
print loggerName

# Data file column header
#dataHeader = '"Date\",\"RecordNumber\",\"coldInSensorVoltage\",\"coldInFlowRate\",\"coldInAvgFlowRate\",' \
#             '\"coldInIncrementalVolume\",\"coldInTotalizedVolume\",\"hotInSensorVoltage\",\"hotInFlowRate\",' \
#             '\"hotInAvgFlowRate\",\"hotInIncrementalVolume\",\"hotInTotalizedVolume\",\"hotOutPulseCount\",' \
#             '\"hotOutFlowRate\",\"hotOutAvgFlowRate\",\"hotOutIncrementalVolume\",\"hotOutTotalizedVolume\",' \
#             '\"netFlowRate\",\"netAvgFlowRate\",\"netIncrementalVolume\",\"netTotalizedVolume\"'
# Data file column header for Testing
dataHeader = '"Date\",\"RecordNumber\",' \
				 '\"coldInVoltage\",\"coldInFlowRate\",\"coldInVolume\",' \
				 '\"hotInVoltage\",\"hotInFlowRate\",\"hotInVolume\",' \
				 '\"hotOutPulseCount\",\"hotOutFlowRate\",\"hotOutVolume\"'

print dataHeader
# Open the file for writing to make sure the file gets created, write the header, then close it
with open(outputFilePath + outputFileName, 'w') as outputFile:
    outputFile.write(loggerName + '\n')

# Initialize timing variables
# --------------------
sampleNum = 1             # Initialize the sample number to 1
currTime = 0.0            # Timing variable
previousTime = 0.0        # Timing variable
previousRecordTime = 0.0  # Timing variable
timeInterval = 0.0        # Timing Variable

# Define sensor related values
# ----------------------------
coldInflowPin = 0           # Analog port on the ADC to which the cold water inflow meter is connected
hotInFlowPin = 1            # Analog port on the ADC to which the hot water inflow meter is connected
resistorValue = 200.5       # The value of the shunt resistor in ohms used for the 4-20 mA current loop measurement -- Adjust for each resistor for better precision.
minVoltage = (0.004 * resistorValue)  # The minimum voltage value that can be measured based on Ohm's Law
maxVoltage = (0.020 * resistorValue)  # The maximum voltage that can be measured
voltageRange = maxVoltage - minVoltage  # Voltage range based on resistor value in volts

coldInSensorVolts = 0.0     # Voltage of the analog input from the cold water inflow meter
hotInSensorVolts = 0.0      # Voltage of the analog input from the hot water inflow meter
global pulseCount
pulseCount = 0.0            # Pulse count from the hot water return meter
pulseVolume = 0.1           # The volume of water per measured pulse from the hot water return multi-jet meter

coldInFlowRate = 0.0        # Instantaneous flow rate measured from cold water inflow meter (gal/min)
hotInFlowRate = 0.0         # Instantaneous flow rate measured from hot water inflow meter (gal/min)
hotOutFlowRate = 0.0        # Instantaneous flow rate measured from hot water inflow meter (gal/min)
netFlowRate = 0.0           # Instantaneous net flow rate measured from hot water return meter (gal/min)
avgColdInFlowRate = 0.0     # Calculated average cold inflow rate over the recording interval (gal/min)
avgHotInFlowRate = 0.0      # Calculated average hot inflow rate over the recording interval (gal/min)
avgHotOutFlowRate = 0.0     # Calculated average hot outflow rate over the recording interval (gal/min)
netAvgFlowRate = 0.0        # Calculated net average flow rate over the recording interval (gal/min)
coldInFlowSum = 0.0         # A sum variable for calculating the average cold water inflow
hotInFlowSum = 0.0          # A sum variable for calculating the average hot water inflow
hotOutFlowSum = 0.0         # A sum variable for calculating the average hot water outflow
netFlowSum = 0.0            # A sum variable for calculating the net water flow
sampleCount = 0             # The number of samples collected within the recording interval
coldInScanIntervalVolume = 0.0    # Total cold water inflow volume for the current scan interval (gal)
hotInScanIntervalVolume = 0.0     # Total hot water inflow volume for the current scan interval (gal)
hotOutScanIntervalVolume = 0.0    # Total hot water outflow volume for the current scan interval (gal)
netScanIntervalVolume = 0.0       # Total net flow volume for the current scan interval (gal)
coldInRecordIntervalVolume = 0.0  # Total cold water inflow volume for the current recording interval (gal)
hotInRecordIntervalVolume = 0.0   # Total hot water inflow volume for the current recording interval (gal)
hotOutRecordIntervalVolume = 0.0  # Total hot water outflow volume for the current recording interval (gal)
netRecordIntervalVolume = 0.0     # Total net flow volume for the current recording interval (gal)
coldInTotalizedVolume = 0.0       # Total cold water inflow volume from the meter since the program started (gal)
hotInTotalizedVolume = 0.0        # Total hot water inflow volume from the meter since the program started (gal)
hotOutTotalizedVolume = 0.0       # Total hot water outflow volume from the meter since the program started (gal)
netTotalizedVolume = 0.0          # Total net flow volume since the program started (gal)
pulseCount = 0					  # Total pulses counted
newPulse = 1					  # Boolean: is the detected pulse new or already counted?

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

	# Check for a new pulse from the multi-jet meter
    if GPIO.input(GPIOPin) == 0:
	if newPulse == 1:
		pulseCount += 1
		newPulse = 0
    if GPIO.input(GPIOPin) == 1:
	time.sleep(0.05)
	newPulse = 1

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
        # Get the flow rate from the multi-jet meter on the hot water return
        hotOutPulseCount = pulseCount  # Get the current pulse count from the hot water return meter
        pulseCount = 0  # Set the pulse count back to 0 immediately after reading for the current scan
        hotOutFlowRate = 60 * (hotOutPulseCount * pulseVolume) / timeInterval  # Flow rate (gal/min)

        # Get the voltage measurements from the Octave meters - divide by 1000 to convert from mV to V
        coldInSensorVolts = (coldInCalibrationFactor * bitConversionFactor * adc.read_adc(coldInflowPin, gain=GAIN) / 1000) + coldInCalibration
        hotInSensorVolts = (hotInCalibrationFactor * bitConversionFactor * adc.read_adc(hotInFlowPin, gain=GAIN) / 1000) + hotInCalibration

        # Check to see if the sensor voltages are greater than the minimum - if not, set flow to zero
        # Add an arbitrary buffer onto the minVoltage (0.0001 V) to avoid small errors when the flow is zero
        if coldInSensorVolts > (minVoltage + 0.0001):
            coldInFlowRate = (coldInMaxFlowRate * (coldInSensorVolts - minVoltage)) / voltageRange  # gallons per minute
        else:
            coldInFlowRate = 0.0
        if hotInSensorVolts > (minVoltage + 0.0001):
            hotInFlowRate = (hotInMaxFlowRate * (hotInSensorVolts - minVoltage)) / voltageRange  # gallons per minute
        else:
            hotInFlowRate = 0.0

        # Calculate the derived values
        coldInScanIntervalVolume = (coldInFlowRate * timeInterval) / 60.0  # Total flow for scan interval (gal)
        coldInRecordIntervalVolume += coldInScanIntervalVolume             # Total flow for record interval (gal)
        coldInTotalizedVolume += coldInScanIntervalVolume                  # Total flow since the program started (gal)
        coldInFlowSum += coldInFlowRate                                    # Sum variable for calculating average flow
        coldInAvgFlowRate = coldInFlowSum / sampleCount                    # Average flow rate (gal/min)

        hotInScanIntervalVolume = (hotInFlowRate * timeInterval) / 60.0  # Total flow for scan interval (gal)
        hotInRecordIntervalVolume += hotInScanIntervalVolume             # Total flow for record interval (gal)
        hotInTotalizedVolume += hotInScanIntervalVolume                  # Total flow since the program started (gal)
        hotInFlowSum += hotInFlowRate                                    # Sum variable for calculating average flow
        hotInAvgFlowRate = hotInFlowSum / sampleCount                    # Average flow rate (gal/min)

        hotOutScanIntervalVolume = hotOutPulseCount * pulseVolume          # Total flow for scan interval (gal)
        hotOutRecordIntervalVolume += hotOutScanIntervalVolume             # Total flow for record interval (gal)
        hotOutTotalizedVolume += hotOutScanIntervalVolume                  # Total flow since the program started (gal)
        hotOutFlowSum += hotOutFlowRate                                    # Sum variable for calculating average flow
        hotOutAvgFlowRate = hotOutFlowSum / sampleCount                    # Average flow rate (gal/min)
        hotOutPulseCountScan = hotOutPulseCount                            # hotOutPulseCountScan will be displayed
        hotOutPulseCount = 0.0                                             # Zero out the pulse count for the scan

        netScanIntervalVolume = coldInScanIntervalVolume + hotInScanIntervalVolume - hotOutScanIntervalVolume  # (gal)
        netRecordIntervalVolume += netScanIntervalVolume             # Total flow for record interval (gal)
        netTotalizedVolume += netScanIntervalVolume                  # Total flow since the program started (gal)
        netFlowSum += netFlowRate                                    # Sum variable for calculating average flow
        netAvgFlowRate = netFlowSum / sampleCount                    # Average flow rate (gal/min)

        # Create a data record string
        outputList = [now, sampleNum, 
        					 coldInSensorVolts, coldInFlowRate, coldInTotalizedVolume, 
        					 hotInSensorVolts, hotInFlowRate, hotInTotalizedVolume, 
        					 hotOutPulseCountScan, hotOutFlowRate, hotOutTotalizedVolume]
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
            coldInRecordIntervalVolume = 0.0
            hotInRecordIntervalVolume = 0.0
            hotOutRecordIntervalVolume = 0.0
            netRecordIntervalVolume = 0.0
            coldInFlowSum = 0.0
            hotInFlowSum = 0.0
            hotOutFlowSum = 0.0
            netFlowSum = 0.0
            coldInAvgFlowRate = 0.0
            hotInAvgFlowRate = 0.0
            hotOutAvgFlowRate = 0.0
            netAvgFlowRate = 0.0
            sampleCount = 0.0

# Added by Joshua Tracy, 5/14/18:
#   Changed "def gpio_callback():" to "def gpio_callback(GPIOPin):"
#       Fixed reported argument error
#
#   Added variable "hotOutPulseCountScan", assigned to hotOutPulseCount before "hotOutPulseCount = 0.0"
#       Allows data to be displayed
# 
#   Changed Record Interval to 1 second for testing purposes
#
#   End of Line

# Known Issues (Josh Tracy, 7/16/18):
#	ADC-measured voltage slightly off from multi-meter measured voltage -- Probably fine
#	Water flow-rate waaayyyyy off from meter reading -- Incorrect meter setup likely. Testing today.
#
#	End of Line

# Fixing Issues (Josh Tracy, 7/17/18):
# ADC-measured voltage slightly off -- one-point calibration fix on each channel
# Water flow-rate waaayyyyy off from meter reading -- Incorrect meter setup. Fixed.
#
# End of Line

# Fixing Issues (Josh Tracy, 7/18/18):
# GPIO Callbacks removed. Switch instead checked regularly (the pulses are long and slow)
