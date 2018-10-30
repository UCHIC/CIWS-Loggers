#include "handleSerial.h"

#include <SPI.h>
#include <SD.h>
#include "state.h"
#include "powerSleep.h"

void handleSerial(State_t* State)
{
  char input; 
  if(Serial.available() > 0)    // Check if serial data is available.
  {
    input = Serial.read();
    if (input != '\n')          // Ignore newline characters.
      Serial.println(input);    // For the Arduino IDE Serial Monitor. Other serial monitors probably don't need this.

    switch(input)               // Switch statement for all defined inputs
    {
      case 'c':                                         // Delete files on the SD Card. Protected from deleting while in use.
        cleanSD(State);
        break;

      case 'd':
        viewDateTime();
        break;

      case 'e':
        exitSerial(State);
        break;

      case 'E':
        ejectSD(State);
        break;

      case 'h':                                         // Help list
        printHelp();
        break;

      case 'i':
        initSD(State);
        break;

      case 's':
        startLogging(State);
        break;

      case 'S':
        stopLogging(State);
        break;

      case 'u':
        updateDateTime();
        break;
        
      case 'v':
        viewFiles();
        break;

      case '\n':
        break;

      default:                                  // If the command is invalid, prompt the user and introduce 'h' option.
        Serial.print(F(">> Logger: Unknown command. Use the command \"h\" for a list of commands.\n>> User:   "));
        break;
    }
  }
}

void serialPowerUp()
{
  power_usart0_enable();
  Serial.begin(9600);
  while(!Serial);
  Serial.print(F(">> Logger: Logger ready.\n>> User:   "));

  return;
}

void cleanSD(State_t* State)
{
  if(!State->SDin)
  {
    Serial.print(F(">> Logger: SD Card not initialized.\n>> User:   "));
    return;
  }
  
  if(!State->logging)
  {
    power_spi_enable();
    digitalWrite(4, LOW);
    SD.begin();
    Serial.print(F(">> Logger: Cleaning SD Card... "));
    SD.remove(F("datalog.csv"));
    Serial.print(F(">> Logger: SD Card clean!\n>> User:   "));
    digitalWrite(4, HIGH);
    power_spi_disable();
  }
  else
  {
    Serial.print(F(">> Logger: Cannot clean SD Card while logging.\n>> User:   "));
  }

  return;
}

void viewDateTime()
{
  
}

void exitSerial(State_t* State)
{
  Serial.print(F(">> Logger: Exitting... "));
  State->serialOn = false;
  Serial.print(F(">> Logger: Finished."));
  power_usart0_disable();
  
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
  
}

void startLogging(State_t* State)
{
  State->logging = true;
  Serial.print(F(">> Logger: Logging started.\n>> User:   "));
  
  return;
}

void stopLogging(State_t* State)
{
  State->logging = false;
  Serial.print(F(">> Logger: Logging stopped.\n>> User:   "));

  return;
}

void updateDateTime()
{
  
}
void viewFiles()
{
  
}
