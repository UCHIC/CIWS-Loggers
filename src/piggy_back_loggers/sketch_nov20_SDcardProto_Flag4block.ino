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

// These six macros below are the ten's place mask for the various real time clock registers
#define SECONDS_REG_MASK 0x7                            // keep lower 3 bits
#define MINUTES_REG_MASK 0x7
#define HOURS_REG_MASK   0x3                            // keep lower 2 bits
#define DAYS_REG_MASK    0x3
#define MONTHS_REG_MASK  0x1                            // keep lower 1 bit
#define YEARS_REG_MASK   0xF                            // keep all four bits


void storeNewRecord() 
{                                                       // Begin
    State.flag4 = 0;                                    //     Reset flag4 to zero
    rtcTransfer(reg_Control_2, WRITE, 0x02);            //     Reset real time clock interrupt flag
    byte finalCount;                                    //     Declare variables
    byte temp;
    finalCount = State.pulseCount;                      //     Store pulse count to a variable named final count
    State.pulseCount = 0;                               //     Set pulseCount to zero
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
                                                        //         Write new record to SD card 
        dataFile.print('\"'));                          //           open the date-time string by writing a double quotation mark
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
        if(Date.seconds < 10)                           //             if seconds is less than ten
        {                                               //             then
          dataFile.print('0');                         //               write a leading zero (the seconds value will be appended to it by the next statement)
        }                                               //             endIf
        dataFile.print(':');                            //             write a colon to separate minutes from seconds
        dataFile.print(Date.seconds);                   //             write the seconds
        dataFile.print('\"');                           //           close date-time string by writing a double quotation mark       
        dataFile.print(',');                            //           write a comma to begin a new field (CSV file format)
        dataFile.print(State.recordNum);                //           write the record number
        dataFile.print(',');                            //           write a comma to begin a new field (CSV file format)
        dataFile.println(finalCount);                   //           write the number of pulses counted when function was called (finalCount) and then print a new line
                                                        //         end of writing the record to SD card 
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
























 
