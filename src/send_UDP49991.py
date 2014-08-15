import socket

import logging
from time import sleep
import os
import sys
import subprocess
import log
from ADC_driver import ADCdriver
import threading
import fcntl
import termios
import struct
import time, signal
from Adafruit_ADS1x15 import ADS1x15

# file manipulation
from os      import remove as remove_file
from os 	 import makedirs
from shutil  import copy   as copy_file
from os.path import join   as join_filename
from os.path import exists
from os.path import dirname
import os
from xml.dom.minidom import Element

from filenamer import Filenamer

from cStringIO import StringIO
from subprocess import Popen, PIPE, STDOUT



UDP_IP = "192.168.2.150"
UDP_PORT = 49991
MESSAGE = ''

#MESSAGE += struct.pack("i",10)#Computer ID
MESSAGE += struct.pack("i",20)#Port 1
MESSAGE += struct.pack("i",20)#Port 2
MESSAGE += struct.pack("i",20)#Port 3
MESSAGE += struct.pack("i",20)#Port 4


print "UDP target IP:", UDP_IP
print "UDP target port:", UDP_PORT
print "message:", MESSAGE

while True:
	sock = socket.socket(socket.AF_INET, # Internet
                     socket.SOCK_DGRAM) # UDP
	sock.sendto(MESSAGE, (UDP_IP, UDP_PORT))
