# Import smtplib for the actual sending function
import smtplib
import time

username = 'ing.mleonardo'
password = 'ML092986'


fromaddr = 'ing.mleonardo@gmail.com'
toaddrs  = 'ing.mleonardo@gmail.com'
msg = 'Raspberry01 - All Systems Working'

while True:
    
    server = smtplib.SMTP("smtp.gmail.com:587")

    server.starttls()
    
    server.login(username,password)

    server.sendmail(fromaddr, toaddrs, msg)

    server.quit()

    time.sleep(3600)

