#!/usr/bin/python

import time, signal, sys
from Adafruit_ADS1x15 import ADS1x15

def signal_handler(signal, frame):
        print 'You pressed Ctrl+C!'
        sys.exit(0)
signal.signal(signal.SIGINT, signal_handler)
#print 'Press Ctrl+C to exit'

ADS1015 = 0x00  # 12-bit ADC
ADS1115 = 0x01	# 16-bit ADC

# Select the gain
# gain = 61    # +/- 6.144V
gain = 4096  # +/- 4.096V
# gain = 2048  # +/- 2.048V
# gain = 1024  # +/- 1.024V
#gain = 512   # +/- 0.512V
# gain = 256   # +/- 0.256V

# Select the sample rate
# sps = 8    # 8 samples per second
# sps = 16   # 16 samples per second
# sps = 32   # 32 samples per second
# sps = 64   # 64 samples per second
#sps = 128  # 128 samples per second
sps = 250  # 250 samples per second
# sps = 475  # 475 samples per second
# sps = 860  # 860 samples per second

# Initialise the ADC using the default mode (use default I2C address)
# Set this to ADS1015 or ADS1115 depending on the ADC you are using!
adc = ADS1x15(address=0x4a,ic=ADS1015)

# Read channel 0 in single-ended mode using the settings above
while 1:
	volts0 = adc.readADCSingleEnded(0, gain, sps)/1000
	#volts1 = adc.readADCSingleEnded(1, gain, sps)/1000
	#volts2 = adc.readADCSingleEnded(2, gain, sps)/1000
        #volts3 = adc.readADCSingleEnded(3, gain, sps)/1000
	#if volts > 1:
		#print "%.6f" % (volts)
        print "Sensor 2"
	print "%.6f" %(volts0)
	#print "%.6f" %(volts1)
	#print "%.6f" %(volts2)
	#print "%.6f" %(volts3)
	time.sleep(1)
	
# To read channel 3 in single-ended mode, +/- 1.024V, 860 sps use:
#volts = adc.readADCSingleEnded(3, 1024, 860)

#print volts
print "%.6f" % (volts)
