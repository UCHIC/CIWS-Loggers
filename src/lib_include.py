#!/usr/bin/env python
import logging
from time import sleep
import os
import sys
import subprocess
import log
#from ADC_driver import ADCdriver
import threading
import struct
import time, signal
#from Adafruit_ADS1x15 import ADS1x15
import csv
import datetime as date

# File manipulation libraries
#------------------------------------------------------------------
from os      import remove as remove_file
from os 	 import makedirs
from shutil  import copy   as copy_file
from os.path import join   as join_filename
from os.path import exists
from os.path import dirname
import os
from xml.dom.minidom import Element
from filenamer import Filenamer
from collections import namedtuple
from array import array
#------------------------------------------------------------------
from subprocess import Popen, PIPE, STDOUT


