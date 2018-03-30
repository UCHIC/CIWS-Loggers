# ------------------------------------------------------
# A class and functions for working with the MCP3008 ADC
# Created by: Jeff Horsburgh using code originally written
# by Miguel Leonardo
# Note: This is a subset of the functionality available
# for the MCP3008 analog to digital converter via the
# Adafruit Python library for the ADC3008, which is
# available at:
# https://github.com/adafruit/Adafruit_Python_MCP3008
# ------------------------------------------------------
import RPi.GPIO as GPIO


class ADC(object):
    """
    Class to represent the Adafruit MCP3008 analog to digital converter
    """
    def __init__(self):
        # Initialize the ADC
        self._initialize_mcp3008()

    def _initialize_mcp3008(self):
        """
        Initialize the MCP3008 analog to digital converter
        """
        GPIO.setwarnings(False)
        # Set the mode we are using to refer to the GPIO pins to the Broadcom SOC channel
        GPIO.setmode(GPIO.BCM)

        # Set up GPIO pin 13 for an LED if we want to have one
        self._led_gpio = 13

        # Set up the GPIO pins to communicate with the MCP3008 ADC
        # We are using the software SPI to connect to the MCP3008
        self._SPICLK = 18
        self._SPIMISO = 23
        self._SPIMOSI = 24
        self._SPICS = 25

        # Set up the SPI interface pins
        GPIO.setup(self._SPIMOSI, GPIO.OUT)
        GPIO.setup(self._SPIMISO, GPIO.IN)
        GPIO.setup(self._SPICLK, GPIO.OUT)
        GPIO.setup(self._SPICS, GPIO.OUT)
        GPIO.setup(self._led_gpio, GPIO.OUT)

        # Set up the ADC ports we will be using for the sensor intputs
        # including battery voltage
        self._battery_adc = 0
        self._adc1 = 1
        self._adc2 = 3
        self._adc3 = 5
        self._adc4 = 7

    def _readadc(self, adcnum, clockpin, mosipin, misopin, cspin):
        """
        Read the current value of the specified ADC channel (0-7). The values
        can range from 0 to 1023 (10 bits).
        :param adcnum: The ADC channel to measure
        :param clockpin:
        :param mosipin:
        :param misopin:
        :param cspin:
        :return: Output value from the ADC
        """
        if (adcnum > 7) or (adcnum < 0):
            return -1
        GPIO.output(cspin, True)

        GPIO.output(clockpin, False)  # start clock low
        GPIO.output(cspin, False)     # bring CS low

        commandout = adcnum
        commandout |= 0x18  # start bit + single-ended bit
        commandout <<= 3    # we only need to send 5 bits here
        for i in range(5):
            if commandout & 0x80:
                GPIO.output(mosipin, True)
            else:
                GPIO.output(mosipin, False)
            commandout <<= 1
            GPIO.output(clockpin, True)
            GPIO.output(clockpin, False)

        adcout = 0
        # read in one empty bit, one null bit and 10 ADC bits
        for i in range(12):
            GPIO.output(clockpin, True)
            GPIO.output(clockpin, False)
            adcout <<= 1
            if GPIO.input(misopin):
                adcout |= 0x1

        GPIO.output(cspin, True)

        adcout >>= 1       # first bit is 'null' so drop it
        return adcout

    def get_battery_voltage(self):
        """
        Function that gets the battery voltage on ADC channel 0
        :return: Voltage of the supply battery
        """
        battery_read = self._readadc(self._battery_adc, self._SPICLK, self._SPIMOSI, self._SPIMISO, self._SPICS)
        battery_voltage = float((battery_read * (4.2 / 1023)) * 3)
        return battery_voltage

    def get_adc1(self):
        """
        Function that returns the voltage measured on datalogger port 1 (ADC channel 1)
        :return: Voltage measured on the ADC from datalogger port 1
        """
        adc_read = self._readadc(self._adc1, self._SPICLK, self._SPIMOSI, self._SPIMISO, self._SPICS)
        adc_volt = float(adc_read * (4.2 / 1023))
        return adc_volt

    def get_adc2(self):
        """
        Function that returns the voltage measured on datalogger port 2 (ADC channel 3)
        :return: Voltage measured on the ADC from datalogger port 2
        """
        adc_read = self._readadc(self._adc2, self._SPICLK, self._SPIMOSI, self._SPIMISO, self._SPICS)
        adc_volt = float(adc_read * (4.2 / 1023))
        return adc_volt

    def get_adc3(self):
        """
        Function that returns the voltage measured on datalogger port 3 (ADC channel 5)
        :return: Voltage measured on the ADC from datalogger port 3
        """
        adc_read = self._readadc(self._adc3, self._SPICLK, self._SPIMOSI, self._SPIMISO, self._SPICS)
        adc_volt = float(adc_read * (4.2 / 1023))
        return adc_volt

    def get_adc4(self):
        """
        Function that returns the voltage measured on datalogger port 4 (ADC channel 7)
        :return: Voltage measured on the ADC from datalogger port 4
        """
        adc_read = self._readadc(self._adc4, self._SPICLK, self._SPIMOSI, self._SPIMISO, self._SPICS)
        adc_volt = float(adc_read * (4.2 / 1023))
        return adc_volt


