import Logger
import os

# The following four lines of code MUST ABSOLUTELY appear in this order. DO NOT MOVE OR CHANGE THE FOLLOWING FOUR LINES OF CODE.

Logger.setRomBusy()		# Tell the AVR datalogger that the EEPROM chip is in use
Logger.setPowerGood()		# Tell the AVR datalogger that the Raspberry Pi is powered on
dataTuple = Logger.loadData()	# Read the data from the EEPROM chip
Logger.setRomFree()		# Tell the AVR datalogger that the EEPROM chip is no longer in use.

# Process the contents of dataTuple here. The format is as follows:
# Index    |    dataTuple
# ---------------------------------------------------------
#  0	         Number of Records (Most significant byte)
#  1             Number of Records (Middle byte)
#  2		 Number of Records (Least significant byte)
#  3             Year logging started
#  4             Month logging started
#  5             Day logging started
#  6             Hour logging started
#  7             Minute logging started
#  8             Second logging started
#  9             Data Byte
# 10		 Data Byte
# 11		 Data Byte
# 12		 Data Byte
# ...		 ...

Logger.setPowerOff()		# Tell the AVR datalogger that the Raspberry Pi is shutting down.
os.system("sudo poweroff")
