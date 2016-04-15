#AggieWater Smart Water Metering Project

This repository contains files and documentation for a smart water metering solution being researched and developed by the Utah Water Research Laboratory at Utah State University using low cost commercially available computers and sensors. This system can turn most possitive displacement water meters with detachable register into a smart water meter with capabilities to store high frequency flowrate data for up to a week in a battery run. 


![alt tag](https://github.com/UCHIC/WaterMonitor/blob/master/doc/images/AGGIEWATER_SYSTEM_CLOSEUP3.png)

1- Datalogger using Raspberry pi and custom made pi hat.  
2- Hall effect senser wired to the datalogger.  
3- Battery pack.  
4- Power on and off connector.  
5- Weather proff case.   
#Getting Started
![alt tag](https://github.com/UCHIC/WaterMonitor/blob/master/doc/images/AGGIEWATER_PACK.png)
###Setting up and starting the system
To start using the this system just plug the power on/off connector and wait 20 seconds for the system to boot. Make sure your battery is fully charge to extract the most of the data collection run. Once the system is up, plug an ethernet cable between the Raspberry and a laptop computer and follow the intructions below.

####Change laptop computer network adapter ip address

Right-click on the network adapter you want to assign an IP address and click Properties. Highlight Internet Protocol Version 4 (TCP/IPv4) then click the Properties button. Now change the IP, Subnet mask, Default Gateway, and DNS Server Addresses. When you're finished click OK. This setting is only  needed once if the same laptop computer is used to communicate to the AggieWater system.

![alt tag](https://github.com/UCHIC/WaterMonitor/blob/master/doc/images/static_ip.png)

####SSH into the Raspberry Pi
We have assigned an unique static IP address to all the prototypes we have created that can be found in a label on the lid of the weather proff case. The label also contains user and password information to access the Raspberry Pi.
#####Windows
Use Putty or any other terminal emulator.  

![alt tag](https://github.com/UCHIC/WaterMonitor/blob/master/doc/images/putty.png)
#####Mac and linux
Use the system default terminal type the following and hit enter:  

   <pre>
   ssh pi@192.168.0.XX
   </pre>

When prompted for credentials type the password provided and hit enter.  

###Calibration
###Data retrieval



##License


##Sponsors and Credit



