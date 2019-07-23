## Source Code for "Piggy-back" Smart Metering Devices

The source code in this folder was created to enable using a Raspberry Pi Model 3 as a "Piggy-back" data logger on top of existing, analog residential water meters.

### datalogger.py

This is the main script to run the data collection system. 

### adc.py

This file contains a class and functions for working with the analog to digital converter.

### config.py

Use this file to set up and configure the datalogger prior to deployment.

### filenamer.py

This script contains a class and functions used by the main datalogger script to manage file creation and naming conventions.

### utility_functions.py

This script contains a set of utility functions used by the main datalogger script.

