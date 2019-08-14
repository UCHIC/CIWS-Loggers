#!/bin/bash
# This bash script builds and installs the Logger module for python 2.7.
# Dependancies: python-dev

python setup.py build
python setup.py install
