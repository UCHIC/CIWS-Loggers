void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

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
 * If four-second flag is set 
 * Then
 *   Read current time from the Real Time Clock and update the Date struct with the current time
 *     turn on the I2C interface
 *       read the year and store into Years field of Date struct
 *       read the month and store into Months field of Date struct
 *       read the day and store into Days field of Date struct
 *       read the hour and store into Hours field of Date struct
 *       read the minutes and store into Minutes field of Date struct
 *       read the seconds and store into Seconds field of Date struct
 *     turn off the I2C interface
 *   Write the new record to the SD card  
 *     power on SD card
 *       write new record to SD card 
 *         write year, month, day, hours, minutes, seconds, 
 *     power down SD card
 * EndIf    
 * 
 */


 
void storeNewRecord(File *FilePntr) // TODO: see if "File FilePntr;" is correct, versus "File *FilePntr;" in Arduino Language...
{                                                       // Begin
  File *recordFile = FilePntr;
  
  if(State.flag4) /* TO DO: decide whether this if statement belongs inside of this function or goes just outside of this function in main file.*/                                       // If four-second flag is set 
  {                                                     // Then
                                                        // *   Read current time from the Real Time Clock and update the Date struct with the current time
    twiPowerUp();                                       // *     turn on the I2C interface
      Date.years   = rtcTransfer(reg_Years,  READ, 0);  // *       read the Years and store into years field of Date struct
      Date.months  = rtcTransfer(reg_Months, READ, 0);  // *       read the Months and store into months field of Date struct
      Date.days    = rtcTransfer(reg_Days,   READ, 0);  // *       read the Days and store into days field of Date struct
      Date.hours   = rtcTransfer(reg_Hours,  READ, 0);  // *       read the Hours and store into hours field of Date struct
      Date.minutes = rtcTransfer(reg_Minutes,READ, 0);  // *       read the Minutes and store into minutes field of Date struct
      Date.seconds = rtcTransfer(reg_Seconds,READ, 0);  // *       read the Seconds and store into seconds field of Date struct
    twiPowerDown();                                     // *     turn off the I2C interface
                                                        // *   Write the new record to the SD card  
    SDPowerUp();                                        // *     power on SD card
                                                        // *       write new record to SD card 
    recordFile.print( );              /* To Do: find out the correct format for our record fields. */
    
    SDPowerDown();                                      // *     power down SD card
  }                                                     // EndIf  
  
}                                                       // End of function
