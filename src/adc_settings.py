#!/usr/bin/env python
#This file contains all the init ADC settings
from ADC_driver import ADCdriver
from Adafruit_ADS1x15 import ADS1x15
#------------------------------------------------------------------

#ADC configurations
#------------------------------------------------------------------
ADS1015 = 0x00  # 12-bit ADC

# Select the gain
#__gain = 61    # +/- 6.144V
gain = 4096  # +/- 4.096V
#__gain = 2048  # +/- 2.048V
#__gain = 1024  # +/- 1.024V
#__gain = 512   # +/- 0.512V
#__gain = 256   # +/- 0.256V


# Select the sample rate
#__sps = 8    # 8 samples per second
#__sps = 16   # 16 samples per second
#__sps = 32   # 32 samples per second
#__sps = 64   # 64 samples per second
#__sps = 128  # 128 samples per second
#sps = 250  # 250 samples per second
#__sps = 475  # 475 samples per second
sps = 860  # 860 samples per second

# Select Address
#ADC0 = 0x48
#ADC1 = 0x49
ADC2 = 0x4a
#ADC3 = 0x4b
#------------------------------------------------------------------
