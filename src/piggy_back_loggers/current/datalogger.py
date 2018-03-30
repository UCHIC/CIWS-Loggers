# -----------------------------------------------------------------
# Main datalogger script that for logging data on a residential
# water meter using a Raspberry Pi, the MCP3008 analog to digital
# converter, and a Hall effect sensor
# Created by: Jeff Horsburgh using code originally writen by Miguel
# Leonardo
# -----------------------------------------------------------------
import time
import datetime as date
from array import array
from adc import *
from config import *
from filenamer import Filenamer
from utility_functions import *
from os.path import join as join_filename

# Set up the directory to which the output data file will be written
# ------------------------------------------------------------------
# Each datalogger output file is written to is own directory
# within the folder named in the config.py file
directory_namer = Filenamer('DataLog_%05d/')
data_directory = join_filename(BaseDataDirectory, directory_namer.next_filename())
while os.path.exists(data_directory):
    data_directory = join_filename(BaseDataDirectory, directory_namer.next_filename())
print data_directory
# If the directory doesn't already exist, create it
if not os.path.exists(data_directory):
    os.makedirs(data_directory)

# Create and initialize csv output file in write mode
# ---------------------------------------------------
column_header = ["TimeStamp", "RecordNumber", "Mem_Space_Available", "Battery_Voltage",
                 Port1Name, Port2Name, Port3Name, Port4Name]
print (','.join(map(str, column_header)))

output_file = initialize_output_file(data_directory, DataloggerName, SiteName,
                                     SiteDescription, column_header)

# Configure the ADC on the Pi Hat
# -------------------------------
# Create an instance of the ADC class that is ready to go
adc = ADC()

# Set up some data collection variables
# -------------------------------------
# Initialize the output record number
record = 0
# set the time support in seconds from the config file
time_support = float(TimeSupport)
# The following variables store the voltage threshold above which a pulse is detected
threshold0 = 3
threshold1 = 3
threshold2 = 3
threshold3 = 3

# Initialize data buffers and pulse counters
# ------------------------------------------
# Initialize the data buffers as integer arrays
intarray0 = array('i')
intarray1 = array('i')
intarray2 = array('i')
intarray3 = array('i')
# Initialize the pulse counters
count1 = 0
count2 = 0
count3 = 0
count4 = 0

# Main program loop
# -----------------
while True:
    acquire = True

    # Get the current time at the start of this loop iteration
    startTime = time.time()

    while acquire:
        # Read the voltage levels on the 4 ADC ports as fast as the Raspberry Pi will go
        volts0 = adc.get_adc1()
        volts1 = adc.get_adc2()
        volts2 = adc.get_adc3()
        volts3 = adc.get_adc4()
        # print(volts0, volts1, volts2, volts3)  # Uncomment this line to debug

        # Convert input voltage data into pulses
        # This code converts the continuous voltage data into a square wave with values of 0 or 1
        # If the voltage read is less than the threshold, a value of 0 is recorded in the array
        # If the voltage read is greater than the threshold, a value of 1 is recorded
        intarray0.append(pulse_detection(volts0, threshold0))
        intarray1.append(pulse_detection(volts1, threshold1))
        intarray2.append(pulse_detection(volts2, threshold2))
        intarray3.append(pulse_detection(volts3, threshold3))

        # Check the current time to see if it's time to record data
        duration = time.time() - startTime
        if duration >= time_support:  # A sampling interval has passed and we need to record data
            acquire = False

            # Increment the record counter
            record += 1

            # Count the pulses in the data buffer arrays
            # Pulse count mode definition can be one of three modes
            #   1) Count X occurrences,
            #   2) Count falling edge,
            #   3) Count rising edge.
            # Currently using falling edge count. Make adjustments here, if required.
            count1 = falling_edge_count(intarray0)
            count2 = falling_edge_count(intarray1)
            count3 = falling_edge_count(intarray2)
            count4 = falling_edge_count(intarray3)
            # print(count1, count2, count3, count4)  # Uncomment this line to debug

            # Log data to the output file
            date_time = date.datetime.now()
            mem_space_available = disk_usage("/")
            batt_volt = adc.get_battery_voltage()

            # Create a list of data items and write a line of output
            # to the output file
            data_list = [date_time, record, mem_space_available, batt_volt, count1, count2, count3, count4]
            print (','.join(map(str, data_list)))
            write_csv_data(output_file, data_list)

            # Reinitialize the data buffer arrays
            intarray0 = array('i')
            intarray1 = array('i')
            intarray2 = array('i')
            intarray3 = array('i')
