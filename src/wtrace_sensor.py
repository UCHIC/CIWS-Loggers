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
import time
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



class watermeter():
    def __init__(self, data_directory = '/', sensor_id='wm001',sitename = 'UWRL', sensor_channel = '2', enumerate_directory = True, time_support = 4):
        logging.debug(sensor_id + ' ' + sitename + ' ' + sensor_channel)
        self._sensor_id = sensor_id
        self._sitename = sitename
        self._sensor_channel = sensor_channel
        self._capture_interval = time_support

        
        #Setting ADC to read values
        #self._sensor = ADCdriver(sensor_channel)

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

        #Opening/Creatingand initializing csv file in write mode
        self._output_file = open(self.data_directory + 'WaterTrace.csv', 'w', newline='')
        self._write = csv.writer(self._output_file, delimiter=',', quotechar='"', quoting=csv.QUOTE_MINIMAL)
        metadata = [sensor_id,sitename,sensor_channel]
        header=["TIMESTAMP","RECORD","MEM_SPACE","Batt_Volt","Sensor_Reading"]
        self._write.writerow(metadata)
        self._write.writerow(header)

        # thread stuff
        self._thread = threading.Thread(target=self._thread_run)
        self._thread.daemon = True

        self._start = True

        # go!
        self._thread.start()
        
        pass

    def _thread_run(self):
        with self._start:
            logging.dedug('Thread is running')
            #self._data = self._sensor.ADCread_v()
            #self._write.writerow(['time.time()', )
            

            pass

    def __del__(self):
        self._thread.join()
        self._output_file.close()

        pass

def test():
    print ('Enter here')
    test = watermeter(data_directory = '/Users/mleonardo/Documents/GitHub/WaterMonitor/src/data/')
    #a= watermeter()
    #print (a)
    pass

test()
if __name__ == '__main__()':
    test()
