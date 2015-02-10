# Import smtplib for the actual sending function
import smtplib
import time
# Import the email modules we'll need
##from email.mime.text import MIMEText
##
### Open a plain text file for reading.  For this example, assume that
### the text file contains only ASCII characters.
##textfile = 'conf.xml'
##fp = open(textfile, 'rb')
### Create a text/plain message
##msg = MIMEText(fp.read())
##fp.close()
##
##me ='ing.mleonardo@gmail.com'
##you = 'ing.mleonardo@gmail.com'
##msg['Subject'] = 'The contents of %s' % textfile
##msg['From'] = me
##msg['To'] = you
##
##print msg
##
### Send the message via our own SMTP server, but don't include the
### envelope header.
###s = smtplib.SMTP('smtp.gmail.com')
###s.sendmail(me, [you], msg.as_string())
###s.quit()

username = 'ing.mleonardo'
password = 'ML092986'


fromaddr = 'ing.mleonardo@gmail.com'
toaddrs  = 'miguel.leonardo@usu.edu'
msg = 'Raspberry01 - All Systems Working'

while True:
    
    server = smtplib.SMTP("smtp.gmail.com:587")

    server.starttls()

    server.login(username,password)

    server.sendmail(fromaddr, toaddrs, msg)

    server.quit()

    time.sleep(3600)

