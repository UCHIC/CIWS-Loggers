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
        break;

      case 'd':
        break;

      case 'e':
        Serial.print(F(">> Logger: Exitting... "));
        State->serialOn = false;
        Serial.print(F(">> Logger: Finished."));
        power_usart0_disable();
        break;

      case 'E':
        break;

      case 'h':                                                                 // Help list
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
        break;

      case 'i':
        break;

      case 's':
        State->logging = true;
        break;

      case 'S':
        break;

      case 'u':
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
