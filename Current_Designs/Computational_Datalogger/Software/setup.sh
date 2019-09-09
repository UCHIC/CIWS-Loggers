#!/bin/bash
# This bash script builds and installs the Logger module for python 2.7.
# Dependancies: python-dev
#		wiringpi

# TODO: need to setup UART. UART setup is a bit funky.
#	1) Add enable_uart=1 to /boot/config.txt
#	2) Add core_freq=250 to /boot/config.txt (or whatever core frequency we wanna run at)
#	3) sudo systemctl stop serial-getty@ttyS0.service
#	4) sudo systemctl disable serial-getty@ttyS0.service
#	5) Remove console=serial0,115200 from /boot/cmdline.txt

# NEW TODO: Method is unreliable. Need to simply replace existing files with our own.

echo "Checking /boot/cmdline.txt for spidev buffer size..."

grep -q -F 'spidev.bufsiz=65536' /boot/cmdline.txt
if [ $? -ne 0 ]; then
	ed -s /boot/cmdline.txt < <(printf '%s\n' 1 a " spidev.bufsiz=65536" . 1,2j w q)
	echo "Spidev buffer size added. Reboot required in order to complete setup."
fi

echo "Checking /boot/cmdline.txt for enabled serial console..."

grep -q -F 'console=serial0,115200' /boot/cmdline.txt
if [ $? -eq 0 ]; then
	sed -i -e 's/console=serial0,115200//g' /boot/cmdline.txt
	echo "Disabled Serial Console. Reboot required in order to complete setup."
fi

echo "Checking /etc/rc.local to disable serial-getty service..."

grep -q -F 'sudo systemctl disable serial-getty@ttyS0.service' /etc/rc.local
if [ $? -ne 0 ]; then
	echo 'sudo systemctl stop serial-getty@ttyS0.service' >> /etc/rc.local
	echo 'sudo systemctl disable serial-getty@ttyS0.service' >> /etc/rc.local
	echo "Disabled serial-getty service. Reboot required in order to complete setup."
fi

echo "Checking /boot/config.txt for enabled UART..."

grep -q -F 'enable_uart=1' /boot/config.txt
if [ $? -ne 0 ]; then
	echo 'enable_uart=1' /boot/config.txt
	echo 'core_freq=250' /boot/config.txt
fi

echo "Building module Logger"

rm -f -r build
rm -f /usr/local/lib/python2.7/dist-packages/Logger*
python setup.py build
python setup.py install

echo "Setup finished."
