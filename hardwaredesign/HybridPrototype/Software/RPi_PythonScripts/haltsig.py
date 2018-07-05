#!/usr/bin/env python

# File:    haltsig.py
# Authors: Joshua Tracy and Daniel Henshaw
# Date:    06/05/2018
# 
# Description: This tiny program sets GPIO pin 26 as an output and sets it high.
# Why: This is to create a halt notification signal that our microcontroller
#	can detect to allow for safe power off.
#

import RPi.GPIO as GPIO
GPIO.setmode(GPIO.BCM)
pin = 25
GPIO.setup(pin,GPIO.OUT)
GPIO.output(pin,GPIO.HIGH)
