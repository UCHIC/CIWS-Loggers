WaterMonitor
============

Code Related Logging Data using Embedded Computers

Requirements:
-Raspberry PI versions A, B and B+
-Debian GNU/Linux installed


Modify the Kernel to enable I2C modules on the raspberry Pi
=======================================
Follow these instructions using the LXterminal:
Run the following command:
sudo nano /etc/modules

Add to the end of the file:
i2c-bcm2708
i2c-dev


Run the following commands:
sudo apt-get install python-smbus
sudo apt-get install i2c-tools

Run the following command:
sudo nano /etc/modprobe.d/raspi-blacklist.conf

Add # to the lines that appered on the file so that they look as the following:

blacklist spi-bcm2708

blacklist i2c-bcm2708

Reboot the Raspberry Pi and you are ready to go!!!


