"""
Listen to UDP, serial, return most recent numeric values
Lots of help from here:
http://stackoverflow.com/questions/1093598/pyserial-how-to-read-last-line-sent-from-serial-device
"""
from threading import Thread
import time
import serial
import socket
from time import sleep
import sys
import struct

#sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
#sock.bind(('0.0.0.0', 49999))

last_received = [0,0,0,0]
def receiving(sock):
    global last_received
    
    while True:
        try:
            data, addr = sock.recvfrom(4096) # buffer size is 1024 bytes
            #print struct.unpack('iiiii',data)

        except socket.timeout,e:

            error = e.args[0]

            if error == 'timed out':
                #sleep(1)
                last_received = [0,0,0,0]
                print 'recv timed out, retry later'
                continue
            else:
                print e
                #print 'Error here'
                sys.exit(1)

        except socket.error, e:
            # Something else happened, handle error, exit, etc.
            print e
            sys.exit(1)
        else:
            if len(data) == 0:
                print 'orderly shutdown on server end'
                sys.exit(0)
            else:
                last_received = struct.unpack("iiii",data)
                pass
            pass
            

class UDPData(object):
    def __init__(self, UDP_IP = '192.168.2.150',UDP_PORT = 00000):
        self._udp_ip = UDP_IP
        self._udp_port = UDP_PORT
        try:
            self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
            self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            self.sock.bind((self._udp_ip, self._udp_port))
            self.sock.settimeout(2)
        except socket.error, e:
            #no udp connection
            print e
            self.sock = None
        else:
            Thread(target=receiving, args=(self.sock,)).start()
            pass
        
    def next(self):
        if not self.sock:
            #data, addr = sock.recvfrom(1024) # buffer size is 1024 bytes
            #print "Not working"
            #return int(data) #return anything so we can test when Arduino isn't connected
            return 0
        return int(last_received[0])
    def __del__(self):
        if self.sock:
            self.sock.close()

if __name__=='__main__':
    s = UDPData('0.0.0.0',49999)
    for i in range(500):
        time.sleep(.015)
        print s.next()
