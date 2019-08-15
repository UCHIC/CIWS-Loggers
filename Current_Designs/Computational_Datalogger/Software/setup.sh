#!/bin/bash
# This bash script builds and installs the Logger module for python 2.7.
# Dependancies: python-dev
#		wiringpi

echo "Checking /boot/cmdline.txt for spidev buffer size..."

grep -q -F 'spidev.bufsiz=65536' /boot/cmdline.txt
if [ $? -ne 0 ]; then
	ed -s /boot/cmdline.txt < <(printf '%s\n' 1 a " spidev.bufsiz=65536" . 1,2j w q)
	echo "Spidev buffer size added. Reboot required in order to complete setup."
fi

echo "Building module Logger"

rm -f -r build
python setup.py build
python setup.py install

echo "Setup finished."
