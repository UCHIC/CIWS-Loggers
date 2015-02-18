# Import smtplib for the actual sending function
import smtplib
import time
import socket
import thread

from email.MIMEMultipart import MIMEMultipart
from email.MIMEText import MIMEText

from conf_reader import *



class system_monitor():

    def __init__(self, username = 'ing.mleonardo',password = 'ML092986',fromaddr = 'ing.mleonardo@gmail.com', toaddrs  = 'ing.mleonardo@gmail.com',subject = 'Water Monitor', system_name = Datalogger_name , site_name = Site_name):

        #Initializing parameters
        self._username = username
        self._password = password
        self._fromaddr = fromaddr
        self._toaddrs  = toaddrs
        self._subject = subject
        self._system_name = system_name
        self._site_name = site_name
        self._monitor_interval = 3600 #Monitor interval in seconds

        

        self._start = True
        
        #Start email report
        self._thread = thread.start_new_thread(self.run,())
 
        pass

    def run(self):

        while self._start:
            self.send_email()
            time.sleep(self._monitor_interval)

    def send_email(self):
        #Creating message
        #Connecting to google server
        self._server = smtplib.SMTP("smtp.gmail.com:587")

        self._server.starttls()
        
        self._server.login(self._username,self._password)
        
        msg = MIMEMultipart()
        msg['From'] = self._fromaddr
        msg['To'] = self._toaddrs
        msg['Subject'] = self._subject
        body = '%s' %self._system_name + '- All Systems Working at %s' %self._site_name + ' %s ' %self.get_ip()
        msg.attach(MIMEText(body, 'plain'))

        #Sending message if connection available
        self._server.sendmail(self._fromaddr, self._toaddrs, msg.as_string())

    def get_ip(self):
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        s.connect(("gmail.com",80))
        ip = s.getsockname()[0]
        #print(ip)
        s.close()
        return ip

    def __del__(self):
        self._server.quit()
        self._thread.exit_thread()
        
        pass



def main():

    system_monitor()
    #thread.start_new_thread(system_monitor,())
    #print 'Done'
    #sm.send_email()

    pass

main()
