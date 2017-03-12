#Brief decription of the source code for AggieWater devices used for logging data on campus residential building water meters.

###meterlog.py

This script is run to log data from a single commercial water meter with a 4-20 mA output. The current output is converted to a voltage using a precision resistor and then measured on the Raspberry Pi using an analog to digital converter (ADC) breakout board.

###multi_meterlog.py

This script is run to log data from multiple water meters - some with 4-20 mA outputs and one with a pulsed output. For the 4-20 mA meters, the current output is converted to a voltage using a precision resistor and then measured on the Raspberry Pi as a voltage using an ADS1015 analog to digital converter (ADC) breakout board. For the pulsed output meter, a simple hardware debounce circuit is used to debounce the reed switch in the pulsed output meter and the pulses from the meter are read on one of the Raspberry Pi's GPIO ports. This script was written for campus buildings where there are multiple meters required to get the water balance for the building (e.g., hot water supply, cold water supply, and hot water return).

###pulse_counter.py

This is an example script for testing the hardware debouncing circuit for the pulsed output meters and for calculating flows from the pulsed output.

###pulse_detector.py

This is an example script for testing the hardware debouncing circuit for the pulsed output meters.

###test.py

This is an example script for how to use the ADS1015 analog to digital converter (ADC) breakout board to measure an analog input voltage using a Raspberry Pi.

