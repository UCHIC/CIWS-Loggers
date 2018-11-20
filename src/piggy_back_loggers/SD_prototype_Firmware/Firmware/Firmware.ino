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
*        If serial is enabled
*          Handle serial input
*        If four seconds are up
*          Update the time
*          Construct a timestamp
*          Power-on the SD card
*          Write data
*          Power-off the SD card
*        If sleep is enabled (disabled when serial is enabled)
*          Enter Sleep (low-power mode)
*          < Will wake up on Interrupt and continue: >
*      Repeat Loop
*      
* Interrupts:
*   1. INT0_ISR()
*      
*   2. INT1_ISR()
*      
\*********************************************************/

#include <SPI.h>
#include <SD.h>
#include "handleSerial.h"
#include "powerSleep.h"
#include "state.h"
#include "RTC_PCF8523.h"

/*********************************************************************************\
 * Setup:
 *    System State structure      Complete
 *    Setup Digital I/O pins
 *      Pin 2 (INT0)
 *      Pin 3 (INT1)
 *      Pin 4 (SD power on/off)
 *      Pin 5 (Serial Activate Button)
 *    Setup interrupts
\*********************************************************************************/

volatile State_t State;           // System State structure
Date_t Date;                      // System Time and Date structure
File dataFile;                    // File pointer for SD Card operations

void setup() 
{
  resetState(&State);             // Setup the System State structure
  
  pinMode(2, INPUT);              // Setup the Digital Pins       
  pinMode(3, INPUT);
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);
  pinMode(5, INPUT);

  rtcTransfer(reg_Control_2, WRITE, 0x02);

  attachInterrupt(digitalPinToInterrupt(2), INT0_ISR, FALLING);   // Setup Interrupts
  attachInterrupt(digitalPinToInterrupt(3), INT1_ISR, FALLING);
  EIMSK &= ~(1 << INT0);        // Disable Hall Effect Sensor interrupt
  EIMSK |= (1 << INT1);         // Enable 4-Second RTC interrupt.
  
  loadDateTime(&Date);            // Load Date_t with Date/Time info
                                  
  disableUnneededPeripherals();   // Disable unneeded peripherals
}

void loop() 
{
  // JOSH
  /*****************************************\
  * ButtonCheck: Is the button pressed?
  * If button is pressed (active-low):
  *   Set serialOn flag.
  *   call serialPowerUp()
  \*****************************************/  
  if((digitalRead(5) == 0) && !State.serialOn)
  {
    State.serialOn = true;
    serialPowerUp();
  }
    
  // JOSH
  /*****************************************\
  * Serial: User I/O over serial
  * serialOn flag is set:
  *   call function handleSerial();
  \*****************************************/
  if(State.serialOn)
    handleSerial(&State, &Date);

  // DANIEL
  /*****************************************\
  * 4-second update: Update at 4 seconds
  * If 4-second flag is set:
  *   <Pseudocode> (I would recommend
  *                 putting this in a
  *                 function)
  \*****************************************/
  if(State.flag4)
  {
    State.flag4 = false;
    rtcTransfer(reg_Control_2, WRITE, 0x02);
  } 
    // Daniel's function
      // Don't forget to clear flag on RTC!

  // JOSH
  /*****************************************\
  * Sleep: put processor to sleep
  *        to be woken by interrupts
  * If serialOn is not set:
  *   call function Sleep();
  \*****************************************/
  if(!State.serialOn)
    enterSleep();
}

// DANIEL
/* Function Title: INT0_ISR()
 * 
 * Friendly Name:  Sensor Interrupt Service Routine (ISR)
 * 
 * Description: increments the value of the pulse count variable by one, each time this 
 *              function is called by hardware.
 */
void INT0_ISR()
{
  State.pulseCount += 1;  // increment pulseCount variable by one
}

// DANIEL
/* Function Title: INT1_ISR()
 * 
 * Friendly Name:  Real Time Clock OUT Interrupt Service Routine (ISR)
 * 
 * Description: sets the 4-second flag to true each time this function
 *              is called by hardware. The Real Time Clock generates
 *              the signal that calls this ISR once every four seconds.
 */
void INT1_ISR()
{
  State.flag4 = true;     // sets the "four second flag" to true
}
