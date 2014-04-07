from wtrace_sensor import *
from MotionSensor import *
from Adafruit_ADS1x15 import ADS1x15

import threading

adc = ADS1x15(address=0x4a,ic=0x00)




def start_meter_1():
    
    watermeter(adc, data_directory = '/home/pi/Datalog/',sensor_port = '1', sensor_id = 'WM01')

    pass

def start_meter_2():

    watermeter(adc, data_directory = '/home/pi/Datalog/',sensor_port = '3', sensor_id = 'WM02')

    pass

def start_pc():

    pcounter(adc, data_directory = '/home/pi/Datalog/', sensor_channel = '0', sensor_id='PC01')
    pass

#pcounter(data_directory = '/home/pi/Datalog/', sensor_channel = '0', sensor_id='PC01', adc)
#watermeter(adc, data_directory = '/home/pi/Datalog/',sensor_port = '1', sensor_id = 'WM01')
#watermeter(adc, data_directory = '/home/pi/Datalog/',sensor_channel = '2', sensor_id = 'WM02')

thread_1 = threading.Thread(target=start_meter_1)
thread_2 = threading.Thread(target=start_meter_2)
thread_3 = threading.Thread(target=start_pc)

lock = threading.Lock()

#Starting threads

thread_1.start()
thread_2.start()
thread_3.start()

