#include "RTC_PCF8523.h"

/************************************************\
 * Name:    rtcTransfer
 * Purpose: Transfer data over TWI to an RTC
 *          Address is 0x68
 * Inputs:  reg
 *            -- value of the register to access
 *          flag
 *            -- read or write flag
 *          value
 *            -- if writing, the value to write
 * Outputs: returns a byte read from the register
 *          "reg" if doing a read.
 *      
 * Pseudocode:
 *   Power on the TWI interface
 *   Begin a transmission to the RTC
 *   Write the register number
 *   If writing,
 *     write the value to write
 *   If reading,
 *     request one byte from the RTC
 *     assign the byte to value
 *   Power down the TWI interface
 *   Return value
\************************************************/

byte rtcTransfer(byte reg, byte flag, byte value)
{
  twiPowerUp();
  
  Wire.beginTransmission(deviceAddr);
  Wire.write(reg);
  if(flag == WRITE)
    Wire.write(value);
  Wire.endTransmission();
  
  if(flag == READ)
  {
    Wire.requestFrom(deviceAddr, byte(1));
    value = Wire.read();
  }

  twiPowerDown();
  
  return value;
}
