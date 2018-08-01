#!/bin/env/python


# File: hybrid_logger.py
#
# Raspberry Pi data transfer script
#
# Date: 6/27/18
# Daniel Henshaw and Josh Tracy
# Platform: Raspberry Pi 3 model B
#
# Python version: 2.7.9

import os                                       # for Linux terminal commands\
import RPi.GPIO as GPIO                         # for General Purpose I/O library
from   smbus import SMBus                       # for basic i2c (smbus) functions
import time                                     # for time functions

#--------------------------------------------------------------------------------------------------
#
# Section: Function to get Diskspace
#
#--------------------------------------------------------------------------------------------------

def getDiskSpace():
	p = os.popen("df /")
	i = 0
	while 1:
		i = i + 1
		line = p.readline()
		if i == 2:
			return(line.split()[1:5])

#--------------------------------------------------------------------------------------------------
#
# Section: Get the Time for the Datalogger
#
#--------------------------------------------------------------------------------------------------


currTime = time.time()                          # Get the current time on the Raspberry Pi
print(currTime)

now = time.strftime("\"%Y-%m-%d %H:%M:%S\"", time.localtime(currTime)) # Get a properly formatted date string
print(now)

samplePeriod = 4                                # define number of seconds per record

bootDelay = 26                                  # define boot delay in seconds, 26 seconds was verified with testing.

crunchTime = 600                                # Amount of time user has to grab data off of the Raspberry Pi (in seconds)

lastRecord = 0					# The last record number of the last run, initially set to zero.

if os.path.exists("/home/pi/LastRecord"):		# Determine the last record number to compute current record number in this run
	fh = open("/home/pi/LastRecord", "r")
	lastRecord = fh.read()
	fh.close()

#--------------------------------------------------------------------------------------------------
#
# Section: Raspberry Pi GPIO Configuration
#
#--------------------------------------------------------------------------------------------------

# Generate "EEPROM Busy" signal on the Raspberry Pi's GPIO

GPIO.setmode(GPIO.BCM)                          # use BCM pin numbering
busyPin = 19                                    # GPIO 19 as signal pin
GPIO.setup(busyPin,GPIO.OUT)                    # set pin as an output
GPIO.output(busyPin,GPIO.HIGH)                  # set it high, meaning the EEPROM is busy

# Generate "Pi On" signal on the Raspberry Pi's GPIO

GPIO.setmode(GPIO.BCM)				# Use BCM pin numbering
onPin = 25					# GPIO 25 as signal pin
GPIO.setup(onPin,GPIO.OUT)			# Set pin as an output
GPIO.output(onPin,GPIO.HIGH)			# Set it high, signalling to the controller that the system is on

# Listen for "Stay ON" signal on the Raspberry Pi's GPIO

GPIO.setmode(GPIO.BCM)                          # use BCM pin numbering
stayOnPin = 13                                  # GPIO 13 as signal pin
GPIO.setup(stayOnPin,GPIO.IN)                   # set pin as an input


#--------------------------------------------------------------------------------------------------
#
# Section: Initialize File
#
#--------------------------------------------------------------------------------------------------


filename = "/home/pi/test.csv"	# Define a path/filename to be used.
dataLoggerName = "0000"				# Define a Data Logger name
siteName = "0000"                               # Define a Site Name
siteDescription = "UWRL Test Facility"          # Define a Site Description

if os.path.exists(filename):			# If the file exists, 
	fh = open(filename, "a")                    # Open the file to append
else:                                           # Otherwise,
	fh = open(filename, "w")                    # Open the new file and write the header
	fh.write("CIWS Data Logger Output File\n")
	fh.write("DataLoggerName: ")
	fh.write(dataLoggerName)
	fh.write("\nSiteName: ")
        fh.write(siteName)
        fh.write("\nSiteDescription: ")
        fh.write(siteDescription)
        fh.write("\nTime,Record,Pulses,Disk_Space_Available (kB)\n")


#--------------------------------------------------------------------------------------------------
#
# Section: Data Transfer from EEPROM to SD Card Data File
#
#--------------------------------------------------------------------------------------------------


# prepare to read EEPROM chip starting at address zero
#  what address belongs to EEPROM? 1010000 in binary ==> 0x50 ==> 80 in base ten, before left shifting the Read/Write bit into the address
#  to read, we set the R/W bit to 1 (R/~W).
#  1010 0001
#   send that address (control byte)
#  then send two bytes for addressing into the EEPROM


# declare object
bus = SMBus(1)                                  # NOTE: the i2c bus number is 1, not zero like in some tutorials

bus.write_byte_data(0x50, 0, 0)                 # start reading EEPROM at address zero

numRecordsHigh = bus.read_byte(0x50)            # read high byte of the number of records on the EEPROM
numRecordsLow  = bus.read_byte(0x50)            # read low  byte of the number of records on the EEPROM
dummy          = bus.read_byte(0x50)		# clear dummy byte
dummy          = bus.read_byte(0x50)		# clear dummy byte

numRecords = (numRecordsHigh << 8) + numRecordsLow  # splice together the low and high bytes to get the number of records

firstRecordTime = currTime - (numRecords * samplePeriod) - bootDelay # calculate starting time in seconds

# EEPROM loop
for i in range(numRecords):                     # loop reads an entire three byte record (sample) + one dummy byte for EEPROM organization purposes
    recordNumHigh = bus.read_byte(0x50)
    recordNumLow  = bus.read_byte(0x50)
    pulseCount    = bus.read_byte(0x50)
    dummy         = bus.read_byte(0x50)

    recordNum = (recordNumHigh << 8) + recordNumLow     # splice together the high and low bytes of recordNum
    recordNum = recordNum + int(lastRecord)		# Compute current record number based on last record number

    currTime = firstRecordTime + (samplePeriod * i)     # calculate time difference for each individual record

    timeStamp = time.strftime("\"%Y-%m-%d %H:%M:%S\"", time.localtime(currTime))  # write timestamp for the current record

    DISK_stats = getDiskSpace()				# Get free space on Disk
    diskSpace = DISK_stats[2]

    # write data out to file
    fh.write(`timeStamp`)
    fh.write(',')
    fh.write(`recordNum`)
    fh.write(',')
    fh.write(`pulseCount`)
    fh.write(',')
    fh.write(diskSpace)
    fh.write("\n")

GPIO.output(busyPin,GPIO.LOW)  # As soon as EEPROM to Raspberry Pi data transfer is complete, set the EEPROM Busy signal LOW, to signal the ATtiny microcontroller.

fh.close()

fh = open("/home/pi/LastRecord", "w")
fh.write(`recordNum`)
fh.close()

#--------------------------------------------------------------------------------------------------
#
# Section: Power ON/OFF Control
#
#--------------------------------------------------------------------------------------------------

stayOn = GPIO.input(stayOnPin)                  # read the status of the Stay On Pin

if stayOn == True:
    time.sleep(crunchTime)                      # have the script wait for crunchTime seconds before an automatic system poweroff

os.system("sudo poweroff")                      # automatic system poweroff
