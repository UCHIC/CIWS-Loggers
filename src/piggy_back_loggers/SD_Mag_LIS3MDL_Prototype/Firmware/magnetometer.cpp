/*********************************************************************\
 * File:      magnetometer.cpp
 * Date:      04/17/2019
 * Authors:   Joshua Tracy and Daniel Henshaw
 * Hardware:  Pololu LIS3MDL 3-Axis magnetometer 
 *********************************************************************/

#include "magnetometer.h"
#include "powerSleep.h"
#include "state.h"
#include <Arduino.h>
#include <SD.h>


/*********************************************************************\
 * 
 * Library for interfacing with the Pololu LIS3MDL magnetometer.
 * 
 * Functions:       Short description:
 *    mag_init      Initialize magnetomter 
 *    read_mag      Read Data from magnetometer
 *    mag_transfer  Read/write data between magnetometer and ATmega328P
 *    
\*********************************************************************/

//--------------------------------------------------------------------//
// Change Log
// ==========
// 4/23/19 Fixed the function header comment for stopLogging in handleSerial.cpp
//          Also improved the header comment for readData in magnetometer.cpp.
//          Also changed Firmware.ino to call readData() as originally designed
//          for the LSM303C sensor, under the if(State.readMag) block.
// 4/19/19 Patched original readData() and initializeData() functions to 
//          work with our in-house library for the Pololu magnetometer board. 
//          Also moved mag_out[] declaration from Firmware.ino to 
//          magnetometer.h and added extern keyword. Initialized mag_out[] 
//          in magnetometer.cpp. Code now compiles; 2 warnings remain. --D.H.
// 4/17/19 Merged Pololu_Mag_Test code into original Sparkfun SD_Mag Firmware code. D.H.
// 4/11/19 Added function mag_init. D.H.
//--------------------------------------------------------------------//

// Declare global variables
int8_t mag_out[] = {0, 0, 0}; // added this 4/19/19 D.H. per J.T.'s finding

/*******************************************************************\
 * Function Name: mag_init
 * 
 * Purpose:   To initialize our desired configuration of the LIS3MDL    
 *            magnetometer: I2C standard mode, FAST_ODR enabled     
 *            (Fast Output Data Rate, meaning faster than 80 Hz), 
 *            Operating Mode for X and Y = Ultra High Performance,
 *            Operating Mode for Z axis  = Ultra High Performance
 *            (this means 155 Hz Output Data Rate is selected),
 *            
 *            The rest of the settings remain in their default state.    
 *
 * Inputs:    none?    
 *                
 *                
 *                
 * Outputs:   Returns true (one) if the configuration was successful,     
 *            or false (zero) if an error occured.
 *                
 *                
 * Pseudocode:
 *  Begin
 *    Declare and initialize variables
 *    //Read status register
 *    Loop While sensor is not ready
 *      Do nothing for a little time to cause a delay
 *      Continue reading status register      
 *    EndLoop
 *    Loop - one loop for each control register
 *      Write to control register N (N is from 1 to 5)
 *      Read back control register N to verify configuration successful
 *    Read 
 *    If successful
 *    Then
 *      Return 1
 *    Else
 *      Return 0
 *    EndIf
 *  End
 */
bool mag_init()
{
  twiPowerUp();
  Wire.begin();
  // Declare and initialize variables
  int8_t receivedBytes[19];
  int8_t myBuffer;
  //byte const initializerForCTRL_REG1
  
  // mag_transfer(&receivedBytes, STATUS_REG, 1 /* byte */, MAG_READ);
  
  // Loop While sensor is not ready
  //while (/* code to read the status register goes here */ mag_transfer) 
  //{
  //  delay(5);  // Do nothing for a little time to cause a delay (5 ms is just a guess)
  //    // Continue reading status register      
  //} // EndLoop

  // Store the configuration value for Control register 1 into buffer
  //myBuffer = 0x72;              // 0b0 11 100 1 0 == 0x72
  myBuffer = 0x32;                // 0b0 00 100 1 0 == 0x12
  // Write to control register N (N is from 1 to 5)
  // Read back control register N to verify configuration successful
  mag_transfer(&myBuffer, CTRL_REG1, 1 /* byte */, MAG_WRITE);
  mag_transfer(&myBuffer, CTRL_REG1, 1 /* byte */, MAG_READ);
  
  //if(myBuffer != 0x72)  // check that the data read back matches what we wanted to write
  if(myBuffer != 0x32)  // check that the data read back matches what we wanted to write
  {
    return 0; // zero means an error occurred
  }

/* NOTE: SKIPPING CTRL_REG2 FOR NOW, BECAUSE DEFAULT RANGE OF +/- 4 GAUSS IS FINE */

  // Store the configuration value for Control register 3 into buffer
  myBuffer = 0x00; // 0b0000 0000 == 0x00
  
  // Write to control register 3
  // Read back control register 3 to verify configuration successful
  mag_transfer(&myBuffer, CTRL_REG3, 1 /* byte */, MAG_WRITE);
  mag_transfer(&myBuffer, CTRL_REG3, 1 /* byte */, MAG_READ);

  if(myBuffer != 0x00)  // check that the data read back matches what we wanted to write
  {
    return 0; // zero means an error occurred
  }

  // Store the configuration value for Control register 4 into buffer
  //myBuffer = 0x0C; // 0b0000 11 00 == 0x0C
  myBuffer = 0x04; // 0b0000 01 00 == 0x04
  // Write to control register 4
  // Read back control register 4 to verify configuration successful
  mag_transfer(&myBuffer, CTRL_REG4, 1 /* byte */, MAG_WRITE);
  mag_transfer(&myBuffer, CTRL_REG4, 1 /* byte */, MAG_READ);

  //if(myBuffer != 0x0C)  // check that the data read back matches what we wanted to write
  if(myBuffer != 0x04)  // check that the data read back matches what we wanted to write
  {
    return 0; // zero means an error occurred
  }

  // Store the configuration value for Control register 5 into buffer
  myBuffer = 0x80; // 0b1000 0000 == 0x80

  // Write to control register 5
  // Read back control register 5 to verify configuration successful
  mag_transfer(&myBuffer, CTRL_REG5, 1 /* byte */, MAG_WRITE);
  mag_transfer(&myBuffer, CTRL_REG5, 1 /* byte */, MAG_READ);

  if(myBuffer != 0x80)  // check that the data read back matches what we wanted to write
  {
    return 0; // zero means an error occurred
  }

/* NOTE: SKIPPING INT_CFG FOR NOW, BECAUSE DEFAULT OF ALL INTERRUPTS DISABLED IS FINE */

/* NOTE: SKIPPING INT_THS FOR NOW, BECAUSE NO THRESHOLDS TO PROGRAM YET */

  twiPowerDown();

  return 1; // one means magnetometer initialization was successful
  
}   // End of function mag_init()



  
/*******************************************************************\
 * Function Name: read_mag
 * 
 * Purpose:       Reads magnetometer output registers (high bytes)
 *
 * Inputs:        Pointer to data buffer to store data
 *                
 * Outputs:       Modifies data stored at address "data"
 *                No return value
 *                
 * Pseudocode:
 *  Begin:
 *    Call mag_transfer to read three bytes starting at OUT_X_H
 *      (Note: Magnetometer must be set in Fast update mode,
 *       which skips LSB registers.)
 *  End
\*******************************************************************/
void read_mag(int8_t* data)
{
  mag_transfer(data, OUT_X_H, 3, MAG_READ);

  return;
}

/*******************************************************************\
 * Function Name: mag_transfer
 * 
 * Purpose:       Handles data transfer between the microcontroller
 *                and the Pololu LIS3MDL Magnetometer. Utilizes
 *                Arduino TWI Library.
 *
 * Inputs:        Pointer to data buffer to be written (or read to)
 *                Number of bytes to write or read
 *                RW Flag (Read or Write Mode)
 *                
 * Outputs:       Modifies data stored at address "data"
 *                No return value
 *                
 * Pseudocode:
 *  Begin:
 *    If RW == READ
 *      Initiate TWI transmission
 *      Write first register to read
 *      Write Stop condition
 *      Request numBytes
 *      while i is less than numBytes
 *        data[i] = byte from TWI
 *        Increment i
 *      end while
 *    Else
 *      Initiate TWI transmission
 *      Write first register to write
 *      while i is less than numBytes
 *        write data[i] to TWI
 *        Increment i
 *      end while  
 *      Write Stop condition
 *  End
\*******************************************************************/
void mag_transfer(int8_t* data, uint8_t reg, uint8_t numBytes, uint8_t RW)
{
  switch(RW)
  {
    case MAG_READ:
      Wire.beginTransmission(DEV_ADDR);
      Wire.write(reg);
      Wire.endTransmission();
      Wire.requestFrom(DEV_ADDR, numBytes);
      for (uint8_t i = 0; i < numBytes; i++)
      {
        data[i] = Wire.read();
      }
      break;

    case MAG_WRITE:
      Wire.beginTransmission(DEV_ADDR);
      Wire.write(reg);
      for (uint8_t i = 0; i < numBytes; i++)
      {
        Wire.write(data[i]);
      }
      Wire.endTransmission();
      break;
  }

  return;
}

/*******************************************************************\
 * Function Name: readData
 * 
 * Purpose:       Gets new data from the magnetometer and places it
 *                in the x member of the SignalState structure.
 *                Utilizes the Arduino TWI Library.
 *
 * Inputs:        
 *                
 *                
 *                
 * Outputs:       
 *                No return value
 *                
 * Pseudocode:
 *  Begin
 *    Power on the Two Wire Interface (TWI)
 *    Read the new data from the X-axis
 *    Store this data into signalState variable
 *    Power down the Two Wire Interface (TWI)
 *  End
\*******************************************************************/


void readData(volatile SignalState_t* SignalState)
{
  twiPowerUp();                               // Power on the Two Wire Interface (TWI)
  
 // SignalState->x = mag->readMagX();
  read_mag(mag_out);                          // Read the new data 
  SignalState->x[1] = (float) mag_out[MAG_X];    // Store the X-axis data (after a type cast from int_8 to float) into x variable of signalState structure
    
  twiPowerDown();                             // Power down the Two Wire Interface (TWI)
    
  return;                                     // End
}


/*******************************************************************\
 * Function Name: initializeData
 * 
 * Purpose:       (Does the same things as readData function.)
 *                Utilizes the Arduino TWI Library.
 *
 * Inputs:        
 *                
 *                
 *                
 * Outputs:       
 *                No return value
 *                
 * Pseudocode:
 *  Begin
 *    Power on the Two Wire Interface (TWI)
 *    Read the new data from the X-axis
 *    Store this data into signalState variable
 *    Power down the Two Wire Interface (TWI)
 *  End  
\*******************************************************************/
  
void initializeData(volatile SignalState_t* SignalState)
{
  twiPowerUp();                               // Power on the Two Wire Interface (TWI)
  
  //SignalState->x = mag->readMagX();
  read_mag(mag_out);                          // Read the new data 
  SignalState->x[1] = (float) mag_out[MAG_X];    // Store the X-axis data (after a type cast from int_8 to float) into x variable of signalState structure
  
  twiPowerDown();                             // Power down the Two Wire Interface (TWI)
  
  return;                                     // End
}




/* THE OLD STUFF from sparkfun version of the code
 *  
void magnetometerInit(LSM303C *mag)
{
  twiPowerUp();
  
  if (mag->begin(
    MODE_I2C,
    MAG_DO_80_Hz,
    MAG_FS_8_Ga,
    MAG_BDU_DISABLE,
    MAG_OMXY_LOW_POWER,
    MAG_OMZ_MEDIUM_PERFORMANCE,
    MAG_MD_CONTINUOUS,
    ACC_FS_2g,
    ACC_BDU_DISABLE,
    ACC_DISABLE_ALL,
    ACC_ODR_POWER_DOWN
    ) != IMU_SUCCESS)
    {
      Serial.println("Magnetometer setup failed. Check connection and reset.");
      while(1);
    }
    twiPowerDown();

    return;
}

void readData(LSM303C* mag, volatile SignalState_t* SignalState)
{
  twiPowerUp();
  SignalState->x = mag->readMagX();
  twiPowerDown();
  
  return;
}


void initializeData(LSM303C* mag, volatile SignalState_t* SignalState)
{
  twiPowerUp();
  SignalState->x = mag->readMagX();
  twiPowerDown();
  
  return;
}
*/
