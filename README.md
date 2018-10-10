## Cyberinfrastructure for Intelligent Water Supply (CIWS)

CIWS is a project funded by the U.S. National Science Foundation to develop cyberinfrastructure for smart water metering and high resolution water-use data collection. We are developing systems for high resolution residential water consumption data collection, processing and analysis.

This repository contains files and documentation for water use dataloggers under development to support a smart water metering effort at the Utah Water Research Laboratory at Utah State University. We are using low cost, commercially available, embedded computers and sensors to log data from existing analog residential water meters as well as commercial water meters with pulsed or current loop outputs (4-20 mA) on USU's campus. 

We are developing ways to turn most analog, positive displacement water meters with detachable registers into a smart water meter with capabilities to store high frequency flowrate data for up to a week or more in a battery run. We are also developing new cyberinfrastructure and techniques for collecting high frequency data from commercial water meters. 

The following is an example of the type of data we are collecting from individual residences:

![Data](/doc/images/data.png)
   
## Getting Started

Visit the [documentation page](/doc/readme.md) for instructions on how we are building and communicating with smart metering devices. 

Visit the [source code](https://github.com/UCHIC/ciws/tree/master/src) folder if you are interested in our data collection scripts for the dataloggers we are prototyping.

Visit the [hardware design](https://github.com/UCHIC/CIWS/tree/master/hardwaredesign) folder if you are interested in the custom data logging hardware we are using to enable high frequency data collection on both commercial meters with 4-20 mA outputs and residential meters with analog registers.

## Sponsors and Credits

This material is based on work supported by National Science Foundation Grant [CBET 1552444](http://www.nsf.gov/awardsearch/showAward?AWD_ID=1552444).  Any opinions, findings, and conclusions or recommendations expressed are those of the author(s) and do not necessarily reflect the views of the National Science Foundation.

![NSF](/doc/images/nsf.gif)
