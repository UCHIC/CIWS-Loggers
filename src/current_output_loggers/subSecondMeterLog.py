import time
from datetime import datetime, timedelta
import pytz
import Adafruit_ADS1x15


# _____________Site Specific settings____________________

siteCode = "Richards_Hall"              # Location of the reading
scanIntervalStr = "500ms"               # String used in the output file name
# scanInterval = 0.50                     # Time between scans within the main loop (sec)
recordInterval = 0.50                   # Time between recorded values (sec)
maxFlowRate = 250.0                     # Max flow corresponding to a 20mA output (gal/min)
calibrationFactor = 1.0                 # Scale output voltages to one point calibration

# __________________Gain Settings_________________________

adc = Adafruit_ADS1x15.ADS1115()        # Initialize the 16 bit ADC
# This setting changes the range of voltages that are read
# See table 3 in the ADS1015/ADS1115 data-sheet for more info on gain.
#  - 2/3 = +/-6.144V 1 bit = 3 mV
#  -   1 = +/-4.096V 1 bit = 2 mV
#  -   2 = +/-2.048V 1 bit = 1 mV
#  -   4 = +/-1.024V 1 bit = 0.5 mV
#  -   8 = +/-0.512V 1 bit = 0.25 mV
#  -  16 = +/-0.256V 1 bit = 0.125 mV
GAIN = 1
bitConversion = 0.125

# ___________________Data Log Output File__________________

localTime = time.localtime(time.time())
outputFilePath = '/home/pi/CampusMeters/datalogs/'
outputFileName = 'datalog_' + scanIntervalStr + '_' + siteCode + '_' + str(localTime.tm_year) \
                 + '-' + str(localTime.tm_mon) + '-' + str(localTime.tm_mday) \
                 + '_' + str(localTime.tm_hour) + ':' + str(localTime.tm_min) \
                 + ':' + str(localTime.tm_sec) + '.csv'

# .csv file heading
loggerName = '\"Campus Meter Data log for: ' + siteCode + '\"'
print loggerName

dataHeader = '\"Date\",\"RecordNumber\",\"SensorVoltage\",\"FlowRate\",\"AvgFlowRate\",' \
             '\"IncrementalVol\",\"TotalVol\"'
print dataHeader

# open and write heading to the .csv file
with open(outputFilePath + outputFileName, 'w') as outputFile:
    outputFile.write(loggerName + '\n')
    outputFile.write(dataHeader + '\n')

# _____________________Timing Variables______________________
# Initial set up of timing variables, these reset every epoch.
sampleNumber = 1
currTime = 0.0
prevTime = 0.0
prevRecTime = 0.0
timeInterval = 0.0
tmDelta = timedelta(millisecond=500)

# Arizona Mountain Standard Time (MST) == UTC -7.0
# This is always the case, because no DST.
mtnStdTm = pytz.timezone('MST')
now = datetime.now(mtnStdTm)

remainder = recordInterval - (now.second % recordInterval)
time.sleep(remainder)
prevTime = datetime.now()
prevRecTime = prevTime

# ______________________Sensor Variables_______________________
# Initial values for sensor variables, these reset every epoch

flowPin = 0                                 # Pin connection on ADC to which the meter is connected
resistorValue = 200                         # Shunt Resistor value in ohms
minVoltage = (0.004 * resistorValue)        # Minimum voltage corresponding to 4 mA meter reading
maxVoltage = (0.020 * resistorValue)        # Maximum voltage corresponding to 20 mA meter reading
voltageRange = maxVoltage - minVoltage      # Voltage range corresponding to 4-20 mA output
sensorVoltage = 0.0                         # Voltage input from the meter
flowRate = 0.0                              # Instantaneous flow rate from the meter (gal/min)
avgFlowRate = 0.0                           # Average flow rate over the recording interval (gal/min)
flowSum = 0.0                               # Variable used to calculate avg flow rate
sampleCount = 0                             # The sample count within the main loop
scanIntervalVol = 0.0                       # Total flow volume for the current scan interval (gal)
recordIntervalVol = 0.0                     # Total flow volume for the current recording interval (gal)
totalVol = 0.0                              # Total flow volume since epoch (gal)

# ______________________Main Program___________________________

while True:
    # Set the current time
    currTime = datetime.now()
    elapsedTime = (currTime - prevTime)

    # If the time between scans is greater than the set scan
    # interval run the program.
    if elapsedTime >= tmDelta:
        sampleCount += 1

        # format date/time string
        now = datetime.now(mtnStdTm)
        timeStamp = now.strftime("\"%Y-%m-%d %H:%M:%S.%f\"",)

        # Take the measurement from the meter and convert it from V to mV
        sensorVoltage = calibrationFactor * bitConversion * adc.read_adc(flowPin, gain=GAIN) / 1000

        # If the reading is greater than the 4 mA (plus a buffer) set the flow rate.
        # If the reading is not greater than the 4 mA (plus a buffer) flow rate is set to 0.
        if sensorVoltage > (minVoltage + 0.0001):
            flowRate = (maxFlowRate * (sensorVoltage - minVoltage)) / voltageRange
        else:
            flowRate = 0.0

        timeInterval = 0.500                                                 # Time of scan interval in (s)
        scanIntervalVol = (flowRate * timeInterval) / 60                     # Total flow vol for scan interval (gal)
        recordIntervalVol += scanIntervalVol                                 # Total flow  vol for record interval (gal)
        totalVol += scanIntervalVol                                          # Total flow vol since epoch (gal)
        flowSum += flowRate                                                  # Flow sum used to calculate avg flow
        avgFlowRate = flowSum / sampleCount                                  # in (gal/min)

        # data record array
        outputList = [timeStamp, sampleNumber, sensorVoltage, flowRate, avgFlowRate, recordIntervalVol, totalVol]
        dataRecord = ','.join(map(str, outputList))
        print dataRecord

        prevTime = currTime

        # If the record interval has passed save the data record to the .csv file
        if elapsedTime >= tmDelta:
            with open(outputFilePath + outputFileName, 'a') as outputFile:
                outputFile.write(dataRecord + '\n')

            prevRecTime = currTime

            # Increment the sampleNumber and reset the rest of the following variables
            sampleNumber += 1
            recordIntervalVol = 0.0
            flowSum = 0.0
            avgFlowRate = 0.0
            sampleCount = 0


