// Firmware for the CIWS Residential Datalogger
// Arduino IDE ver. 1.8.7
// Utah Water Research Lab
// Updated: 10/25/2018
// Daniel Henshaw and Josh Tracy
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

// DANIEL
/*********************************************************************************\
 * Setup: Declare variables
 *          May need to be global (If not global, this goes in void setup()
 *          Put in a "state" struct? A state struct can just go in a header file.
\*********************************************************************************/

void setup() 
{
  // Initialize Serial Library
}

void loop() 
{
  // JOSH
  /*****************************************\
  * ButtonCheck: Is the button pressed?
  * If button is pressed:
  *   Set serialActive flag.
  \*****************************************/  
  if(/*buttonPressed*/)
    /*serialActive*/ = true;
    
  // JOSH
  /*****************************************\
  * Serial: User I/O over serial
  * serialActive flag is set:
  *   call function handleSerial();
  \*****************************************/
  if(/*serialActive*/)
    handleSerial(/*State*/);

  // DANIEL
  /*****************************************\
  * 4-second update: Update at 4 seconds
  * If 4-second flag is set:
  *   <Pseudocode>
  \*****************************************/

  // JOSH
  /*****************************************\
  * Sleep: put processor to sleep
  *        to be woken by interrupts
  * If sleepEnable is set:
  *   call function Sleep();
  \*****************************************/
  if(/*sleepEnable*/)
    Sleep();
}
