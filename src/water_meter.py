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
import fcntl
import termios
import struct
import time

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
#from isaac_link import Isaac_link


log.setLevel(logging.DEBUG)


log.set_udp_destination('192.168.2.3')
log.set_udp_destination('129.123.189.171')
log.set_udp_destination('10.0.0.8')#Home connection
#log.set_udp_destination('estimated.local')
#log.set_udp_destination('localhost')



class watermeter():
    def __init__(self, data_directory):

        logging.debug('Hello from Water Meter 1')

        #Setting ADC to read values
        self._sensor = ADCdriver(channel = 2)
        enumerate_data_directory = True
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
        self.output_directory = open(self.data_directory + 'WaterTrace_datalog.xml', 'w')
        self.output_directory.write("<?xml version=\"1.0\" encoding=\"utf-8\"?>")
        self.output_directory.write("\r\n")
        self.output_directory.write("<WaterTraceDataLog")
        self.output_directory.write(">")
        #self.output_directory.flush()

        # thread stuff
        self._thread = threading.Thread(target=self._thread_run)
        self._thread.daemon = True

        self._start = True

        # go!
        self._thread.start()
        

        pass

    def _thread_run(self):

        #print ('started Thread')
        while self._start:
            #print ('started Thread')
            self._data = self._sensor.ADCread_v()
            element = Element('Water_meter001')
            element.setAttribute('Meter_Reading','%f' % self._data)
            element.setAttribute('Free_Space','%d' % getFreeSpace())
            element.setAttribute('Time','%f' % time.time())
            
            self.output_directory.write("\r\n")
            self.output_directory.write(element.toxml())
			
        pass

    def fetch(self, output_directory='none'):

        element = Element('Water_meter001')
        element.setAttribute('Meter_Reading','%f' % self._data)
        self.output_directory.write("\r\n")
        self.output_directory.write(element.toxml())
        
        #element.toxml()
        pass

    






    def __del__(self):
        self._thread.join()
        output_directory.write("\n</WaterTraceDataLog>")
        output_directory.flush()

        pass


def test():

    meter = watermeter('/media/card/Datalog/')
    meter._thread_run()
    #print (getFreeSpace())
    pass

def getFreeSpace(partition=0):
	partition += 1	
	cmd = 'df'	
	p = Popen(cmd, shell=True, stdin=PIPE, stdout=PIPE, stderr=STDOUT, close_fds=True)
	output = p.stdout.read()

	space = ""
	numOfWord = 1
	start = 0
	end = 0
	outOfBlank = True
	count = 0

	while(start == 0 or end == 0):

		if output[count] == ' ' and outOfBlank == True:
			outOfBlank = False
			if numOfWord == 5+5*partition:
					end = count
		if (not(output[count] == ' ')) and outOfBlank == False:
			outOfBlank = True
			numOfWord += 1
			if numOfWord == 5+5*partition:
				start = count
		count += 1

	space = output[start:end]
	return int(space)


if __name__ == '__main__':
    test()
