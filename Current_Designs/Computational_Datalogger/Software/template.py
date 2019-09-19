import Logger
import os

# The following five lines of code MUST ABSOLUTELY appear in this order. DO NOT MOVE OR CHANGE THE FOLLOWING FOUR LINES OF CODE.
# Logger.initPins() Should never be called by the user. It should only be called when this script is automatically run.

Logger.init()			# Initialzie the Logger Python module.
Logger.initPins()		# Sets pins to initial state. This function should only be called once, when called automatically when powered on.
Logger.setRomBusy()		# Tell the AVR datalogger that the EEPROM chip is in use
Logger.setPowerGood()		# Tell the AVR datalogger that the Raspberry Pi is powered on
dataTuple = Logger.loadData()	# Read the data from the EEPROM chip
Logger.setRomFree()		# Tell the AVR datalogger that the EEPROM chip is no longer in use.

# Process the contents of dataTuple here. The format is as follows:
# Index    |    dataTuple
# ---------------------------------------------------------
#  0	         Number of Records
#  1             Year logging started
#  2             Month logging started
#  3             Day logging started
#  4             Hour logging started
#  5             Minute logging started
#  6             Second logging started
#  7             Data Byte
#  8		 Data Byte
#  9		 Data Byte
# 10		 Data Byte
# ...		 ...

if (dataTuple[0] == Logger.bufferMax()):	# This means that the Pi was turned on by the Datalogger, not a user, so it should turn itself off.
	Logger.setPowerOff()			# Tell the AVR datalogger that the Raspberry Pi is shutting down.
	os.system("sudo poweroff")		# Shut down the Raspberry Pi
