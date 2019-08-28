// Specialized Frimware for the CIWS Residential Datalogger
// Collects raw magnetometer data and stores to the datalogger's SD card.
// For use characterizing the magnetic signal read from the user's particular water meter/register.
// Arduino IDE ver. 1.8.9
// Updated: 8/27/2019
// Josh Tracy

/*******************************************************************************************\
* Hardware Description
*                     ___________________   TWI                       ___
*                    |                   |<------------------------->[___] Magnetometer 
*                    |                   |
*                    |                   |
*                    |                   |  SPI                       _______
*                    |                   |<------------------------->|       | SD
*                    |                   |<-----[] Start/Stop        |______/
*                    |___________________|  GPIO
*                      Controller
* 
* Controller:      Arduino Pro/Pro Mini (used for SD library and lower power options)
* SD:              External SD Card storage for the logger.
* Activate Serial: Button to start/stop logging
* Magnetometer:    LIS3MDL Magnetometer for reading magnetic signals from water meters.
* NOTE: This is implementable on the same datalogger hardware as Firmware.ino.
\*******************************************************************************************/

/*******************************************************************************************\
* Software Description
* Overview:
*   User Interface:
*     Start/Stop Button
*   Device Inputs:
*     Start/Stop Button
*     Magnetometer Sensor (LIS3MDL)
*   Device Outputs:
*     Data File
*
* Structure:
*   1. Setup
*   2. Loop
*         If button pressed and not logging
*           Wait while button pressed
*           Start logging
*         If button pressed and logging
*           Wait while button pressed
*           Stop logging
*         If logging and Data Ready
*           Read Magnetometer
*           Store value on SD Card
\*******************************************************************************************/

#include "magnetometer.h"
#include "Storage.h"

volatile bool dataReady;
bool logging;
int8_t magData[3];
File dataFile;

void setup()
{
  pinMode(2, INPUT);                                            // Set digital pins 2 and 5 as inputs. Pin 2: Data ready interrupt. Pin 5: Start/Stop Button
  pinMode(5, INPUT);

  dataReady = false;                                            // Initially, we won't worry about reading the magnetometer.
  logging = false;                                              // Start the device not logging

  mag_init();                                                   // Initialize the magnetometer

  SD.begin();                                                   // Initialize the SD Card

  dataFile = SD.open("Raw_Data.csv", FILE_WRITE);               // Write file header
  dataFile.print("X_Axis,Y_Axis,Z_Axis\n");
  dataFile.close();

  attachInterrupt(digitalPinToInterrupt(2), INT0_ISR, RISING);  // Set up the magnetometer data ready interrupt.
  EIMSK &= ~(1 << INT0);                                        // Start with the interrupt disabled. Will be enabled when logging starts

  sei();                                                        // Globally enable interrupts
}

void loop()
{
  if(((PIND & 0x20) == 0) && !logging)                          // If a button press is detected and the device is not logging,
  {
    while((PIND & 0x20) == 0);                                    // Wait until the button is no longer pressed
    EIMSK |= (1 << INT0);                                         // Enable the magnetometer data ready interrupt
    if((PIND & 0x4) == 0x4)                                       // If the magnetometer already has data ready
    {
      dataReady = true;                                           // Set data ready true
    }
    logging = true;                                               // set logging true
    dataFile = SD.open("Raw_Data.csv", FILE_WRITE);               // Open a file on the SD card.
  }

  if(((PIND & 0x20) == 0) && logging)                           // If a button press is detected and the device is logging
  {
    while((PIND & 0x20) == 0);                                    // Wait until the button is no longer pressed
    EIMSK &= ~(1 << INT0);                                        // Disable the magnetometer data ready interrupt
    dataReady = false;                                            // set data ready false
    logging = false;                                              // set logging false
    dataFile.close();                                             // Close the data file on the SD Card
  }

  if(logging && dataReady)                                      // If the device is logging and the magnetometer has data ready
  {
    dataReady = false;                                            // Reset data ready flag.
    read_mag(magData);                                            // Read data from the magnetometer and store it in the array magData[].
    dataFile.print(magData[0]);                                   // Store the data in magData[] to the SD card
    dataFile.print(',');
    dataFile.print(magData[1]);
    dataFile.print(',');
    dataFile.println(magData[2]);
  }
}

void INT0_ISR()                                                 // When the magnetometer signals that data is ready:
{
  dataReady = true;                                               // Set data ready flag true.
}
