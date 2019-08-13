// Firmware for the CIWS Residential Datalogger
// Arduino IDE ver. 1.8.8
// Utah Water Research Lab
// Updated: 7/30/2019 (Updated comments)
// Daniel Henshaw and Josh Tracy
// Note: F("String") keeps string literals in program memory and out of RAM. Saves RAM space. Very good. Don't remove the F. I know it looks funny. But don't do it. Really. The program might crash if you do. And then you'll have dishonor on yourself, dishonor on your cow, and you'll find out your cricket ain't lucky.
// Note: Be sure to process resulting data file as ASCII characters, not Unicode. 
/*******************************************************************************************\
* Hardware Description
*                     ___________________   TWI                       ___
*                    |                   |<------------------------->[___] RTC  
*                    |                   |                   |        ___
*                    |                   |                    ------>|___| Magnetometer
*  ________          |                   |  SPI                       _______
* |        |         |                   |<------------------------->|       | SD
* |        |<------->|                   |<-----[] Activate Serial   |______/
* |________|         |___________________|  GPIO
*  Serial             Controller
* 
* Serial:          Serial interface for user interaction with the logger
* Controller:      Arduino Pro/Pro Mini (used for SD library and lower power options)
* SD:              External SD Card storage for the logger.
* Activate Serial: Button to wake up the controller and activate the Serial interface
* Magnetometer:    LIS3MDL Magnetometer for reading magnetic signals from water meters.
* RTC:             Real Time Clock to track time and wake up controller every four seconds.
\*******************************************************************************************/

/*****************************************************************\
* Software Description
* Overview: 
*   User inputs:
*     Serial input.
*   Device inputs:
*     Magnetometer Sensor (LIS3MDL)
*     Real Time Clock     (PCF8523)
*   Device outputs:
*     Serial output
*     Datalog file        (SD Card)
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
*          Write data
*        If the Magnetometer interrupted:
*          Read the magnetometer data and check for peaks.
*      Repeat Loop
*      
* Interrupts:
*   1. INT0_ISR()
*      Magnetometer Data Ready signal. When magnetometer data is ready, set State.readMag
*
*   2. INT1_ISR()
*      RTC Four Second signal. When four seconds pass, set State.flag4
\*****************************************************************/

#include <SPI.h>
#include <SD.h>

#include "handleSerial.h"
#include "powerSleep.h"
#include "state.h"
#include "RTC_PCF8523.h"
#include "storeNewRecord.h"
#include "detectPeaks.h"
#include "magnetometer.h"
#include "configuration.h"

// These six macros below are the ten's place mask for the various real time clock registers
#define SECONDS_REG_MASK 0x7                            // keep lower 3 bits
#define MINUTES_REG_MASK 0x7
#define HOURS_REG_MASK   0x3                            // keep lower 2 bits
#define DAYS_REG_MASK    0x3
#define MONTHS_REG_MASK  0x1                            // keep lower 1 bit
#define YEARS_REG_MASK   0xF                            // keep all four bits


/*********************************************************************************\
 * Setup:
 *    Setup the System State structure     
 *    Setup the Digital I/O pins
 *      Pin 2 (INT0):  Magnetometer Interrupt
 *      Pin 3 (INT1):  RTC 4-Second Interrupt
 *      Pin 5 (INPUT): Serial Activate Button
 *    Initialize Magnetometer
 *    Setup RTC Timer
 *    Setup interrupts
 *    Load Date_t with Date/Time info
 *    Check for device configuration data
 *    Disable unneeded peripherals
 *    If Activate Serial Button is pressed
 *      Power on the Serial Interface
\*********************************************************************************/

volatile State_t State;             // System State structure
Date_t Date;                        // System Time and Date structure
File dataFile;                      // File pointer for SD Card operations
volatile SignalState_t SignalState; // Struct containing signal data from magnetometer

void setup() 
{
  resetState(&State);             // Setup the System State structure
  
  pinMode(2, INPUT);              // Setup the Digital I/O Pins       
  pinMode(3, INPUT_PULLUP);
  pinMode(5, INPUT);
  pinMode(4, OUTPUT); // For debugging
  digitalWrite(4, LOW);

  mag_init(); //magnetometerInit(&mag);         // Initialize Magnetometer
  

  rtcTransfer(reg_Tmr_CLKOUT_ctrl, WRITE, 0x3A);                  // Setup RTC Timer
  rtcTransfer(reg_Tmr_A_freq_ctrl, WRITE, 0x02);
  rtcTransfer(reg_Tmr_A_reg, WRITE, 0x04);
  rtcTransfer(reg_Control_2, WRITE, 0x02);
  rtcTransfer(reg_Control_3, WRITE, 0x80);                        

  attachInterrupt(digitalPinToInterrupt(2), INT0_ISR, RISING);   // Setup Interrupts
  attachInterrupt(digitalPinToInterrupt(3), INT1_ISR, FALLING);
  EIMSK &= ~(1 << INT0);        // Disable Sensor interrupt
  EIMSK |= (1 << INT1);         // Enable 4-Second RTC interrupt.
  
  loadDateTime(&Date);            // Load Date_t with Date/Time info

  if(configurationExists())
  {
    State.configured = true;
    State.meterSize = readConfiguration(addr_meterSize);
  }
  else
    State.configured = false;
                                  
  disableUnneededPeripherals();   // Disable unneeded peripherals

  if((digitalRead(5) == 0) && !State.serialOn)  // If Activate Serial Button is pressed
  {
    State.serialOn = true;                        // Power on the Serial Interface
    serialPowerUp();
    if(!State.configured)
      Serial.print(F(">> Logger: Invalid device configuration. Reset configuration with command 'g'\n"));
  }
}

void loop() 
{
  // JOSH
  /*****************************************\
  * ButtonCheck: Is the button pressed?
  * If button is pressed (active-low):
  *   Set serialOn flag.
  *   call serialPowerUp()
  *   If device is not configured:
  *     Prompt user
  \*****************************************/  
  if((digitalRead(5) == 0) && !State.serialOn)
  {
    State.serialOn = true;
    serialPowerUp();
    if(!State.configured)
      Serial.print(F(">> Logger: Invalid device configuration. Reset configuration with command 'g'\n"));
  }
    
  // JOSH
  /*****************************************\
  * Serial: User I/O over serial
  * serialOn flag is set:
  *   call function handleSerial();
  \*****************************************/
  if(State.serialOn)
    handleSerial(&State, &Date, &SignalState);
  
  // DANIEL
  /*****************************************\
  * 4-second update: Update at 4 seconds
  * Clear interrupt flag on RTC
  * Load date and time
  * If 4-second flag is set:
  *   Store a new record
  \*****************************************/
  if(State.flag4)
  {
    State.flag4 = 0;                                    //     Reset flag4 to zero
    rtcTransfer(reg_Control_2, WRITE, 0x02);            //     Reset real time clock interrupt flag
    loadDateTime(&Date);
    if(State.logging)
      storeNewRecord();
  }

  // JOSH
  /*****************************************\
   * Read Magnetometer: 
   * 
   * If readMag is set:
   *   call readData(&mag, &SignalState);
   *   If peakDetected(&SignalState)
   *     State.pulseCount += 1;
  \*****************************************/
  if(State.readMag)
  {
    //if(State.serialOn)
    //  Serial.println("Interrupted");
    State.readMag = false;
    readData(&SignalState);  // changed from calling read_mag(mag_out) back to readData(&SignalState). Now it should work as intented, per J.T.'s instructions to me. 4/23/19 by D.H.

    // bool peak = peakDetected(&SignalState);
    bool peak = peakDetected2(&SignalState);
    
    if(peak){
      State.pulseCount += 1;
      //if(State.serialOn)
        //Serial.println("Peak Detected");
    }
  }
}

// DANIEL
/* Function Title: INT0_ISR()
 * 
 * Friendly Name:  Sensor Interrupt Service Routine (ISR)
 * 
 * Description: sets readMag flag to true every time this function is
 *              called by hardware.
 */
void INT0_ISR()
{
  // For magnetometer version: set a flag which will allow the datalogger to read the magnetometer
  State.readMag = true;
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

/* Function: storeNewRecord
 *  
 * Author: Daniel Henshaw             
 * Date: 11/10/18    
 * 
 * Description: This function gets the current time from the Real Time Clock, and then  
 *              carries out the communication to write a new record to the SD card and 
 *              then turns off the I2C and SD card.
 * 
 * Pseudocode: 
 *  
 * Begin
 *   Reset flag4 to zero
 *   Reset real time clock interrupt flag
 *   Declare variables
 *   Store pulse count to a variable named final count
 *   Set pulseCount to zero
 *   Read current time from the Real Time Clock and update the Date struct with the current time
 *     turn on the I2C interface (function rtcTransfer does this for us)
 *       read the year and store into temp variable       
 *       convert from binary-coded decimal into binary, and store into years field of Date struct
 *       read the month and store into temp variable       
 *       convert from binary-coded decimal into binary, and store into months field of Date struct
 *       read the day and store into temp variable       
 *       convert from binary-coded decimal into binary, and store into days field of Date struct
 *       read the hour and store into temp variable       
 *       convert from binary-coded decimal into binary, and store into hours field of Date struct
 *       read the minutes and store into temp variable       
 *       convert from binary-coded decimal into binary, and store into minutes field of Date struct
 *       read the seconds and store into temp variable       
 *       convert from binary-coded decimal into binary, and store into seconds field of Date struct 
 *     turn off the I2C interface (function rtcTransfer does this for us)
 *   Write the new record to the SD card  
 *     power on SD card
 *       write new record to SD card 
 *         open the date-time string by writing a double quotation mark
 *           write year, month, day, hours, 
 *           if minutes is less than ten
 *           then
 *             write a leading zero (the minutes value will be appended to it by the next statement)
 *           endIf 
 *           write the minutes         
 *           if seconds is less than ten
 *           then
 *             write a leading zero (the seconds value will be appended to it by the next statement)
 *           endIf  
 *           write the seconds
 *         close the date-time string by writing a double quotation mark
 *         write a comma to begin a new field (CSV file format)
 *         write the record number 
 *         write a comma to begin a new field (CSV file format)
 *         write the number of pulses counted when function was called (finalCount)
 *       end of writing the record to SD card  
 *     power down SD card     
 * End of function storeNewRecord() 
 */

void storeNewRecord() 
{                                                       // Begin
    byte finalCount;                                    //     Declare variables
    byte temp;
    finalCount = State.pulseCount;                      //     Store pulse count to a variable named final count
    State.pulseCount = 0;                               //     Set pulseCount to zero
    State.lastCount = finalCount;
    State.totalCount += (unsigned int)finalCount;
                                                        //     Read current time from the Real Time Clock and update the Date struct with the current time                                                      
      temp         = rtcTransfer(reg_Years,  READ, 0);  //         read the Years and store into temp variable
      Date.years   = bcdtobin(temp, YEARS_REG_MASK);    //         convert from binary-coded decimal into binary, and store into years field of Date struct
      temp         = rtcTransfer(reg_Months, READ, 0);  //         read the Months and store into temp variable
      Date.months  = bcdtobin(temp, MONTHS_REG_MASK);   //         convert from binary-coded decimal into binary, and store into months field of Date struct
      temp         = rtcTransfer(reg_Days,   READ, 0);  //         read the Days and store into temp variable
      Date.days    = bcdtobin(temp, DAYS_REG_MASK);     //         convert from binary-coded decimal into binary, and store into days field of Date struct
      temp         = rtcTransfer(reg_Hours,  READ, 0);  //         read the Hours and store into temp variable
      Date.hours   = bcdtobin(temp, HOURS_REG_MASK);    //         convert from binary-coded decimal into binary, and store into hours field of Date struct   
      temp         = rtcTransfer(reg_Minutes,READ, 0);  //         read the Minutes and store into temp variable  
      Date.minutes = bcdtobin(temp, MINUTES_REG_MASK);  //         convert from binary-coded decimal into binary, and store into minutes field of Date struct      
      temp         = rtcTransfer(reg_Seconds,READ, 0);  //         read the Seconds and store into temp variable 
      Date.seconds = bcdtobin(temp, SECONDS_REG_MASK);  //         convert from binary-coded decimal into binary, and store into seconds field of Date struct    
                                                        //     Write the new record to the SD card  
      SDPowerUp();                                      //         power on SD card
      dataFile = SD.open(State.filename, FILE_WRITE);
                                                        //         Write new record to SD card 
        dataFile.print('\"');                          //           open the date-time string by writing a double quotation mark
        dataFile.print(Date.years);                     //             write year, month, day, hours 
        dataFile.print('-');
        dataFile.print(Date.months);
        dataFile.print('-');
        dataFile.print(Date.days);
        dataFile.print(' ');
        dataFile.print(Date.hours);
        dataFile.print(':');
        if(Date.minutes < 10)                           //             if minutes is less than ten
        {                                               //             then
          dataFile.print('0');                         //               write a leading zero (the minutes value will be appended to it by the next statement)     
        }                                               //             endIf
        dataFile.print(Date.minutes);                   //             write the minutes
        dataFile.print(':');                            //             write a colon to separate minutes from seconds
        if(Date.seconds < 10)                           //             if seconds is less than ten
        {                                               //             then
          dataFile.print('0');                         //               write a leading zero (the seconds value will be appended to it by the next statement)
        }                                               //             endIf
        dataFile.print(Date.seconds);                   //             write the seconds
        dataFile.print('\"');                           //           close date-time string by writing a double quotation mark       
        dataFile.print(',');                            //           write a comma to begin a new field (CSV file format)
        dataFile.print(State.recordNum);                //           write the record number
        dataFile.print(',');                            //           write a comma to begin a new field (CSV file format)
        dataFile.println(finalCount);                   //           write the number of pulses counted when function was called (finalCount) and then print a new line
        dataFile.close();
        State.recordNum += 1;
      SDPowerDown();                                    //       power down SD card
}                                                       // End of function storeNewRecord() 






/* Function: bcdtobin
 *  
 * Author: Daniel Henshaw             
 * Date: 11/17/18    
 * 
 * Description: This function converts Binary Coded Decimal (BCD) numbers to 
 *              standard binary format. This is particularly useful when working
 *              with the real time clock, which stores time data in BCD format
 *              in its registers. The function takes in the value to convert, and
 *              the source register the value is coming from. The source register
 *              is required so that the conversion will be compatible with the 
 *              number of bits into which the BCD value is stored into the RTC's
 *              registers.
 *
 * Optional dependencies: set of macro definitions, included below, improve
 *              this function's usability or user friendliness. These make it
 *              straightforward for the user to specify from which register
 *              the data is coming from. These also do double duty as masks for the 
 *              value, as not all 8 bits in the register are for the BCD value; some 
 *              of the upper bits serve other purposes or are "unused" 
 *              (see datasheet for PCF8523 by NXP).
 *              
 *              #define SECONDS_REG_MASK 0x7
 *              #define MINUTES_REG_MASK 0x7
 *              #define HOURS_REG_MASK   0x3
 *              #define DAYS_REG_MASK    0x3
 *              #define MONTHS_REG_MASK  0x1
 *              #define YEARS_REG_MASK   0xF
 *
 * Pseudocode: 
 *  
 * Begin
 *   Declare variable
 *   shift right by four the bcdValue (Puts upper 4 bits at the bottom)
 *   tensPlace is tensPlace ANDed with the provided bit mask a.k.a. conversion constant (hexadecimal value)
 *   Return the binary value result (the unit's place plus tensPlace times ten)
 * End   
 */

byte bcdtobin(byte bcdValue, byte sourceReg)
{
  byte tensPlace;
 
  tensPlace  = bcdValue >> 4;
  tensPlace &= sourceReg;         /* NOTE: sourceReg is actually a bit mask */

  return (bcdValue & 0x0F) + (tensPlace * 10); // return the binary value result: 
                                               //   step 1) First parenthesis: bcdValue ANDed with mask 0xF gives us the one's place, 
                                               //   step 2) Second parenthesis: move the ten's place value over by multiplying by ten
                                               //   step 3) Add the first parenthesis group to the second parenthesis group
}
