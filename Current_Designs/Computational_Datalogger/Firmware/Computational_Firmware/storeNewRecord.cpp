#include "storeNewRecord.h"

unsigned char romDataBuffer[romDataBufferSize];       // Data buffer for when the RPi is in control of the EEPROM chip. Gets copied to EEPROM once freed.
unsigned char romDataBufferIndex = 0;                 // romDataBufferIndex always points to the next available memory location in the buffer.

unsigned char writeInstr = 0x02;                      // This instruction tells the EEPROM chip the controller wants to write to it.
unsigned char wrenInstr = 0x06;                       // This instruction enables writes to the EEPROM chip. Must be done for each write operation.

void writeDataSize(State_t* State)
{
  delay(6);                                           // delay for 6 ms in case the EEPROM is writing.
  
  unsigned char data[7];                              // This is the array that will be populated and then written to the EEPROM
  
  unsigned long recordNum  = State->recordNum - 1;    // Use recordNum to tell number of records. Subtract one because the number of records on the EEPROM chip is always one less than State.recordNum
  unsigned char recordNum0, recordNum1, recordNum2;
  
  recordNum0 = recordNum & 0xFF;                      // Split the recordNum into three bytes
  recordNum  = recordNum >> 8;
  recordNum1 = recordNum & 0xFF;
  recordNum  = recordNum >> 8;
  recordNum2 = recordNum & 0xFF;

  data[0] = writeInstr;                               // Load data array with the write instruction, address 0, and recordNum
  data[1] = 0;
  data[2] = 0;
  data[3] = 0;
  data[4] = recordNum2;
  data[5] = recordNum1;
  data[6] = recordNum0;
  
  spiSelectSlave();                                   // Select the EEPROM chip
  spiTransceive(&wrenInstr, 1);                       // Send the Write Enable instruction
  spiReleaseSlave();                                  // De-Select the EEPROM chip (or writing will not be enabled)

  spiSelectSlave();                                   // Select the EEPROM chip
  spiTransceive(data, 7);                             // Write data to the EEPROM chip
  spiReleaseSlave();                                  // De-Select the EEPROM chip

  return;
}

void writeDateAndTime(Date_t* Date)
{
  unsigned char dateTimeArray[10];                    // Holds write instruction, address, and the date/time to be written

  dateTimeArray[0] = writeInstr;                      // Write Instruction: tells EEPROM chip to receive data to write
  dateTimeArray[1] = 0;                               // Store starting at address 0x003
  dateTimeArray[2] = 0;
  dateTimeArray[3] = 3;
  dateTimeArray[4] = Date->years;                     // Years
  dateTimeArray[5] = Date->months;                    // Months
  dateTimeArray[6] = Date->days;                      // Days
  dateTimeArray[7] = Date->hours;                     // Hours
  dateTimeArray[8] = Date->minutes;                   // Minutes
  dateTimeArray[9] = Date->seconds + 4;               // Seconds + 4: The first data byte will be 4 seconds after the timestamp for logging start

  spiSelectSlave();                                   // Select the EEPROM chip
  spiTransceive(&wrenInstr, 1);                       // Send the Write Enable instruction
  spiReleaseSlave();                                  // De-Select the EEPROM chip (or writing will not be enabled)

  spiSelectSlave();                                   // Select the EEPROM chip
  spiTransceive(dateTimeArray, 7);                    // Write dateTimeArray to the EEPROM chip
  spiReleaseSlave();                                  // De-Select the EEPROM chip
}

void storeNewRecord(State_t* State)                   // Stores a new record in either the EEPROM chip or the romDataBuffer array.
{
  unsigned char finalCount;                             // Holds the final count for the four-second sample.
  unsigned char romAddr2, romAddr1, romAddr0;           // Each address in the EEPROM is 24 bits, which must be split into three bytes.
  unsigned char data[5];                                // This is the array that will be populated and then written to the EEPROM
  unsigned long romAddr;                                // Holds current ROM address

  finalCount = State->pulseCount;                       // Assign current pulseCount to finalCount
  State->pulseCount = 0;                                // Reset pulseCount to zero
  State->lastCount = finalCount;                        // lastCount is used to generate water usage information for the report swap between microcontroller and host computer. This is used to report the current water flow during a logging session.
  State->totalCount += (unsigned int)finalCount;        // totalCount is also used to generate water usage information. This holds the total water flow during a logging session.

  spiSelectSlave();                                   // Select the EEPROM chip
  spiTransceive(&wrenInstr, 1);                       // Send the Write Enable instruction
  spiReleaseSlave();                                  // De-Select the EEPROM chip (or writing will not be enabled)

  spiSelectSlave();                                   // Select the EEPROM chip
  
  if(State->romFree)                                  // If the microcontroller has control over the EEPROM chip,
  {
    romAddr = State->romAddr;                         // Load the current ROM address (points to next available cell)

    romAddr0 = romAddr & 0xFF;                          // Split the ROM address into three bytes
    romAddr = romAddr >> 8;
    romAddr1 = romAddr & 0xFF;
    romAddr = romAddr >> 8;
    romAddr2 = romAddr & 0xFF;

    if(romDataBufferIndex > 0)                          // If romDataBufferIndex is greater than zero, then there is data in the romDataBuffer that must be written to the EEPROM
    {
      data[0] = writeInstr;                               // Load the data array with the write instruction and three-byte ROM address
      data[1] = romAddr2;
      data[2] = romAddr1;
      data[3] = romAddr0;

      spiTransceive(data, 4);                             // Send the write instruction and the address to write to
      spiTransceive(romDataBuffer, romDataBufferIndex);   // Write the contents of the romDataBuffer to the EEPROM

      State->romAddr += romDataBufferIndex;               // Update the ROM address
      State->recordNum += romDataBufferIndex;             // Update the record number
      romDataBufferIndex = 0;                             // Reset the romDataBufferIndex to zero
    }

    spiTransceive(&finalCount, 1);                      // Send the write instruction, the address to write to, and the data byte to write.
    spiReleaseSlave();                                  // De-Select the EEPROM chip
  
    State->recordNum += 1;                              // Update the record number
    State->romAddr += 1;                                // Update the ROM address
  }
  else                                                // If the microcontroller does not have control over the EEPROM chip
  {
    romDataBuffer[romDataBufferIndex] = finalCount;     // Store data in romDataBuffer
    if(romDataBufferIndex < romDataBufferSize)          // If the buffer is not full
    {
      romDataBufferIndex += 1;                            // Increment the romDataBufferIndex
    }
    else                                                // Otherwise
    {
      State->RPiFalseON = true;                           // Let the rest of the program know that the host computer needs to be powered off.
    }
  }
  
  return;
}