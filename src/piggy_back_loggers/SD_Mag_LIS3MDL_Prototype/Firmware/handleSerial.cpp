/*********************************************************************\
 * File:      handleSerial.cpp
 * Date:      04/17/2019
 * Authors:   Joshua Tracy and Daniel Henshaw
 * Hardware:  Pololu LIS3MDL 3-Axis magnetometer 
 *********************************************************************/

#include "handleSerial.h"
#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include "state.h"
#include "powerSleep.h"
#include "RTC_PCF8523.h"
#include "magnetometer.h"

/**************************************************\
 * Function Name: handleSerial
 * Purpose:       Recieve and process user input
 * Inputs:        Pointer to State_t struct
 *                Pointer to Date_t struct
 * Outputs:       None
 * pseudocode:
 *  If serial data is available
 *    Obtain user input
 *  If input is c
 *    Delete files on the SD Card
 *  If input is d
 *    Display the current date and time
 *  If input is e
 *    User exit serial
 *  If input is E
 *    Let system know the SD Card is to be removed
 *  If input is h
 *    Print help list
 *  If input is i
 *    Let system know an SD Card is inserted
 *  If input is R
 *    Diagnose RTC (RTC Doctor)
 *  If input is s
 *    Start logging data from meter
 *  If input is S
 *    Stop logging data from meter
 *  If input is u
 *    Update the date and time
 *  If input is '\n'
 *    Ignore
 *  If the input is erroneous
 *    Prompt user for new input and to print help.
 *  Return
\**************************************************/

void handleSerial(volatile State_t* State, Date_t* Date, volatile SignalState_t* SignalState)
{
  if(Serial.available() > 0)    // Check if serial data is available.
  {
    char input = getInput();    // Obtain user input.
    if(input != '\n')
      Serial.println();

    switch(input)               // Switch statement for all defined inputs
    {
      case 'g':                 // Set Configuration
        setConfiguration(State);
        Serial.print(F("\n>> User:   "));
        break;
        
      case 'c':                 // Delete files on the SD Card. Protected from deleting while in use.
        cleanSD(State);
        Serial.print(F("\n>> User:   "));
        break;

      case 'd':                 // View the current date and time.
        viewDateTime(Date);
        Serial.print(F("\n>> User:   "));
        break;

      case 'e':                 // User exit serial. Allows device to go into low-power mode.
        exitSerial(State, Date);
        break;

      case 'E':                 // Let system know the SD Card is to be removed.
        ejectSD(State);
        Serial.print(F("\n>> User:   "));
        break;

      case 'h':                 // Print help list.
        printHelp();
        Serial.print(F("\n>> User:   "));
        break;

      case 'i':                 // Let system know an SD Card is inserted. Actual card initialization is handled elsewhere when powered on/off.
        initSD(State);
        Serial.print(F("\n>> User:   "));
        break;

      case 'l':
        listFiles();
        Serial.print(F("\n>> User:   "));
        break;

      case 'p':
        printConfig(State);
        Serial.print(F("\n>> User:   "));
        break;

      case 'R':
        Serial.print(F(">> Logger: Calling the RTC Doctor...\n"));
        RTC_Doctor();
        Serial.print(F("\n>> User:   "));
        break;

      case 's':                 // Start logging data from meter.
        startLogging(State, SignalState, Date);
        Serial.print(F("\n>> User:   "));
        break;

      case 'S':                 // Stop logging data from meter.
        stopLogging(State);
        Serial.print(F("\n>> User:   "));
        break;

      case 'u':                 // Update date and time.
        updateDateTime(Date);
        Serial.print(F("\n>> User:   "));
        break;

      case 'w':
        printWater(State);
        Serial.print(F("\n>> User:   "));
        break;

      case '\n':
        break;

      default:                  // If the command is invalid, prompt the user and introduce 'h' option.
        Serial.print(F(">> Logger: Unknown command. Use the command \"h\" for a list of commands.\n>> User:   "));
        break;
    }
  }
  
  return;
}

/*******************************************************************************************************************************\
 * Supporting Functions:
 *   void setConfiguration(State_t* State);
 *   void cleanSD(State_t* State, Date_t* Date);    Complete  Tested
 *   void viewDateTime();                           Complete  Tested
 *   void exitSerial(State_t* State);               Complete  Tested
 *   void ejectSD(State_t* State);                  Complete  Tested
 *   void printHelp();                              Complete  Tested
 *   void initSD(State_t* State);                   Complete  Tested, won't catch error state (when no card present)
 *   void startLogging(State_t* State);             Complete  Tested
 *   void stopLogging(State_t* State);              Complete  Tested
 *   void updateDateTime();                         Complete  Tested
 *   char getInput();                               Complete  Tested
 *   char getNestedInput();                         Complete  Tested
\********************************************************************************************************************************/

/*****************************************************************\
 * Function Name: setConfiguration
 * Purpose:       
 * Inputs:        Pointer to State_t structure
 * Outputs:       None
 * pseudocode:
 *  Begin
 *    
 *  Return
\*****************************************************************/

/*
 * Bug: Writes, but cannot read (saved binary, not ASCII). 
 */

void setConfiguration(volatile State_t* State)
{
  Serial.print(F("\n>> Logger: Configuring\n"));
  
  Serial.print(F(">> Logger: Input Site Number (3 DIGITS). Current Site Number is "));
  Serial.write(readConfiguration(addr_siteNum100));
  Serial.write(readConfiguration(addr_siteNum10));
  Serial.write(readConfiguration(addr_siteNum1));
  Serial.println();
  Serial.print(F(">> User:   "));
  writeConfiguration(addr_siteNum100, getNestedInput());
  writeConfiguration(addr_siteNum10, getNestedInput());
  writeConfiguration(addr_siteNum1, getNestedInput());

  Serial.print(F("\n>> Logger: Input Datalogger ID Number (3 DIGITS). Current ID is "));
  Serial.write(readConfiguration(addr_logID100));
  Serial.write(readConfiguration(addr_logID10));
  Serial.write(readConfiguration(addr_logID1));
  Serial.println();
  Serial.print(F(">> User:   "));
  writeConfiguration(addr_logID100, getNestedInput());
  writeConfiguration(addr_logID10, getNestedInput());
  writeConfiguration(addr_logID1, getNestedInput());
  
  Serial.print(F("\n>> Logger: Select Meter\n"));
  Serial.print(F("    1 -- 1\" Meter\n"));
  Serial.print(F("    5 -- 5/8\" Meter\n"));
  Serial.print(F(">> User:   "));
  char input = getNestedInput();
  switch(input)
  {
    case '1':
      Serial.print(F("\n>> Logger: 1\" Meter\n"));
      State->meterSize = '1';
      break;

    case '5':
      Serial.print(F("\n>> Logger: 5/8\" Meter\n"));
      State->meterSize = '5';
      break;

    default:
      Serial.print(F("\n>> Logger: WARNING: INVALID Meter setting\n"));
      State->meterSize = NULL;
      break;
  }
  writeConfiguration(addr_meterSize, State->meterSize);
  
  Serial.print(F("\n>> Logger: Input Sequential File Number (4 DIGITS). Current Number is "));
  Serial.write(readConfiguration(addr_fileNum1000));
  Serial.write(readConfiguration(addr_fileNum100));
  Serial.write(readConfiguration(addr_fileNum10));
  Serial.write(readConfiguration(addr_fileNum1));
  Serial.println();
  Serial.print(F(">> User:   "));
  writeConfiguration(addr_fileNum1000, getNestedInput());
  writeConfiguration(addr_fileNum100, getNestedInput());
  writeConfiguration(addr_fileNum10, getNestedInput());
  writeConfiguration(addr_fileNum1, getNestedInput());


  writeConfiguration(addr_chs3, 0x95);
  writeConfiguration(addr_chs2, 0x6A);
  writeConfiguration(addr_chs1, 0xBD);
  writeConfiguration(addr_chs0, 0x42);

  Serial.println();

  printConfig(State);
  

  return;
}

/*****************************************************************\
 * Function Name: cleanSD
 * Purpose:       Delete "datalog.csv" from the SD Card (Protected 
 *                from deleting while the device is logging).
 * Inputs:        Pointer to State_t structure
 * Outputs:       None
 * pseudocode:
 *  If the SD is not inserted
 *    Prompt the user
 *    Return
 *  If the device is not logging
 *    Power on the SD Card
 *    Initialize the SD Class
 *    Remove "datalog.csv"
 *    Power down SD Card
 *  Else
 *    Prompt the user
 *  Return
\*****************************************************************/

void cleanSD(volatile State_t* State)
{
  if(!State->SDin)
  {
    Serial.print(F(">> Logger: SD Card not initialized."));
    return;
  }
  
  if(!State->logging)
  {
    char filename[13];
    filename[12] = '\0';

    listFiles();

    Serial.print(F(">> Logger: File to remove: "));

    for(byte i = 0; i < 12; i++)
    {
      filename[i] = getNestedInput();
    }

    Serial.print(F("\n>> Logger: WARNING: This operation will permanently delete "));
    Serial.print(filename);
    Serial.print(F(". Continue?(y/n): "));

    char Continue = getNestedInput();

    switch(Continue)
    {
      case 'y':
      case 'Y':
        SDPowerUp();
        SD.begin();
        SD.remove(filename);
        SDPowerDown();
        break;
      case 'n':
      case 'N':
        break;
      default:
        break;       
    }

    Serial.println();
    listFiles();

  }
  else
  {
    Serial.print(F(">> Logger: Cannot clean SD Card while logging."));
  }

  return;
}

/*****************************************\
 * Function Name: viewDateTime()
 * Purpose:       Display the current date
 *                and time information to 
 *                the user.
 * Inputs:        Pointer to Date_t struct
 * Outputs:       None
 * pseudocode:
 *  Power on TWI interface
 *  Request Time and Date from Date_t struct
 *  Print a Timestamp
 *  Return
\*****************************************/

void viewDateTime(Date_t* Date)
{ 
  byte currMinutes = Date->minutes;
  byte currHours = Date->hours;
  byte currDays = Date->days;
  byte currMonths = Date->months;
  byte currYears = Date->years;

  Serial.print(F(">> Logger: "));
  Serial.print(currMonths);
  Serial.print('/');
  Serial.print(currDays);
  Serial.print('/');
  Serial.print(currYears);
  Serial.print(' ');
  Serial.print(currHours);
  Serial.print(':');
  if(currMinutes < 10)
    Serial.print('0');
  Serial.print(currMinutes);

  return;
}

/******************************************\
 * Function Name: exitSerial
 * Purpose:       Logs the user out of
 *                the serial interface,
 *                which allows the 
 *                processor to enter
 *                sleep modes.
 * Inputs:        Pointer to State_t struct
 * Outputs:       None
 * pseudocode:
 *  Print: Exiting.
 *  Print Current Time and date
 *  Set serialOn flag false
 *  Power down serial interface
 *  Return
\******************************************/

void exitSerial(volatile State_t* State, Date_t* Date)
{
  viewDateTime(Date);
  Serial.print(F("\n>> Logger: Exitting.\n\n"));
  State->serialOn = false;
  _delay_ms(1000);
  serialPowerDown();
  
  return;  
}

/***************************************************\
 * Function Name: ejectSD
 * Purpose:       To let the system know that
 *                the SD Card is to be ejected.
 * Inputs:        Pointer to State_t struct.
 * Outputs:       None (modifies input by ref)
 * Pseudocode:
 *  If the device is logging
 *    Prompt user (do not eject)
 *    Return
 *  If the device is unaware of an inserted SD Card
 *    Prompt user
 *    Return
 *  Set SDin flag false
 *  Prompt user
 *  Return
\***************************************************/

void ejectSD(volatile State_t* State)
{
  if(State->logging)
  {
    Serial.print(F(">> Logger: SD Card busy. Use command 'S' to stop datalogging and try again."));
    return;
  }
  if(!State->SDin)
  {
    Serial.print(F(">> Logger: SD Card already ejected."));
    return;
  }
  State->SDin = false;
  Serial.print(F(">> Logger: SD Card may now be removed."));
  
  return;
}

/*******************************\
 * Function Name: printHelp()
 * Purpose:       Prints a list
 *                of commands
 *                recognized by
 *                the device.
 * Inputs:        None
 * Outputs:       None
 * pseudocode:
 *  Print list of commands
 *  Return
\*******************************/

void printHelp()
{
  Serial.print(F(">> Logger: List of commands:\n"));
  Serial.print(F("           c  -- Clean SD card\n"));
  Serial.print(F("           d  -- View date/time\n"));
  Serial.print(F("           e  -- Exit serial interface\n"));
  Serial.print(F("           E  -- Eject SD card\n"));
  Serial.print(F("           g  -- Set device configuration\n"));
  Serial.print(F("           h  -- Display help\n"));
  Serial.print(F("           i  -- Initialize the SD card\n"));
  Serial.print(F("           l  -- List files on the SD card\n"));
  Serial.print(F("           p  -- Print configuration data\n"));
  Serial.print(F("           R  -- Diagnose the RTC\n"));
  Serial.print(F("           s  -- Start datalogging (will append to any existing datalog.csv)\n"));
  Serial.print(F("           S  -- Stop datalogging\n"));
  Serial.print(F("           u  -- Update date/time\n"));
  Serial.print(F("           w  -- Print water flow data from last complete sample\n"));

  return;
}

/*********************************************\
 * Function Name: initSD
 * Purpose:       Let the system know that
 *                an SD card has been 
 *                inserted. Actual 
 *                initialization happens on
 *                SD Card use.
 * Inputs:        Pointer to State_t struct
 * Outputs:       None (modifies input by ref)
 * Pseudocode:
 *  Power on SD Card
 *  If SD initialization is successful
 *    Set SDin flag true
 *    Prompt user
 *  Else
 *    Set SDin flag false (just in case)
 *    Prompt user: No SD Card detected
 *  Power down SD Card
 *  Return
\*********************************************/

void initSD(volatile State_t* State)
{
  SDPowerUp();
  if(SD.begin())
  {
    State->SDin = true;
    Serial.print(F(">> Logger: SD Ready."));
  }
  else
  {
    State->SDin = false; // Should already be false, but just in case.
    Serial.print(F(">> Logger: Error. Is the SD Card inserted?"));
  }
  SDPowerDown();
  
  return;
}

void listFiles(void)
{
  SDPowerUp();
  float fileSize;
  File root;
  root = SD.open("/");

  root.rewindDirectory();
  Serial.print(F(">> Logger: Files on this SD Card\n\nName    \tSize\n--------\t-----\n"));
    
  while(true)
  {
    File entry;
    entry = root.openNextFile();
    if(!entry)
    {
      Serial.print(F("\n>> Logger: End of files"));
      break;
    }
    Serial.print(entry.name());
    Serial.print("\t");
    fileSize = float(entry.size());
    if((fileSize > 1024) && (fileSize < 1048576))
    {
      fileSize = fileSize / 1024.0;
      Serial.print(fileSize, 1);
      Serial.print(F(" kB\n"));
    }
    else if(fileSize >= 1048576)
    {
      fileSize = fileSize / 1048576.0;
      Serial.print(fileSize, 1);
      Serial.print(F(" MB\n"));
    }
    else
    {
      Serial.print(fileSize, 0);
      Serial.print(F(" Bytes\n"));
    }
    
    entry.close();
  }
  SDPowerDown();
  return;
}

void RTC_Doctor()
{
  // Check/Set Interrupt Enable
  // Check/Clear Interrupt Flag
  // Dump Register Contents
  // Manually alter register contents

  EIMSK &= ~(1 << INT1);         // Disable RTC interrupt.
  Serial.print(F(">> RTC Doctor: Hi! I'm here to help fix your RTC.\n"));
  Serial.print(F(">> RTC Doctor: I'm trained to run a couple procedures on your RTC. "));
  bool finished = false;
  while(!finished)
  {
    Serial.print(F("What will it be?\n"));
    Serial.print(F("    1 -- Re-Enable the RTC Interrupt\n"));
    Serial.print(F("    2 -- Reset Interrupt Timer\n"));
    Serial.print(F("    3 -- Display the RTC Register contents\n"));
    Serial.print(F("    4 -- Alter a register\n"));
    Serial.print(F("    5 -- Finish\n"));
    Serial.print(F(">> User:   "));
    char input = getNestedInput();

    char regInput10;
    char regInput1;
    byte regInput;

    char hex16;
    char hex1;
    byte hexConv16;
    byte hexConv1;
    byte hexInput;
  
    switch(input)
    {
      case '1':
        Serial.print(F("\n>> RTC Doctor: Resetting the RTC's Interrupt...\n"));
        rtcTransfer(reg_Control_2, WRITE, 0x02);
        Serial.print(F(">> RTC Doctor: Interrupt reset!"));
        break;
        
      case '2':
        Serial.print(F("\n>> RTC Doctor: Resetting the RTC's Interrupt Timer...\n"));
        rtcTransfer(reg_Tmr_CLKOUT_ctrl, WRITE, 0x3A);
        rtcTransfer(reg_Tmr_A_freq_ctrl, WRITE, 0x02);
        rtcTransfer(reg_Tmr_A_reg, WRITE, 0x04);
        rtcTransfer(reg_Control_2, WRITE, 0x02);
        Serial.print(F(">> RTC Doctor: Timer reset!"));
        break;
  
      case '3':
        Serial.print(F("\n>> RTC Doctor: Here are the contents of the RTC Registers. You will probably want a copy of the PCF8523 Datasheet to interpret these.\n"));
        registerDump();
        break;
  
      case '4':
        Serial.print(F("\n>> RTC Doctor: I'll write whatever you tell me to, so be sure to double check."));
        Serial.print(F("\n>> RTC Doctor: Which register would you like to alter? (rr): "));
        regInput10 = getNestedInput();
        regInput1  = getNestedInput();
        regInput = (byte(regInput1) - 48) + ((byte(regInput10) - 48) * 10);
        Serial.print(F("\n>> RTC Doctor: What value would you like to write? (specify in hexadecimal): 0x"));
        hex16 = getNestedInput();
        hex1  = getNestedInput();
        switch(hex16)
        {
          case '0':
          case '1':
          case '2':
          case '3':
          case '4':
          case '5':
          case '6':
          case '7':
          case '8':
          case '9':
            hexConv16 = (byte(hex16) - 48) << 4;
            break;

          case 'A':
          case 'a':
          case 'B':
          case 'b':
          case 'C':
          case 'c':
          case 'D':
          case 'd':
          case 'E':
          case 'e':
          case 'F':
          case 'f':
            hexConv16 = (byte(hex16) - 55) << 4;
            break;
            
          default:
            hexConv16 = 0;
            break; 
        }

        switch(hex1)
        {
          case '0':
          case '1':
          case '2':
          case '3':
          case '4':
          case '5':
          case '6':
          case '7':
          case '8':
          case '9':
            hexConv1 = (byte(hex1) - 48);
            break;

          case 'A':
          case 'a':
          case 'B':
          case 'b':
          case 'C':
          case 'c':
          case 'D':
          case 'd':
          case 'E':
          case 'e':
          case 'F':
          case 'f':
            hexConv1 = (byte(hex1) - 55);
            break;
            
          default:
            hexConv1 = 0;
            break; 
        }
        hexInput = hexConv16 + hexConv1;
        rtcTransfer(regInput, WRITE, hexInput);
        Serial.print(F("\n>> RTC Doctor: Finished! Check the registers to make sure they're correct:\n"));
        registerDump();
        break;
        
      case '5':
        finished = true;
        break;

      default:
        Serial.print(F("\n>> RTC Doctor: I don't understand your input. Try something from the list."));
    }
    Serial.print(F("\n>> RTC Doctor: "));
  }
  Serial.print(F("Goodbye!"));
  EIMSK |= (1 << INT1);       
  return;
}

/*********************************************\
 * Function Name: startLogging
 * Purpose:       Enable the datalogging
 *                process. This does so by 
 *                enabling the interrupts 
 *                which govern the process.
 * Inputs:        Pointer to State_t struct
 * Outputs:       None (input modified by ref)
 * Pseudocode:
 *  If SD Card is inserted
 *    Set logging flag true
 *    Enable Magnetometer Sensor interrupt
 *    Enable 4-Second RTC interrupt
 *    Prompt User
 *  Else
 *    Prompt User (no SD Card)
 *  Return
\*********************************************/

void startLogging(volatile State_t* State, volatile SignalState_t* SignalState, volatile Date_t* Date)
{
  if(State->SDin)
  {
    if(State->logging)
    {
      Serial.print(F(">> Logger: Already Logging."));
    }
    else
    {
      nameFile(State, Date);
      SDPowerUp();
      while(SD.exists(State->filename))
      {
        incrementFileNumber();
        nameFile(State, Date);
      }
      Serial.print(F(">> Logger: "));
      for(int i = 0; i < 13; i++)
      {
        Serial.print(State->filename[i]);
      }
      Serial.println();
      State->logging = true;
      State->recordNum = 1;
      State->pulseCount = 0;
      State->lastCount = 0;
      State->totalCount = 0;
      createHeader(State);
      initializeData(SignalState);  // To Do: can this be replaced with mag_transfer() ???
      EIMSK |= (1 << INT0);         // Enable Magnetometer Sensor interrupt.
      Serial.print(F("\n>> Logger: Logging started."));
    }
  }
  else
  {
    Serial.print(F(">> Logger: Cannot log without SD Card."));
  }
  return;
}

/*********************************************\
 * Function Name: stopLogging
 * Purpose:       Stops the datalogging
 *                process. This does so by 
 *                disabling the interrupts 
 *                which govern the process.
 * Inputs:        Pointer to State_t struct
 * Outputs:       None (input modified by ref)
 * Pseudocode:
 *  If device is logging
 *    Set logging flag false
 *    Disable Hall Effect Sensor interrupt
 *    Disable 4-Second RTC interrupt
 *    Prompt User
 *  Else
 *    Prompt User (not loggging)
 *  Return
\*********************************************/

void stopLogging(volatile State_t* State)
{
  if(State->logging)
  {
    State->logging = false;
    EIMSK &= ~(1 << INT0);         // Disable Hall Effect Sensor interrupt
    Serial.print(F(">> Logger: Logging stopped."));
    incrementFileNumber();
  }
  else
  {
    Serial.print(F(">> Logger: Not logging."));
  }
  return;
}

/**********************************************\
 * Function Name: updateDateTime
 * Purpose:       Allows user to update the
 *                date and time on the device.
 * Inputs:        None?
 * Outputs:       None?
 * Pseudocode:
 *  Receive Time and Date from user
 *  Write Time and Date to RTC
 *  Write Time and Date to Date_t struct
 *  Prompt User
 *  Return
\**********************************************/

void updateDateTime(Date_t* Date)
{

  char month1;
  char month10;
  char day1;
  char day10;
  char year1;
  char year10;
  char hour1;
  char hour10;
  char minute1;
  char minute10;

  Serial.print(F(">> Logger: Updating date and time...\n"));
  
  Serial.print(F(">> Logger: Enter new month (mm): "));
  month10 = getNestedInput();
  month1  = getNestedInput();

  Serial.print(F("\n>> Logger: Enter new day (dd): "));
  day10 = getNestedInput();
  day1  = getNestedInput();

  Serial.print(F("\n>> Logger: Enter new year (yy): "));
  year10 = getNestedInput();
  year1  = getNestedInput();

  Serial.print(F("\n>> Logger: Enter new hour (hh, 24-hour clock): "));
  hour10 = getNestedInput();
  hour1  = getNestedInput();

  Serial.print(F("\n>> Logger: Enter new minute (mm): "));
  minute10 = getNestedInput();
  minute1  = getNestedInput();
  Serial.println();

  byte months  = (byte(month1) - 48) + ((byte(month10) - 48) << 4);
  byte days    = (byte(day1) - 48) + ((byte(day10) - 48) << 4);
  byte years   = (byte(year1) - 48) + ((byte(year10) - 48) << 4);
  byte hours   = (byte(hour1) - 48) + ((byte(hour10) - 48) << 4);
  byte minutes = (byte(minute1) - 48) + ((byte(minute10) - 48) << 4);

  rtcTransfer(reg_Months, WRITE, months);
  rtcTransfer(reg_Days, WRITE, days);
  rtcTransfer(reg_Years, WRITE, years);
  rtcTransfer(reg_Hours, WRITE, hours);
  rtcTransfer(reg_Minutes, WRITE, minutes);
  rtcTransfer(reg_Seconds, WRITE, byte(0x00));

  loadDateTime(Date);
  viewDateTime(Date);

  Serial.print(F("\n>> Logger: Date and Time reset."));

  return;
}

/*************************************************************\
 * Function Name: getInput
 * Purpose:       Receive user input to process
 * Inputs:        None
 * Outputs:       character command
 * Pseudocode:
 *  Read Serial input
 *  If input is not a newline character
 *    Print the input (convenient for some terminal emulators)
 *  Return input
\*************************************************************/

char getInput()
{
  char input = Serial.read();
  if (input != '\n')          // Ignore newline characters.
    Serial.print(input);    // For the Arduino IDE Serial Monitor. Other serial monitors may not need this.

  return input;
}

/********************************************\
 * Function Name: getNestedInput
 * Purpose:       Get input from user within
 *                a command invoked by the
 *                user.
 * Inputs:        None
 * Outputs:       char input
 * Pseudocode:
 *  Set finished flag false
 *  While not finished
 *    If serial data is available
 *      input = getInput()
 *      if input isn't a newline
 *        finished is true
 *  Return input
\********************************************/

char getNestedInput()
{
  bool finished = false;
  char input;
  while(!finished)
  {
    if(Serial.available() > 0)
    {
      input = getInput();
      if(input != '\n')
        finished = true;
    }
  }
  
  return input;
}

void printWater(State_t* State)
{
  Serial.println(F(">> Logger: Data from last sample:"));
  byte pulses = State->lastCount;
  unsigned int totalPulses = State->totalCount;
  float avgFlowRate;
  float totFlow;
  float totFlowSinceStart;
  float convFactor;
  switch(State->meterSize)
  {
    case '1':
      convFactor = 0.033;
      break;
    case '5':
      convFactor = 0.0087;
      break;
    default:
      convFactor = NULL;
      break;
  }
  totFlow = (float)pulses * convFactor;
  totFlowSinceStart = (float)totalPulses * convFactor;
  avgFlowRate = totFlow * 60.0 / 4.0;

  Serial.print(F(">> Logger: Total Pulses: "));
  Serial.println(pulses);
  if(convFactor != NULL)
  {
    Serial.print(F(">> Logger: Total Flow:   "));
    Serial.print(totFlow);
    Serial.print(F(" Gal\n"));
  
    Serial.print(F(">> Logger: Average Flow: "));
    Serial.print(avgFlowRate);
    Serial.print(F(" Gal/min\n\n"));

    Serial.print(F(">> Logger: Total Pulses since logging start: "));
    Serial.println(totalPulses);

    Serial.print(F(">> Logger: Total Flow since logging start:   "));
    Serial.print(totFlowSinceStart);
    Serial.print(F(" Gal\n"));
  }
  else
  {
    Serial.println(F(">> Logger: Invalid meter configuration. Cannot print more flow data. Reset configuration with 'g'."));
  }

  return;
}

void printConfig(State_t* State)
{
  Serial.print(F(">> Logger: Site #:            "));
  Serial.write(readConfiguration(addr_siteNum100));
  Serial.write(readConfiguration(addr_siteNum10));
  Serial.write(readConfiguration(addr_siteNum1));
  Serial.println();
  
  Serial.print(F(">> Logger: Datalogger ID #:   "));
  Serial.write(readConfiguration(addr_logID100));
  Serial.write(readConfiguration(addr_logID10));
  Serial.write(readConfiguration(addr_logID1));
  Serial.println();

  Serial.print(F(">> Logger: Sequential File #: "));
  Serial.write(readConfiguration(addr_fileNum1000));
  Serial.write(readConfiguration(addr_fileNum100));
  Serial.write(readConfiguration(addr_fileNum10));
  Serial.write(readConfiguration(addr_fileNum1));
  Serial.println();
  
  Serial.print(F(">> Logger: Meter Size:        "));
  switch(State->meterSize)
  {
    case '1':
      Serial.println(F("1\""));
      break;
    case '5':
      Serial.println(F("5/8\""));
      break;
    default:
      Serial.println(F("INVALID"));
      break;
  }

  return;
}

/****************************************************\
 * Function Name: createHeader
 * Purpose:       Write a header to datalog.csv
 * 
 * Inputs:        Pointer to State_t struct
 * Outputs:       Writes to SD Card (no return)
 * 
 * Pseudocode:
 *  Begin
 *    Power on SD card
 *    Print site number
 *    Print datalogger ID
 *    Print meter size
 *    Print column names
 *  Return
\****************************************************/

void createHeader(State_t* State)
{
  SDPowerUp();
  File dataFile;
  dataFile = SD.open(State->filename, FILE_WRITE);
  dataFile.print(F("Site #: "));
  dataFile.write(readConfiguration(addr_siteNum100));
  dataFile.write(readConfiguration(addr_siteNum10));
  dataFile.write(readConfiguration(addr_siteNum1));
  dataFile.println();
  
  dataFile.print(F("Datalogger ID #: "));
  dataFile.write(readConfiguration(addr_logID100));
  dataFile.write(readConfiguration(addr_logID10));
  dataFile.write(readConfiguration(addr_logID1));
  dataFile.println();
  
  dataFile.print(F("Meter Size: "));
  switch(State->meterSize)
  {
    case '1':
      dataFile.println(F("1\""));
      break;
    case '5':
      dataFile.println(F("5/8\""));
      break;
    default:
      dataFile.println(F("INVALID"));
      break;
  }
  
  dataFile.print(F("Time,Record,Pulses\n"));

  dataFile.close();
  SDPowerDown();

  return;
}

void nameFile(State_t* State, Date_t* Date)
{
  State->filename[0] = readConfiguration(addr_siteNum100);
  State->filename[1] = readConfiguration(addr_siteNum10);
  State->filename[2] = readConfiguration(addr_siteNum1);
  State->filename[3] = '_';

  State->filename[4] = readConfiguration(addr_fileNum1000);
  State->filename[5] = readConfiguration(addr_fileNum100);
  State->filename[6] = readConfiguration(addr_fileNum10);
  State->filename[7] = readConfiguration(addr_fileNum1);

  State->filename[8] = '.';
  State->filename[9] = 'c';
  State->filename[10] = 's';
  State->filename[11] = 'v';
  State->filename[12] = '\0';

  return;
}

void incrementFileNumber(void)
{
  int fileNum1000 = (readConfiguration(addr_fileNum1000) - 48) * 1000;
  int fileNum100  = (readConfiguration(addr_fileNum100) - 48) * 100;
  int fileNum10   = (readConfiguration(addr_fileNum10) - 48) * 10;
  int fileNum1    = (readConfiguration(addr_fileNum1) - 48);

  int fileNum = fileNum1000 + fileNum100 + fileNum10 + fileNum1 + 1;

  writeConfiguration(addr_fileNum1, ((fileNum % 10) + 48));
  fileNum = fileNum / 10;
  writeConfiguration(addr_fileNum10, ((fileNum % 10) + 48));
  fileNum = fileNum / 10;
  writeConfiguration(addr_fileNum100, ((fileNum % 10) + 48));
  fileNum = fileNum / 10;
  writeConfiguration(addr_fileNum1000, ((fileNum % 10) + 48));

  return;
}
