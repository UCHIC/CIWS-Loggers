#!/usr/bin/env python

#Water meter monitor
#Written by: Miguel Leonardo
#Version: Beta2.0
#Using ADC pi hat

from lib_include import *
from conf_reader import *


_ntuple_diskusage = namedtuple('usage', 'total used free')


#------------------------------------------------------------------
log.setLevel(logging.DEBUG)
#log.set_udp_destination('estimated.local')
log.set_udp_destination('localhost')


def signal_handler(signal, frame):
        print 'You pressed Ctrl+C!'
        sys.exit(0)
signal.signal(signal.SIGINT, signal_handler)
print 'Press Ctrl+C to exit'

#Sensor_Module Class start here
#------------------------------------------------------------------
class SensorModule():
    def __init__(self,data_directory = Data_directory, enumerate_directory = True, time_support = Time_support):
        self._ntuple_diskusage = namedtuple('usage', 'total used free')

        enumerate_data_directory = enumerate_directory
        #Accessing DataLogger directory - This will create the directory if not exist
        if enumerate_data_directory:
            directorynamer = Filenamer('DataLog_%05d/')
            self.data_directory = join_filename(data_directory, directorynamer.next_filename())
            while exists(self.data_directory):
                self.data_directory = join_filename(data_directory, directorynamer.next_filename())
        else:
            self.data_directory = data_directory

        print self.data_directory 
        if not exists(self.data_directory):
            makedirs(self.data_directory)
            pass

        #Opening/Creating and initializing csv file in write mode
        #Initializing Datalog Files
        self._output_file = open(self.data_directory + 'sensors_datalog' + '.csv', 'w')
        self._write = csv.writer(self._output_file, delimiter=',', quotechar='"', quoting=csv.QUOTE_MINIMAL)


        header1 = ["Datalogger Name:",Datalogger_name]
        header2 = ["Site Name:",Site_name]
        header3 = ["Site Descrption:",Site_description]
        header4 = ["TIMESTAMP","RECORD","MEM_SPACE_AVAILABLE",Port1_name, Port2_name, Port3_name, Port4_name]
        self._write.writerow(header1)
        self._write.writerow(header2)
        self._write.writerow(header3)
        self._write.writerow(header4)
        self._output_file.close()
##
##
##
##        #Starting ADC
##        self._adc = ADS1x15(address=ADC2,ic=ADS1015)
        self._start = True
##
        #Datalogger interval settings

        self._sample_time = float(time_support)
        self._count = 0
        self._count1 = 0
        self._pulse_count0 = 0
        self._pulse_count1 = 0
        self._pulse_count2 = 0
        self._pulse_count3 = 0
        self._threshold0 = 3
        self._threshold1 = 3
        self._threshold2 = 3
        self._threshold3 = 3

        self._MotionPause = 5

        self._record = 0

        self._conv_factor1 = Conv_factor1
        self._conv_factor2 = Conv_factor2
        self._conv_factor3 = Conv_factor3
        self._conv_factor4 = Conv_factor4

        self._time_support = float(time_support)


        #Defining data buffers
        
        self._intarray = array('i')
        self._intarray1 = array('i')
        self._intarray2= array('i')
        self._intarray3 = array('i')

        self._reading_1 = 0
        self._reading_2 = 0
        self._reading_3 = 0
        self._reading_4 = 0


        #Starting system monitor
        #sm.system_monitor()

        #Go
        self._set_MCP3008()
        self._run()


        pass

    def _run(self):
        capture = True
        sample = 0
        while self._start: 
            time_lapse = 0
            acquire = True
            self._startTime = time.time()
            
            while acquire:
                #This section reads the voltage levels on the 4 port ADCs
                #--------------------------------------------------------------------------------------------------------    
                self._volts0 = self._get_adc1()
                self._volts1 = self._get_adc2()
                self._volts2 = self._get_adc3()
                self._volts3 = self._get_adc4()
                #print(self._volts0, self._volts1, self._volts2, self._volts3)
                #--------------------------------------------------------------------------------------------------------

                #This section converts input data into pulses
                #--------------------------------------------------------------------------------------------------------
                self._intarray.append(self._pulse_detection(self._volts0, self._threshold0))
                self._intarray1.append(self._pulse_detection(self._volts1, self._threshold1))
                self._intarray2.append(self._pulse_detection(self._volts2, self._threshold2))
                self._intarray3.append(self._pulse_detection(self._volts3, self._threshold3))
                #--------------------------------------------------------------------------------------------------------
                
                #Pulse count mode definition 1)Count X occurrences, 2)Count falling edge, 3)Count rising edge.
                #Make adjustments here, if required.
                #--------------------------------------------------------------------------------------------------------
                self._reading_1 = self._falling_edge_count(self._intarray)
                self._reading_2 = self._falling_edge_count(self._intarray1)
                self._reading_3 = self._falling_edge_count(self._intarray2)
                self._reading_4 = self._falling_edge_count(self._intarray3)
                #--------------------------------------------------------------------------------------------------------
                #print(self._reading_1, self._reading_2, self._reading_3, self._reading_4)


                #This section creates the wait time based on the data collection time frame defined
                #--------------------------------------------------------------------------------------------------------
                self._duration = (time.time() - self._startTime)
                if self._duration >= self._sample_time:
                    acquire = False
                    self._record = self._record + 1
                    
                    #Dataloging
                    self._capture_wm(self._reading_1, self._reading_2, self._reading_3, self._reading_4, self._write)
                    self._print_flowrate(self._reading_1, self._reading_2, self._reading_3, self._reading_4)
                    #Reinitialize buffers and variables
                    self._reset()
                #--------------------------------------------------------------------------------------------------------
                #Debug
                #self._print_readings(self._reading_1, self._reading_2, self._reading_3, self._reading_4)
            pass
    def _print_readings(self,reading1, reading2, reading3, reading4):

        os.system('clear')
        print 'Duration: %.3f' %self._duration
        print '---------------------'
        print 'ADC0: %.6f' %(self._volts0) + 'Count: %d' %reading1 
        print 'ADC1: %.6f' %(self._volts1) + 'Count: %d' %reading2
        print 'ADC2: %.6f' %(self._volts2) + 'Count: %d' %reading3
        print 'ADC3: %.6f' %(self._volts3) + 'Count: %d' %reading4
        print '---------------------'
        print self._disk_usage("/")
        print 'Press Ctrl+C to exit'
        pass
    def _print_flowrate(self,reading1, reading2, reading3, reading4):

        os.system('clear')
        print 'Duration: %.3f' %self._duration
        print '---------------------'
        print 'ADC0: %.6f' %(self._volts0) + ' FLowrate: %f' %self._rev_to_gpm(reading1, self._conv_factor1, self._time_support) + ' GPM %s' %Port1_name
        print 'ADC1: %.6f' %(self._volts1) + ' Flowrate: %f' %self._rev_to_gpm(reading2, self._conv_factor2, self._time_support) + ' GPM %s' %Port2_name
        print 'ADC2: %.6f' %(self._volts2) + ' Flowrate: %f' %self._rev_to_gpm(reading3, self._conv_factor3, self._time_support) + ' GPM %s' %Port3_name
        print 'ADC3: %.6f' %(self._volts3) + ' Flowrate: %f' %self._rev_to_gpm(reading4, self._conv_factor4, self._time_support) + ' GPM %s' %Port4_name
        print '---------------------'
        #print self._disk_usage("/")
        print 'Press Ctrl+C to exit'
        pass
    def _rev_to_gpm(self, reading, conv_factor, time_support):

        volume = reading*conv_factor #Gallons per time support
        gpm = (volume/time_support)*60
        return gpm

    def _rev_to_volume(self, reading, conv_factor, time_support):

        volume = reading*conv_factor #Gallons
        return volume


    def _capture_wm(self, Reading1, Reading2, Reading3, Reading4, write):

        Record = self._record
        MEM_space_available = self._disk_usage("/")
        Bat_Volt = Reading2

        Time = time.time()
        Date_Time = date.datetime.now()
        data = [Date_Time, Record,MEM_space_available, Reading1, Reading2, Reading3, Reading4]
        #data = [Date_Time, Record,MEM_space_available, Bat_Volt, Reading4]
        self._log_data_csv(data)

        pass

    def _pulse_detection(self, reading, threshold):

        if reading > threshold:
            pulse = 1
        else:
            pulse = 0
        return pulse

    def pulse_count(self,array):
        count = array.count(1)
        return count

    def _falling_edge_count(self, array):

        count = 0
        if array.count(1) > 0:
           for elements in range(0,len(array)-1):
               previous_element = array[elements]
               next_element = array[elements+1]

               if array[elements]==1:
                  if array[elements+1]==0:
                          count = count + 1
        return count

    def _log_data_csv(self, data):

        self._output_file = open(self.data_directory + 'sensors_datalog' + '.csv', 'a')
        self._write = csv.writer(self._output_file, delimiter=',', quotechar='"', quoting=csv.QUOTE_MINIMAL)
        self._write.writerow(data)
        self._output_file.close()
        
        pass

    def _reset(self):
    
        self._pulse_count0 = 0
        self._pulse_count1 = 0
        self._pulse_count2 = 0
        self._pulse_count3 = 0
        self._intarray = array('i')
        self._intarray1 = array('i')
        self._intarray2 = array('i')
        self._intarray3 = array('i')
        pass

    def _disk_usage(self, path):    
        st = os.statvfs(path)
        free = st.f_bavail * st.f_frsize
        total = st.f_blocks * st.f_frsize
        used = (st.f_blocks - st.f_bfree) * st.f_frsize
        self._ntuple_diskusage(total, used, free)
        return free

    def _set_MCP3008(self):
        GPIO.setwarnings(False)

        GPIO.setmode(GPIO.BCM)

        self._SPICLK = 18
        self._SPIMISO = 23
        self._SPIMOSI = 24
        self._SPICS = 25

        # set up the SPI interface pins
        GPIO.setup(self._SPIMOSI, GPIO.OUT)
        GPIO.setup(self._SPIMISO, GPIO.IN)
        GPIO.setup(self._SPICLK, GPIO.OUT)
        GPIO.setup(self._SPICS, GPIO.OUT)

        self._battery_adc = 0
        self._adc1 = 1
        self._adc2 = 3
        self._adc3 = 5
        self._adc4 = 7
        pass
    def _get_battery_volt(self):
        
        battery_read = self._readadc(self._battery_adc, self._SPICLK, self._SPIMOSI, self._SPIMISO, self._SPICS)
        battery_voltage = float((battery_read*(4.2/1023))*3)
        return battery_voltage
    def _get_adc1(self):
        adc_read = self._readadc(self._adc1,self._SPICLK, self._SPIMOSI, self._SPIMISO, self._SPICS)
        adc_volt = float(adc_read*(4.2/1023))
        return adc_volt
    def _get_adc2(self):
        adc_read = self._readadc(self._adc2,self._SPICLK, self._SPIMOSI, self._SPIMISO, self._SPICS)
        adc_volt = float(adc_read*(4.2/1023))
        return adc_volt
    def _get_adc3(self):
        adc_read = self._readadc(self._adc3,self._SPICLK, self._SPIMOSI, self._SPIMISO, self._SPICS)
        adc_volt = float(adc_read*(4.2/1023))
        return adc_volt
    def _get_adc4(self):
        adc_read = self._readadc(self._adc4,self._SPICLK, self._SPIMOSI, self._SPIMISO, self._SPICS)
        adc_volt = float(adc_read*(4.2/1023))
        return adc_volt

    # read SPI data from MCP3008 chip, 8 possible adc's (0 thru 7)
    def _readadc(self,adcnum, clockpin, mosipin, misopin, cspin):
        if ((adcnum > 7) or (adcnum < 0)):
                return -1
        GPIO.output(cspin, True)

        GPIO.output(clockpin, False)  # start clock low
        GPIO.output(cspin, False)     # bring CS low

        commandout = adcnum
        commandout |= 0x18  # start bit + single-ended bit
        commandout <<= 3    # we only need to send 5 bits here
        for i in range(5):
                if (commandout & 0x80):
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
                if (GPIO.input(misopin)):
                        adcout |= 0x1

        GPIO.output(cspin, True)
        
        adcout >>= 1       # first bit is 'null' so drop it
        return adcout
        
    def __del__(self):
        print 'Files Closed'
        pass


def main():

    a = SensorModule()

    pass

main()

    
    
