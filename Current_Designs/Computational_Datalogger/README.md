The Computational Datalogger comes in two parts. One part is a Raspberry Pi Embedded Computer. The second is the data acquisition device, similar to the Residential Datalogger.

The data acquisition device is primarily responsible for processing magnetometer data and storing flow data. Secondarily, it controls power to the Raspberry Pi in order to conserve battery life. The Raspberry Pi retreives the flow data and processes it to identify water end-usage.

The `Firmware` directory holds the firmware for the data acquisition device, or AVR Datalogger. The `Software` directory holds the software for the Raspberry Pi.

This datalogger is still under development.

