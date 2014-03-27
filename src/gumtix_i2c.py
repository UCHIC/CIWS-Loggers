#!/usr/bin/env python
#
# Add python and python-fcntl packages to your Overo image
#
# Python seems to be adding an extra character in the write
# call. Setting the limit at 32 messes up the Arduino.
# Investigate later. Leaving the limit at 31 for now.

import fcntl;
import sys;

IOCTL_I2C_SLAVE = 0x0703

ARDUINO_SLAVE_ADDRESS = 0x48

fh = open('/dev/i2c-3', 'r+', 1)

fcntl.ioctl(fh, IOCTL_I2C_SLAVE, ARDUINO_SLAVE_ADDRESS)

if len(sys.argv) > 1:
        buff = sys.argv[1]

        if len(buff) > 31:
                buff = buff[:31]
else:
        buff = hex(16516)

bufflen = len(buff)

print 'Sent:', buff

fh.write(buff)

buff = fh.read(bufflen)
#buff = fh.read(20)

print buff

if len(buff) > 0:
        print 'Received:', buff

fh.close()
