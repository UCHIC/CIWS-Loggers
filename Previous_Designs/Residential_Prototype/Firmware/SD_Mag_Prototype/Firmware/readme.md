This firmware is for the more power-efficient datalogger prototype based around the Atmel ATmega328 microcontroller. The corresponding hardware files can be found in CIWS-Loggers/hardware/<insert_file_path_when_created>. This source code was written in the Arduino 1.8.8 IDE.

This prototype uses a magnetometer strapped to the outside of a register instead of a hall-effect sensor sandwiched between a register and meter.

Peak-counting algorithm verified. It works properly.

DONE: Fixed bug: Datalogger detects proper number of peaks, but logs completely different number. Write to serial to determine issue.

TODO: Find a way to ignore the preliminary noise when the logger first starts to log.
