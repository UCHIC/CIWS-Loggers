#include "handleSerial.h"

#include <SPI.h>
#include <SD.h>
#include "state.h"
#include "powerSleep.h"

void handleSerial(State_t* State)
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
        viewDateTime();
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
        updateDateTime();
        break;

      case '\n':
        break;

      default:                  // If the command is invalid, prompt the user and introduce 'h' option.
        Serial.print(F(">> Logger: Unknown command. Use the command \"h\" for a list of commands.\n>> User:   "));
        break;
    }
  }
}

/*******************************************************************************************************************************\
 * Supporting Functions:
 *   void cleanSD(State_t* State);          Complete
 *   void viewDateTime();
 *   void exitSerial(State_t* State);       Complete
 *   void ejectSD(State_t* State);          Complete
 *   void printHelp();                      Complete
 *   void initSD(State_t* State);           Complete
 *   void startLogging(State_t* State);     Complete
 *   void stopLogging(State_t* State);      Complete
 *   void updateDateTime();
 *   char getInput();
\********************************************************************************************************************************/

void cleanSD(State_t* State)
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
    Serial.print(F(">> Logger: Cleaning SD Card... "));
    SD.remove(F("datalog.csv"));
    Serial.print(F(">> Logger: SD Card clean!\n>> User:   "));
    SDPowerDown();
  }
  else
  {
    Serial.print(F(">> Logger: Cannot clean SD Card while logging.\n>> User:   "));
  }

  return;
}

void viewDateTime()
{
  // Request Time and Date from RTC
  // Construct a Timestamp
  // Print the Timestamp

  return;
}

void exitSerial(State_t* State)
{
  Serial.print(F(">> Logger: Exitting... "));
  State->serialOn = false;
  Serial.print(F(">> Logger: Finished."));
  serialPowerDown();
  
  return;  
}

void ejectSD(State_t* State)
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
  Serial.print(F("           v  -- View files\n"));
  Serial.print(F(">> User:   "));

  return;
}

void initSD(State_t* State)
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

void startLogging(State_t* State)
{
  State->logging = true;
  EIMSK |= (1 << INT0);         // Enable Hall Effect Sensor interrupt.
  EIMSK |= (1 << INT1);         // Enable 4-Second RTC interrupt.
  Serial.print(F(">> Logger: Logging started.\n>> User:   "));
  
  return;
}

void stopLogging(State_t* State)
{
  EIMSK &= ~(1 << INT0);         // Disable Hall Effect Sensor interrupt
  EIMSK &= ~(1 << INT1);         // Disable 4-Second RTC interrupt.
  State->logging = false;
  Serial.print(F(">> Logger: Logging stopped.\n>> User:   "));

  return;
}

void updateDateTime()
{
  // Receive Time and Date from user
  // Write Time and Date to RTC

  return;
}

char getInput()
{
  char input = Serial.read();
  if (input != '\n')          // Ignore newline characters.
    Serial.println(input);    // For the Arduino IDE Serial Monitor. Other serial monitors probably don't need this.

  return input;
}
