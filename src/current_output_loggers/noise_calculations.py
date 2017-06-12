# ------------- Variables ---------------
# Variables are reset every epoch
scanInterval = 1.0                              # Time between scans in sec
recordInterval = 1.0                            # Time between recorded values in sec
calibrationFactor = 1.0                         # used to scale voltage on one point calibration
milliAmp = 4.0                                  # Provided current from 4-20mA loop simulator
expectedVoltage = (milliAmp * 0.001) * 200      # Expected Voltage according to Ohm's Law
recordNumber = 1                                # Set initial record number to 1

# -------- CSV File set-up -------------
outputFilePath = '/home/pi/CampusMeters/'
outputFileName = 'Noise_Calculations.csv'

# Header written at the top of the .csv file
dataHeader = '\"RecordNumber\",\"ExpectedVoltage\",\"SensorVoltage\",\"'
print dataHeader
# create a .csv file and write the header at the top.
with open(outputFilePath + outputFileName, 'w') as outputFile:
    outputFile.write(dataHeader + '\n')

# -------------- ADC Initialization ---------
# variables used to initialize the 16-bit ADC
adc = Adafruit_ADS1x15.ADS1115()                # create a 16-bit instance
GAIN = 1                                        # GAIN setting from ADS1115 data-sheet (Table 3)
bitConversion = 0.125                           # mV/bit according to Table 3 of ADS1115 data-sheet



