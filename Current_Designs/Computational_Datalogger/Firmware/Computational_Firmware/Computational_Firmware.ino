// Firmware for the CIWS Residential Datalogger
// Arduino IDE ver. 1.8.9
// Utah Water Research Lab
// Updated: 9/12/2019
// Daniel Henshaw and Josh Tracy

/*****************************************************************************************************************\
* Hardware Description
*  ___________________   GPIO     Power Enable                                  |__Power
* |                   |------------------------------------------------------->|___|---
* |                   |                                                                |
* |                   |  TWI          ___                                              |_______________________
* |                   |<------------>[___] RTC                                         |                       |
* |                   |      |        ___                                              |                       |
* |                   |       ------>|___| Sensor                                      |                       |
* |                   |  GPIO                                   Enable                 |                       |
* |                   |------------------------------------------------->|         SPI |                       |
* |                   |                                 <---------------|><|---------->|          Host         |
* |                   |  SPI                           |   ___        Tri-State Buff   |        Computer       |
* |                   |<--------------------------------->|___| EEPROM                 |                       |
* |                   |<-----[] Activate Button                                        |                       |
* |___________________|  GPIO                                                          |                       |
*  Controller    | Serial                                                       Serial |                       |
*                 <------------------------------------------------------------------->|                       |
*                                                                                      |                       |
*                                                                                      |                       |
*                                                                                      |_______________________|
* 
* Serial:          Serial interface for data swapping between Controller and Host Computer
* Controller:      Arduino Pro/Pro Mini (Note: Few if any Arduino Libraries used, in order to boost performance)
* Host Computer:   Raspberry Pi. Runs data processing algorithms on data collected from Controller.
* EEPROM:          Electrically Erasable Programmable Read-Only Memory -- Stores data logged by Controller
* Activate Button: Button to power up the Host Computer manually.
* Sensor:          LIS3MDL Magnetometer to detect magnetic signal from water register
* RTC:             Real Time Clock to track time and initialize new data sample every four seconds.
* Power Enable:    Enable signal controls Host Computer's connection to power (NOTE: does not power Host Computer)
\*****************************************************************************************************************/

/*****************************************************************\
* Software Description
* Overview: 
*   User inputs:
*     Activation Button.
*   Device inputs:
*     Magnetometer Sensor
*     Real Time Clock
*     Serial Reports
*   Device outputs:
*     Data Records
*     Serial Reports
*     Power Control for host computer
*     
* Structure:
*   1. Setup
*   2. Handle Raspberry Pi Power-up (for start-up configuration)
*   3. Loop
*        If Raspberry Pi is on
*          Handle Raspberry Pi
*        If Four Seconds has passed
*          Load date/time
*          If logging
*            If EEPROM Free
*              Store new record on EEPROM
*            Else
*              Store new record in RAM Buffer
*        If Magnetometer data ready
*          Read the magnetometer data
*          If peak detected
*            pulseCount += 1
*      Repeat Loop
*      
* Interrupts:
*   1. INT0_ISR()
*      
*   2. INT1_ISR()
*      
\*****************************************************************/

#include "powerSleep.h"
#include "state.h"
#include "RTC_PCF8523.h"
#include "storeNewRecord.h"
#include "detectPeaks.h"
#include "magnetometer.h"
#include "storeNewRecord.h"
#include "comm.h"

#define BUFFER_MAX 21600

volatile State_t State;             // System State structure
volatile Date_t Date;               // System Time and Date structure
volatile SignalState_t SignalState; // Struct containing signal data from magnetometer
volatile char report[9];            // Array containing system information. The report is passed between the microcontroller and the host computer (designed for a Raspberry Pi)
volatile char reportIndex = 0;      // current index of the above report
bool countDown = false;             // Tells program to count every four seconds until it's time to power off the host computer.
char powerOff_Count = 0;            // Stores the count as described above.

void setup()
{
  DDRB |= 0x01;                                   // Pin B0 (Digital pin 8) set as output (the rest of GPIO B is taken care of by SPI initialization)
  PORTB |= 0x01;                                  // Set enable pin to host computer bus buffer high (active low, keeps host computer disconnected until powered on).

  DDRD = 0xD3;                                    // Input Pins: 0, 1, 2, 3, 5;     Output Pins: 4, 6, 7;
  PORTD |= 0x88;                                  // Pullup on Pin 3, Set Pin 7 HIGH;

  DDRC = 0x04;                                    // Input Pins: A0, A1;            Output Pins: A2;
  PORTC = 0x00;                                   // A0 and A1 have no pullups, A2 starts low (keeps host computer off)

  resetState(&State);                             // Set up the System State structure

  mag_init();                                     // Initialize Magnetometer
  spiInit();                                      // Initialize SPI Module

  rtcTransfer(reg_Tmr_CLKOUT_ctrl, WRITE, 0x3A);  // Setup RTC Timer
  rtcTransfer(reg_Tmr_A_freq_ctrl, WRITE, 0x02);
  rtcTransfer(reg_Tmr_A_reg, WRITE, 0x04);
  rtcTransfer(reg_Control_2, WRITE, 0x02);
  rtcTransfer(reg_Control_3, WRITE, 0x80);

  attachInterrupt(digitalPinToInterrupt(2), INT0_ISR, RISING);   // Setup Interrupts
  attachInterrupt(digitalPinToInterrupt(3), INT1_ISR, FALLING);
  EIMSK &= ~(1 << INT0);                                        // Disable Sensor interrupt
  EIMSK |= (1 << INT1);                                         // Enable 4-Second RTC interrupt.

  loadDateTime(&Date);            // Disable Unneeded Peripherals

  report[0] = Date.years;         // Initialize report data
  report[1] = Date.months;
  report[2] = Date.days;
  report[3] = Date.hours;
  report[4] = Date.minutes;
  report[5] = Date.seconds;
  report[6] = 0;
  report[7] = 0;
  report[8] = 0;

  sei();                          // Enable interrupts
}

void loop()
{
  if((((PIND & 0x20) == 0x00) && !State.RPiON) || (State.romAddr > (BUFFER_MAX + HEADER_SIZE))) // If (button is pressed and the host computer is off) OR (The ROM address is greater than the data length)
  {
    State.RPiON = true;                                                               // Let the rest of the program know that the host computer is on
    State.romAddr = HEADER_SIZE;                                                      // Reset the ROM address
    State.romFree = false;                                                            // Let the rest of the program know that the EEPROM chip is not accessible
    writeDataSize(&State);                                                            // Store the number of data bytes in the first three bytes of the EEPROM chip for the host computer
    powerRPiON();                                                                     // Power on the host computer
  }
  
  if(State.RPiON)                     /** Execute the following code IF the host computer is on **/
  {
    if(State.newReport)                   // If a new report has been exchanged between the microcontroller and the host computer                          
    {
      State.newReport = false;              // Let the rest of the program know that the new report has been serviced
      updateReport(report, &Date, &State);  // Service the new report by updating state, date, and time information.
    }
  
    if(usartReceiveComplete())            // If the microcontroller has received a byte from the host computer
    {
      unsigned char temp = UART_Receive();  // Store new byte
      UART_Transmit(report[reportIndex]);   // Transmit next byte
      if(temp != 0xFF)                      // If the received byte is not 0xFF (0xFF means to not overwrite the byte in the report)
        report[reportIndex] = temp;           // Update the current report byte with the newly received byte
      
      if((++reportIndex) > 8)               // If the report has been filled up
      {
        reportIndex = 0;                      // Reset the report index
        State.newReport = true;               // Let the rest of the program know that a new report is ready to be serviced
      }
      
    }
/* 
    if(((PINC & 0x01) == 0x01) && State.romFree)
      State.romFree = false;
*/  
    if(((PINC & 0x01) == 0x00) && !State.romFree && State.powerGood)  // If the EEPROM Busy signal from the host computer goes low when the host computer has been detected as ON and reading the EEPROM
    {
      State.romFree = true;                                             // Let the rest of the program know that the EEPROM is accessible
      PORTB |= 0x01;                                                    // Set the host computer's SPI bus enable line high (active low, so the host computer is disconnected).
      spiInit();                                                        // Set the microcontroller's SPI pins appropriately again.
    }
  
    if(((PINC & 0x02) == 0x02) && !State.powerGood)   // If the host computer's power good line transitions from low to high
      State.powerGood = true;                           // Let the rest of the program know that the host computer is successfully powered on.
  
    if(((PINC & 0x02) != 0x02) && State.powerGood)    // If the host computer's power good line transitions from high to low
    {
      State.powerGood = false;                          // Let the rest of the program know that the host computer is shutting down
      countDown = true;                                 // Tell the rest of the program to count every four seconds to know when to disconnect power to the host computer
      powerOff_Count = 0;                               // Initialize the counter.
    }
  
    if(powerOff_Count > 3)                            // If the count exceeds three
    {
      powerOff_Count = 0;                               // Reset the counter
      countDown = false;                                // Let the rest of the program know that it is know longer counting
      powerRPiOFF();                                    // Disconnect power to the host computer
      State.RPiON = false;                              // Let the rest of the program know that the host computer has been powered off
    }

    if(State.RPiFalseON)                              // If a certain amount of time passes and the host computer is not freeing up the EEPROM
    {
      State.romFree = true;                             // Let the rest of the program know that the EEPROM chip is now accessible
      State.RPiFalseON = false;
      PORTB |= 0x01;                                    // Disable the host computer's SPI bus
      spiInit();                                        // Set the microcontroller's SPI pins appropriately again.
      powerRPiOFF();                                    // Disconnect power to the host computer
      State.RPiON = false;                              // Let the rest of the program know that the host computer is no longer on
      State.powerGood = false;                          // Let the rest of the program know that the host computer is no longer on
    }
  }
  
  // DANIEL
  /*****************************************\
  * 4-second update: Update at 4 seconds
  * If 4-second flag is set:
  *   Store a new record
  \*****************************************/
  if(State.flag4)                                     // If four seconds have passed
  {
    State.flag4 = 0;                                    // Reset flag4 to zero
    rtcTransfer(reg_Control_2, WRITE, 0x02);            // Reset real time clock interrupt flag
    loadDateTime(&Date);                                // Update the date and time from the real time clock
    report[0] = Date.years;                             // Sync report date/time with RTC date/time
    report[1] = Date.months;
    report[2] = Date.days;
    report[3] = Date.hours;
    report[4] = Date.minutes;
    report[5] = Date.seconds;
    if(State.logging)                                   // If the microcontroller is logging data
      storeNewRecord(&State);                             // Store a new record
    if(countDown)                                       // If it's time to count to the host computer's shutdown time
      powerOff_Count += 1;                                // Increment the counter
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
  if(State.readMag)                                   // If new data is ready from the magnetometer
  {
    State.readMag = false;                              // Let the rest of the program know that the new data is being read
    readData(&SignalState);                             // Read the data from the magnetometer and store it in the SignalState struct

    bool peak = peakDetected(&SignalState);             // Process the data in order to detect peaks
    
    if(peak)                                            // If a peak was detected
    {
      State.pulseCount += 1;                              // Increment the current pulse count
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
