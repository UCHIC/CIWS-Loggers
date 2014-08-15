#People Counter
#Written by: Miguel Leonardo
#Version: Beta1.0

import logging
from time import sleep
import os
import sys
import subprocess
import log
from ADC_driver import ADCdriver
import threading
import fcntl
import termios
import struct
import time, signal
from Adafruit_ADS1x15 import ADS1x15

# file manipulation
from os      import remove as remove_file
from os 	 import makedirs
from shutil  import copy   as copy_file
from os.path import join   as join_filename
from os.path import exists
from os.path import dirname
import os
from xml.dom.minidom import Element

from filenamer import Filenamer

from cStringIO import StringIO
from subprocess import Popen, PIPE, STDOUT


log.setLevel(logging.DEBUG)


log.set_udp_destination('192.168.2.150')
#log.set_udp_destination('129.123.189.171')
#log.set_udp_destination('10.0.0.8')#Home connection
#log.set_udp_destination('estimated.local')
#log.set_udp_destination('localhost')

def signal_handler(signal, frame):
        print 'You pressed Ctrl+C!'
        sys.exit(0)
signal.signal(signal.SIGINT, signal_handler)
#print 'Press Ctrl+C to exit'

__ADS1015 = 0x00  # 12-bit ADC

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
sps = 250  # 250 samples per second
#__sps = 475  # 475 samples per second
#__sps = 860  # 860 samples per second

# Select Address
#ADC0 = 0x48
#ADC1 = 0x49
ADC2 = 0x4a
#ADC3 = 0x4b
logging.debug(5)
class pcounter():
    def __init__(self, adc, data_directory='/home/pi/Datalog/',sensor_channel = '0', sensor_id='None'):

        logging.debug('Started People Counter: '+ sensor_id)

        self._sensor_channel = sensor_channel

        #Setting ADC to read values
        #self._adc = ADS1x15(address=ADC2,ic=0x00)
        self._adc = adc
        enumerate_data_directory = True
        #Data logger file manipulation
        if enumerate_data_directory:
            directorynamer = Filenamer('PeopleCounter_%05d/')
            self.data_directory = join_filename(data_directory, directorynamer.next_filename())
            while exists(self.data_directory):
                self.data_directory = join_filename(data_directory, directorynamer.next_filename())
        else:
            self.data_directory = data_directory
        if not exists(self.data_directory):
            makedirs(self.data_directory)
            pass
        self.output_directory = open(self.data_directory + 'PeopleCounter_datalog.xml', 'w')
        self.output_directory.write("<?xml version=\"1.0\" encoding=\"utf-8\"?>")
        self.output_directory.write("\r\n")
        self.output_directory.write("<PeopleCounterDataLog")
        self.output_directory.write(">")
        #self.output_directory.flush()

        # thread stuff
        self._thread = threading.Thread(target=self._thread_run)
        #self._thread.daemon = True

        self._start = True

        # go!
        #self._thread.start()
        self._thread_run()
        #logging.debug('Thread Initialized')

        self._counter = 0
        self._data = 0

        

        pass

    def _thread_run(self):

        logging.debug('started Thread')
        self._counter = 0
        while self._start:
            #print ('started Thread')
	    #print self._sensor_channel
            self._data = self._adc.readADCSingleEnded(0, gain, sps)/1000
            #print self._data
            if self._data > 2:
                #print self._sensor_channel
                self._counter = self._counter+1
                #print self._counter
                element = Element('PeopleCounter')
                element.setAttribute('Record','%d' % self._counter)
                element.setAttribute('Time','%f' % time.time())
                element.setAttribute('DateTime','%s' % time.strftime('%c'))            
                self.output_directory.write("\r\n")
                self.output_directory.write(element.toxml())
                logging.debug(element.toxml())
                sleep(5)
            pass
			
        pass

    def fetch(self, output_directory='none'):

        element = Element('Water_meter001')
        element.setAttribute('Meter_Reading','%f' % self._data)
        self.output_directory.write("\r\n")
        self.output_directory.write(element.toxml())
        
        #element.toxml()
        pass

    def __del__(self):
        #self._thread.join()
        output_directory.write("\n</PeopleCounter>")
        output_directory.flush()

        pass


def test():
    #pc = pcounter()
    pass


if __name__ == '__main__':
    test()
