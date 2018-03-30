import logging
import logging.handlers
 
_logger = logging.getLogger('')
_udp_handler = None
 
def setLevel(level):
    global _logger, _udp_handler
 
    _logger.setLevel(level)
    if _udp_handler is not None:
        _udp_handler.setLevel(level)
 
def set_udp_destination(host, port=49999):
    global _logger, _udp_handler
 
    _udp_handler = logging.handlers.DatagramHandler(host, port)
    _udp_handler.setLevel(_logger.level)
 
    _logger.addHandler(_udp_handler)
