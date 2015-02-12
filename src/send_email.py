# Import smtplib for the actual sending function
import smtplib
import time
import socket

from email.MIMEMultipart import MIMEMultipart
from email.MIMEText import MIMEText

username = 'ing.mleonardo'
password = 'ML092986'


fromaddr = 'ing.mleonardo@gmail.com'
toaddrs  = 'ing.mleonardo@gmail.com'

msg = MIMEMultipart()

msg['From'] = fromaddr
msg['To'] = toaddrs
msg['Subject'] = 'Water Monitor'
#body = 'Raspberry04 - All Systems Working at MBB'

#msg.attach(MIMEText(body, 'plain'))

while True:

    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.connect(("gmail.com",80))
    ip = s.getsockname()[0]
    print(ip)
    s.close()
    body = 'Raspberry04 - All Systems Working at MBB %s' %ip
    msg.attach(MIMEText(body, 'plain'))
    
    server = smtplib.SMTP("smtp.gmail.com:587")


    server.starttls()
    
    server.login(username,password)
    
    print body
    server.sendmail(fromaddr, toaddrs, msg.as_string())

    server.quit()

    time.sleep(3600)

