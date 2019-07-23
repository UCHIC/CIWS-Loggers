# ----------------------------------------------------
# A set of utility functions for the Raspberry Pi
# Water Meter Datalogger
# Created by: Jeff Horsburgh using code originally
# written by Miguel Leonardo
# ----------------------------------------------------
import os


def initialize_output_file(data_directory, datalogger_name, site_name, site_description, column_header):
    """
    Given information from the configuration file, create the output data
    file and write the header information to the top of it
    :param data_directory: Path to the directory in which the file will be located
    :param datalogger_name: Name of the datalogger
    :param site_name: Name of the site to which the datalogger will be deployed
    :param site_description: Description of the site to which the datalogger will be deployed
    :param column_header: List of column headers to write to the data file header
    :return: The full path to the output file, including the file name
    """
    output_file_name = data_directory + 'datalog' + '.csv'
    with open(output_file_name, 'w') as outputFile:
        outputFile.write('CIWS Data Logger Output File' + '\n')
        outputFile.write("DataloggerName: " + datalogger_name + '\n')
        outputFile.write("SiteName: " + site_name + '\n')
        outputFile.write("SiteDescription: " + site_description + '\n')
        outputFile.write(','.join(map(str, column_header)) + '\n')
    return output_file_name


def write_csv_data(file_name, csv_data):
    """
    Given the path to an output file and a list of data, write a CSV
    line to the file
    :param file_name: Full path the the file to which the data will be written
    :param csv_data: List of values to write to the file as comma separated values
    :return:
    """
    with open(file_name, 'a') as outputFile:
        outputFile.write(','.join(map(str, csv_data)) + '\n')


def disk_usage(path):
    """
    Determine the amount of free storage remaining
    :param path: Path to a directory
    :return: Amount of free storage
    """
    st = os.statvfs(path)
    free = st.f_bavail * st.f_frsize
    # total = st.f_blocks * st.f_frsize
    # used = (st.f_blocks - st.f_bfree) * st.f_frsize
    # self._ntuple_diskusage(total, used, free)
    return free


def pulse_detection(reading, threshold):
    """
    Return a value of 1 if an input voltage is above a threshold
    and 0 if the value is below the threshold
    :param reading: Input voltage
    :param threshold: Threshold to which the input voltage is compared
    :return: 0 if below threshold, 1 if above threshold
    """
    if reading > threshold:
        pulse = 1
    else:
        pulse = 0
    return pulse


def falling_edge_count(array):
    """
    Loop through all of the elements in the array and count the number
    of times that the value switches from 1 to 0 (falling edges)
    :param array: Input data buffer array - values are 0 or 1
    :return: Count of times the value in the array falls from 1 to 0 (number of pulses)
    """
    count = 0
    if array.count(1) > 0:  # Check to make sure there are elements in the array
        for elements in range(0, len(array) - 1):
            if array[elements] == 1:
                if array[elements + 1] == 0:
                    count += 1
    return count
