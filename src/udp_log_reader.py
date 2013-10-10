import logging
#import cPickle as pickle
import pickle
import socket
import struct

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind(('0.0.0.0', 49999))

handler = logging.StreamHandler()
logger = logging.getLogger('')
logger.addHandler(handler)

try:
    while True:
        dgram = sock.recv(4096)
        data_size = struct.unpack('!L', dgram[:4])[0]
        data = dgram[4:]
        if data_size != len(data):
            print ('received a partial packet')
            continue
        log_entry = logging.makeLogRecord(pickle.loads(data))
        logger.handle(log_entry)
except KeyboardInterrupt:
    pass

