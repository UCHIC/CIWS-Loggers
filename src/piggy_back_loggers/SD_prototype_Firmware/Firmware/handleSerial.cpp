#include "handleSerial.h"

#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include "state.h"
#include "powerSleep.h"
#include "RTC_PCF8523.h"

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

void handleSerial(volatile State_t* State, Date_t* Date)
{ 
  if(Serial.available() > 0)    // Check if serial data is available.
  {
    char input = getInput();    // Obtain user input.

    switch(input)               // Switch statement for all defined inputs
    {
      case 'c':                 // Delete files on the SD Card. Protected from deleting while in use.
        cleanSD(State);
        break;

      case 'd':                 // View the current date and time.
        viewDateTime(Date);
        break;

      case 'e':                 // User exit serial. Allows device to go into low-power mode.
        exitSerial(State);
        break;

      case 'E':                 // Let system know the SD Card is to be removed.
        ejectSD(State);
        break;

      case 'h':                 // Print help list.
        printHelp();
        break;

      case 'i':                 // Let system know an SD Card is inserted. Actual card initialization is handled elsewhere when powered on/off.
        initSD(State);
        break;

      case 's':                 // Start logging data from meter.
        startLogging(State);
        break;

      case 'S':                 // Stop logging data from meter.
        stopLogging(State);
        break;

      case 'u':                 // Update date and time.
        updateDateTime(Date);
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
 *   void cleanSD(State_t* State, Date_t* Date);    Complete
 *   void viewDateTime();                           Complete
 *   void exitSerial(State_t* State);               Complete  Tested
 *   void ejectSD(State_t* State);                  Complete  
 *   void printHelp();                              Complete  Tested
 *   void initSD(State_t* State);                   Complete  
 *   void startLogging(State_t* State);             Complete  Tested
 *   void stopLogging(State_t* State);              Complete  Tested
 *   void updateDateTime();                         Complete
 *   char getInput();                               Complete  Tested
 *   char getNestedInput();                         Complete
\********************************************************************************************************************************/

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
    Serial.print(F(">> Logger: SD Card not initialized.\n>> User:   "));
    return;
  }
  
  if(!State->logging)
  {
    SDPowerUp();
    SD.begin();
    Serial.print(F(">> Logger: Removing \"datalog.csv\"... "));
    SD.remove(F("datalog.csv"));
    Serial.print(F(">> Logger: File removed.\n>> User:   "));
    SDPowerDown();
  }
  else
  {
    Serial.print(F(">> Logger: Cannot clean SD Card while logging.\n>> User:   "));
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
  twiPowerUp();
  
  byte currMinutes = Date->minutes;
  byte currHours = Date->hours;
  byte currDays = Date->days;
  byte currMonths = Date->months;
  byte currYears = Date->years;

  Serial.print(F(">> Logger: "));
  Serial.print(currMonths);
  Serial.print(F("/"));
  Serial.print(currDays);
  Serial.print(F("/"));
  Serial.print(currYears);
  Serial.print(F(" "));
  Serial.print(currHours);
  Serial.print(F(":"));
  if(currMinutes < 10);
    Serial.print(F("0"));
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
 *  Print: Exiting...
 *  Set serialOn flag false
 *  Print: Finished.
 *  Power down serial interface
 *  Return
\******************************************/

void exitSerial(volatile State_t* State)
{
  Serial.print(F(">> Logger: Exitting... \n"));
  State->serialOn = false;
  Serial.print(F(">> Logger: Finished.\n"));
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
    Serial.print(F(">> Logger: SD Card busy. Use command 'S' to stop datalogging and try again.\n>> User:   "));
    return;
  }
  if(!State->SDin)
  {
    Serial.print(F(">> Logger: SD Card already ejected.\n>> User:   "));
    return;
  }
  State->SDin = false;
  Serial.print(F(">> Logger: SD Card may now be removed.\n>> User:   "));
  
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
  Serial.print(F("           h  -- Display help\n"));
  Serial.print(F("           i  -- Initialize the SD card\n"));
  Serial.print(F("           s  -- Start datalogging (will overwrite old datalog.csv)\n"));
  Serial.print(F("           S  -- Stop datalogging\n"));
  Serial.print(F("           u  -- Update date/time\n"));
  Serial.print(F(">> User:   "));

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
    Serial.print(F(">> Logger: SD Ready.\n>> User:   "));
  }
  else
  {
    State->SDin = false; // Should already be false, but just in case.
    Serial.print(F(">> Logger: Error. Is the SD Card inserted?\n>> User:   "));
  }
  SDPowerDown();
  
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
 *    Enable Hall Effect Sensor interrupt
 *    Enable 4-Second RTC interrupt
 *    Prompt User
 *  Else
 *    Prompt User (no SD Card)
 *  Return
\*********************************************/

void startLogging(volatile State_t* State)
{
  if(State->SDin)
  {
    State->logging = true;
    EIMSK |= (1 << INT0);         // Enable Hall Effect Sensor interrupt.
    Serial.print(F(">> Logger: Logging started.\n>> User:   "));
  }
  else
  {
    Serial.print(F(">> Logger: Cannot log without SD Card.\n>> User:   "));
  }
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
 *  If device is logging
 *    Set logging flag false
 *    Disnable Hall Effect Sensor interrupt
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
    Serial.print(F(">> Logger: Logging stopped.\n>> User:   "));
  }
  else
  {
    Serial.print(F(">> Logger: Not logging.\n>> User:   "));
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
  
  Serial.print(F(">> Logger: Enter new month: (mm)"));
  month1  = getNestedInput();
  month10 = getNestedInput();

  Serial.print(F("\n>> Logger: Enter new day: (dd)"));
  day1  = getNestedInput();
  day10 = getNestedInput();

  Serial.print(F("\n>> Logger: Enter new year: (yy)"));
  year1  = getNestedInput();
  year10 = getNestedInput();

  Serial.print(F("\n>> Logger: Enter new hour: (hh, 24-hour clock)"));
  hour1  = getNestedInput();
  hour10 = getNestedInput();

  Serial.print(F("\n>> Logger: Enter new minute: (mm)"));
  minute1  = getNestedInput();
  minute10 = getNestedInput();

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

  Date->months  = months;
  Date->days    = days;
  Date->years   = years;
  Date->hours   = hours;
  Date->minutes = minutes;

  viewDateTime(Date);

  Serial.print(F("\n>> Logger: Date and Time reset.\n>> User:   "));

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
    Serial.println(input);    // For the Arduino IDE Serial Monitor. Other serial monitors may not need this.

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
