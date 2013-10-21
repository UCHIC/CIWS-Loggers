#from basesensor import BaseSensor
from datetime import datetime
from xml.dom.minidom import Element
 
class DateTimeSensor():
    def __init__(self, format='%Y-%m-%d %H:%M:%S.%f'):
        self._format = format
 
    def capture(self):
        self._captured_datetime = datetime.now()
 
    def fetch(self, output_directory):
        '''
        The output_directory parameter is ignored.
        '''
 
        formatted_datetime = self._captured_datetime.strftime(self._format)
 
        element = Element('DateTime')
        element.setAttribute('CurrentDateTime', formatted_datetime)
        return element
 
if __name__ == '__main__':
    sensor = DateTimeSensor()
 
    sensor.capture()
    print (sensor.fetch(None).toxml())
 
    #sensor.capture()
    #print (sensor.fetch(None).toxml())
 
    #sensor.capture()
    #print (sensor.fetch(None).toxml())
