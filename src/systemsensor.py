#from __future__ import division
from cStringIO import StringIO
from xml.dom.minidom import Element
from os.path import exists
from subprocess import Popen, PIPE, STDOUT
#from isaac_link import Isaac_link

#from basesensor import BaseSensor
import logging
from threading import Thread



class SystemSensor():

    def __init__(self):


        self._memory = None
        self._uptime = None
        self._load = None
        self._temperature = None
        self._diskSpace = None
        self._old_stat = None
        self.m_save_ugear = False

        self._memoryfile = '/proc/meminfo'
        self._loadavgfile = '/proc/loadavg'
        self._statfile = '/proc/stat'
        self._uptimefile = '/proc/uptime'
        self._tempfile = '/proc/acpi/thermal_zone/TZ00/temperature'
        self.payload_directory = None
        if exists('/proc/meminfo'):
            self.meminfo = open('/proc/meminfo').read()

        
        pass

    def parse_meminfo(self):
        meminfo = self.meminfo.split('\n')
        memory_total, byte_unit = self.meminfo[0].split()[1:3]
        memory_free = self.meminfo[1].split()[1]
        return 'memory usage: %d %s of %s %s (%.1f%%)'\
            % (int(memory_total) - int(memory_free),
                    byte_unit,
                    memory_total,
                    byte_unit,
                    100*(int(memory_total)-int(memory_free))/int(memory_total))

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

    
    
    def __del__(self):

        pass

def test():

    #system = SystemSensor()

##    while 1:
##        if exists('/proc/meminfo'):
##            meminfo = open('/proc/meminfo').read()
##            memory = system.parse_meminfo(meminfo)
##            pass
##        print (memory)
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

