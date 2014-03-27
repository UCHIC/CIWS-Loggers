#!/usr/bin/env python

import fcntl;

IOCTL_I2C_SLAVE = 0x0703
ARDUINO_SLAVE_ADDRESS = 0x48

fh = open('/dev/i2c-3', 'r+', 1)

fcntl.ioctl(fh, IOCTL_I2C_SLAVE, ARDUINO_SLAVE_ADDRESS)

fh.write('C421')

response = fh.readline()
#print response
#if len(response) > 0:
#	print response
#else:
#	print 'Arduino not responding'

fh.close()
