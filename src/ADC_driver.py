import logging
from time import sleep
import os
import sys
import subprocess
import log


voltage_conversion_factor = 5/2497

class ADCdriver():
    def __init__(self, channel = 2 ):
        

        #print ('Initializing ADC')
        self._channel = channel
        self._cmd1 = "cat /sys/class/hwmon/hwmon0/device/in%d" %self._channel+ "_input"
        
        logging.debug('ADCs initialized')
        logging.debug('Reading channel: %d' %self._channel)
        pass

    
    def ADCread(self):

        process = subprocess.Popen(self._cmd1, stdout=subprocess.PIPE , shell=True)
        os.waitpid(process.pid, 0)[1]
        udata = process.stdout.read().strip()
        result = float(udata)
        return result

    def ADCread_v(self):
        process = subprocess.Popen(self._cmd1, stdout=subprocess.PIPE , shell=True)
        os.waitpid(process.pid, 0)[1]
        udata = process.stdout.read().strip()
        result = float(udata)
        return result*voltage_conversion_factor

        pass
     
 
    def __del__(self):
        pass


def test():
    adc = ADCdriver(channel =3)
    pass
 
if __name__== '__main__':
    test()





