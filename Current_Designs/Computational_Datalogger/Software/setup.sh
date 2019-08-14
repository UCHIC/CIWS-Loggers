#!/bin/bash
# This bash script builds and installs the Logger module for python 2.7.
# Dependancies: python-dev
#		wiringpi

python setup.py build
python setup.py install
