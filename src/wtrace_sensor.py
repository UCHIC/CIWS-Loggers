#Water meter monitor
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
#import fcntl
#import termios
import struct
import time, signal
from Adafruit_ADS1x15 import ADS1x15
import csv

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


#from cStringIO import StringIO
from subprocess import Popen, PIPE, STDOUT
#from isaac_link import Isaac_link


log.setLevel(logging.DEBUG)
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



class watermeter():
    def __init__(self, adc, data_directory = '/', sensor_id='WM001',sitename = 'UWRL', sensor_port = '1', enumerate_directory = True, time_support = 4):
            
        logging.debug(sensor_id + ' ' + sitename + ' ' + sensor_port)
        self._sensor_id = sensor_id
        self._sitename = sitename
        self._sensor_channel = sensor_port
        self._capture_interval = time_support

        self._sample_time = time_support

        self._count = 0
        self._count1 = 0
        self._pulse_count = 0
        self._offset = 2
        
        #Setting ADC to read values
        #self._adc = ADS1x15(address=ADC2,ic=0x00)
        self._adc = adc

        enumerate_data_directory = enumerate_directory
        #Data logger file manipulation
        if enumerate_data_directory:
            directorynamer = Filenamer('WaterTrace_%05d/')
            self.data_directory = join_filename(data_directory, directorynamer.next_filename())
            while exists(self.data_directory):
                self.data_directory = join_filename(data_directory, directorynamer.next_filename())
        else:
            self.data_directory = data_directory
        if not exists(self.data_directory):
            makedirs(self.data_directory)
            pass

        #Opening/Creating and initializing csv file in write mode
        self._output_file = open(self.data_directory + sensor_id + '.csv', 'w')
        self._write = csv.writer(self._output_file, delimiter=',', quotechar='"', quoting=csv.QUOTE_MINIMAL)
        metadata = [sensor_id,sitename,sensor_port]
        header=["TIMESTAMP","RECORD","MEM_SPACE","Batt_Volt","Sensor_Reading"]
        self._write.writerow(metadata)
        self._write.writerow(header)
        #self._output_file.close()

        #Creating lock
        self._lock = threading.Lock()

        # thread stuff
        self._thread = threading.Thread(target=self._thread_run)
        #self._thread.daemon = True

        self._start = True
        #self._thread_run()
        

        # go!
        #self._thread.start()
        self._thread_run()
        pass

    def _thread_run(self):

        
        start = time.time()
        #while self._count1<5:
        while self._start:
            time_lapse = 0
            run = True
            
            while run:
                with self._lock:
                        self._data = self._adc.readADCSingleEnded(int(self._sensor_channel), gain, sps)/1000
		#print self._data
                #self._data = 3
                self._count = self._count + 1

                if self._data > self._offset:
                    self._pulse_count = self._pulse_count + 1
		    print self._data
		    print self._sensor_id
                duration = (time.time() - start)
                if duration > self._sample_time:
                    run = False
                    pass
                pass
            start = time.time()    
            #Logging Data
            Record = self._count1
            Reading = self._pulse_count
            #MEM_space = getFreeSpace()
            MEM_space = 8000000
            Bat_Volt = 5
            Time = time.time()
            Date_Time = time.strftime('%c')
            self._data = [Date_Time, Record,MEM_space, Bat_Volt, Reading]
            self._write.writerow(self._data)
            self._count1 = self._count1 + 1
            self._pulse_count = 0

##            if getFreeSpace()<10000:
##                self._start = False
##                element.setAttribute('Warning','Not enough memory on Dataloger')        

            pass

    def __del__(self):
        #self._thread.join()
        self._output_file.close()

        pass

def test():
        
    #print ('Enter here')
    #adc = ADS1x15(address=ADC2,ic=0x00)
    #test = watermeter(data_directory = '/home/aggieair/Desktop/WaterMeter/')
    #test1 = watermeter(adc, data_directory = '/home/pi/Datalog/',sensor_port = '0', sensor_id = '1')
    #test2 = watermeter(adc, data_directory = '/home/pi/Datalog/',sensor_port = '1', sensor_id = '2')
    #a= watermeter()
    #print (a)
    pass

test()
if __name__ == '__main__()':
    test()
