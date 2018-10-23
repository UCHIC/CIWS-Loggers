// Firmware for the CIWS Residential Datalogger
// Arduino IDE ver. 1.8.7
// Utah Water Research Lab
// Updated: 10/23/2018
// Note: F("String") keeps string literals in program memory and out of RAM. Saves RAM space. Very good. Don't remove the F. I know it looks funny. But don't do it. Really. The program might crash if you do. And then you'll have dishonor on yourself, dishonor on your cow...

/*******************************************************************************************\
* Hardware Description
*                     ___________________                             ___
*                    |                   |<------------------------->[___] RTC  
*  ________          |                   |                            _______
* |        |         |                   |<------------------------->|       | SD
* |        |<------->|                   |<-----[] Activate Serial   |______/
* |________|         |___________________|<-----[] Sensor
*  Serial             Controller
* 
* Serial:          Serial interface for user interaction with the logger
* Controller:      Arduino Pro/Pro Mini (used for SD library and lower power options)
* SD:              External SD Card storage for the logger.
* Activate Serial: Button to wake up the controller and activate the Serial interface
* Sensor:          Hall Effect Sensor for logging pulses from water meters.
* RTC:             Real Time Clock to track time and wake up controller every four seconds.
\*******************************************************************************************/

/*********************************************************\
* Software Description
* Overview: 
*   User inputs:
*     Serial input.
*   Device outputs:
*     Serial output
*     Datalog file
*     
* Structure:
*   1. Setup
*   2. Handle Serial (for start-up configuration)
*   3. Loop
*        Enter Sleep (low-power mode)
*        < Will wake up on Interrupt and continue: >
*        If serial is enabled
*          Handle serial input
*        If four seconds are up
*          Update the time
*          Construct a timestamp
*          Power-on the SD card
*          Write data
*          Power-off the SD card
*      Repeat Loop
*      
* Interrupts:
*   1. INT0 ()
*      
*   2. INT1 ()
*      
\*********************************************************/

#include <SPI.h>
#include <SD.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include "handleSerial.h"

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
