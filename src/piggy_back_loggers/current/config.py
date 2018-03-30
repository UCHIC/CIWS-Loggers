# ----------------------------------------------------------------------
# This file is used to set the configuration settings for the datalogger
# ----------------------------------------------------------------------
# Brief name or identifier for the datalogger.
# For this, we are using the participant ID.
DataloggerName = "0003"

# Brief name and description for the site at which the datalogger is installed
# For SiteDescription, use the name of the water meter
SiteName = "Logan City Site 0003"
SiteDescription = "Neptune T10 Residential Water Meter"

# Column name labels and units for the inputs plugged into the 4 analog
# Ports on the datalogger. Use "N/A" if nothing is plugged in.
Port1Name = "Water_Meter_Pulses"
Port2Name = "N/A"
Port3Name = "N/A"
Port4Name = "N/A"

# Time support for the datalogger measurements (recording interval) in seconds
TimeSupport = "4"

# Directory to which the data files should be written
BaseDataDirectory = "Datalog/"

# Conversion factors for converting meter pulses to gallons
# These conversion factors are in units gal/pulse
Conv_factor1 = 0.01
Conv_factor2 = 0.01
Conv_factor3 = 0.01
Conv_factor4 = 0.01


