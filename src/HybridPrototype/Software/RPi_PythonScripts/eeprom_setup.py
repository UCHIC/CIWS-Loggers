#!/bin/env/python


# File: eeprom_setup.py
#
# Setup test data on the I2C EEPROM
#
# Date: 6/28/18
# Platform: Raspberry Pi 3 Model B
#
# Python Version: 2.7.9

from smbus import SMBus			# For I2C operations

vals = [0, 0, 5, 0, 1, 0, 245, 0, 2, 1, 245, 0, 3, 2, 244, 0, 4, 4, 244, 0, 5, 3, 243]

bus = SMBus(1)				# Declare "Bus" Object
bus.write_i2c_block_data(0x50, 0, vals)	# Write data to EEPROM
print "Finished"
