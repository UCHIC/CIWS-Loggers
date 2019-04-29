/*********************************************************************\
 * File:      RTC_PCF8523.cpp
 * Date:      04/17/2019
 * Authors:   Joshua Tracy and Daniel Henshaw
 * Hardware:  Pololu LIS3MDL 3-Axis magnetometer 
 *********************************************************************/
 
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

/***********************************************\
 * Name:    registerDump
 * Purpose: Prints the contents of all RTC 
 *          registers to the serial terminal.
 *          Serial interface must be active
 *          when this function is called.
 * Inputs:  None.
 * Outputs: No function outputs. Just terminal
 *          output.
 *          
 * Pseudocode:
 *   Power on TWI interface
 *   Initiate TWI Read from RTC
 *   Sequentially read 20 bytes from RTC:
 *     Print Register number (decimal)
 *     Print Register contents
 *   Power down TWI interface
\***********************************************/

void registerDump()
{
  twiPowerUp();

  Wire.beginTransmission(deviceAddr);
  Wire.write(reg_Control_1);
  Wire.endTransmission();
  
  Wire.requestFrom(deviceAddr, byte(20));
  byte reg = 0;
  while(Wire.available())
  {
    byte c = Wire.read();
    Serial.print(F("Register "));
    if(reg < 10)
      Serial.print('0');
    Serial.print(reg);
    Serial.print(F(": "));
    Serial.println(c);
    reg += 1;
  }

  twiPowerDown();

  return;
}

/*********************************************\
 * Function Name: loadDateTime
 * Purpose:       Loads date and time info
 *                from RTC into Date_t struct
 * Inputs:        Pointer to Date_t struct
 * Outputs:       None (modifes Date_t struct)
 * Pseudocode:
 *  Read seconds from RTC
 *  Convert from BCD to Binary byte
 *  Store in Date_t struct
 *  
 *  Read minutes from RTC
 *  Convert from BCD to Binary byte
 *  Store in Date_t struct
 *  
 *  Read hours from RTC
 *  Convert from BCD to Binary byte
 *  Store in Date_t struct
 *  
 *  Read days from RTC
 *  Convert from BCD to Binary byte
 *  Store in Date_t struct
 *  
 *  Read months from RTC
 *  Convert from BCD to Binary byte
 *  Store in Date_t struct
 *  
 *  Read years from RTC
 *  Convert from BCD to Binary byte
 *  Store in Date_t struct
 *  
 *  Return
\*********************************************/

void loadDateTime(Date_t* Date)
{
  byte seconds  = rtcTransfer(reg_Seconds, READ, 0);
  byte second10 = (seconds >> 4) & 0x07;
  byte second1  = seconds & 0x0F;
  Date->seconds = ((second10 * 10) + second1);
  
  byte minutes  = rtcTransfer(reg_Minutes, READ, 0);
  byte minute10 = (minutes >> 4) & 0x07;
  byte minute1  = minutes & 0x0F;
  Date->minutes = ((minute10 * 10) + minute1);
  
  byte hours    = rtcTransfer(reg_Hours, READ, 0);
  byte hour10   = (hours >> 4) & 0x03;
  byte hour1    = hours & 0x0F;
  Date->hours   = ((hour10 * 10) + hour1);
  
  byte days     = rtcTransfer(reg_Days, READ, 0);
  byte day10    = (days >> 4) & 0x03;
  byte day1     = days & 0x0F;
  Date->days    = ((day10 * 10) + day1);
  
  byte months   = rtcTransfer(reg_Months, READ, 0);
  byte month10  = (months >> 4) & 0x01;
  byte month1   = months & 0x0F;
  Date->months  = ((month10 * 10) + month1);
  
  byte years    = rtcTransfer(reg_Years, READ, 0);
  byte year10   = years >> 4;
  byte year1    = years & 0x0F;
  Date->years   = ((year10 * 10) + year1);

  return;
}
