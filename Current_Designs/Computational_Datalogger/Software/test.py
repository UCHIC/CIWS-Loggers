import Logger
import os

# The following five lines of code MUST ABSOLUTELY appear in this order. DO NOT MOVE OR CHANGE THE FOLLOWING FOUR LINES OF CODE.

Logger.init()			# Initialzie the Logger Python module.
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

print("Number of Records:")
print(dataTuple[0])
print("Maximum allowable records:")
print(Logger.bufferMax())
print("Contents:")
print(dataTuple)

for i in range(Logger.bufferMax()):
	print(i)

Report = [1, 2, 3, 4, 5, 6, 7, 8, 9]
print("Initial Report:")
print(Report)
Report = Logger.reportSwap(Report)
print("New Report:")
print(Report)

Logger.setPowerOff()
