#include "storeNewRecord.h"

unsigned char romDataBuffer[romDataBufferSize];       // Data buffer for when the RPi is in control of the EEPROM chip. Gets copied to EEPROM once freed.
unsigned char romDataBufferIndex = 0;                 // romDataBufferIndex always points to the next available memory location in the buffer.

#define writeInstr 0x02                               // This instruction tells the EEPROM chip the controller wants to write to it.
#define wrenInstr 0x06                                // This instruction enables writes to the EEPROM chip. Must be done for each write operation.

void writeDataSize(State_t* State)
{
  delay(10);                                          // delay for 10 ms in case the EEPROM is writing.
  
  unsigned char data[7];                              // This is the array that will be populated and then written to the EEPROM

  unsigned char writeEnable[1];                       // writeEnable[0] is a copy of wrenInstr. Since spiTranceive overwrites input data, can't just pass a pointer to wrenInstr.
  writeEnable[0] = wrenInstr;

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
  spiTransceive(writeEnable, 1);                      // Send the Write Enable instruction
  spiReleaseSlave();                                  // De-Select the EEPROM chip (or writing will not be enabled)

  spiSelectSlave();                                   // Select the EEPROM chip
  spiTransceive(data, 7);                             // Write data to the EEPROM chip
  spiReleaseSlave();                                  // De-Select the EEPROM chip

  return;
}

void writeDateAndTime(Date_t* Date)
{
  delay(10);                                          // delay for 10 ms in case the EEPROM is writing.

  unsigned char dateTimeArray[10];                    // Holds write instruction, address, and the date/time to be written

  unsigned char writeEnable[1];                       // writeEnable[0] is a copy of wrenInstr. Since spiTranceive overwrites input data, can't just pass a pointer to wrenInstr.
  writeEnable[0] = wrenInstr;

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
  spiTransceive(writeEnable, 1);                      // Send the Write Enable instruction
  spiReleaseSlave();                                  // De-Select the EEPROM chip (or writing will not be enabled)

  spiSelectSlave();                                   // Select the EEPROM chip
  spiTransceive(dateTimeArray, 10);                   // Write dateTimeArray to the EEPROM chip
  spiReleaseSlave();                                  // De-Select the EEPROM chip
}

void storeNewRecord(State_t* State)                   // Stores a new record in either the EEPROM chip or the romDataBuffer array.
{
  delay(10);                                            // delay for 10 ms in case the EEPROM is writing.
  unsigned char finalCount;                             // Holds the final count for the four-second sample.
  unsigned char romAddr2, romAddr1, romAddr0;           // Each address in the EEPROM is 24 bits, which must be split into three bytes.
  unsigned char data[5];                                // This is the array that will be populated and then written to the EEPROM
  unsigned long romAddr;                                // Holds current ROM address
  unsigned long pageBoundary = 0;                       // This variable holds a page boundary address if one is about to be crossed (Bad ju-ju, don't want that)
  unsigned long i;                                      // This variable is used for looping over all the addresses about to be written when checking for page boundaries.

  unsigned char writeEnable[1];                       // writeEnable[0] is a copy of wrenInstr. Since spiTranceive overwrites input data, can't just pass a pointer to wrenInstr.
  writeEnable[0] = wrenInstr;

  finalCount = State->pulseCount;                       // Assign current pulseCount to finalCount
  State->pulseCount = 0;                                // Reset pulseCount to zero
  State->lastCount = finalCount;                        // lastCount is used to generate water usage information for the report swap between microcontroller and host computer. This is used to report the current water flow during a logging session.
  State->totalCount += (unsigned int)finalCount;        // totalCount is also used to generate water usage information. This holds the total water flow during a logging session.
  
  if(State->romFree)                                  // If the microcontroller has control over the EEPROM chip,
  {
    spiSelectSlave();                                   // Select the EEPROM chip
    spiTransceive(writeEnable, 1);                      // Send the Write Enable instruction
    spiReleaseSlave();                                  // De-Select the EEPROM chip (or writing will not be enabled)

    spiSelectSlave();                                   // Select the EEPROM chip

    romAddr = State->romAddr;                         // Load the current ROM address (points to next available cell)

    romAddr0 = romAddr & 0xFF;                          // Split the ROM address into three bytes
    romAddr = romAddr >> 8;
    romAddr1 = romAddr & 0xFF;
    romAddr = romAddr >> 8;
    romAddr2 = romAddr & 0xFF;

    data[0] = writeInstr;                               // Load the data array with the write instruction and three-byte ROM address
    data[1] = romAddr2;
    data[2] = romAddr1;
    data[3] = romAddr0;

    spiTransceive(data, 4);                             // Send the write instruction and the address to write to

    if(romDataBufferIndex > 0)                          // If romDataBufferIndex is greater than zero, then there is data in the romDataBuffer that must be written to the EEPROM //TODO: Check for page boundary
    {
      for(i = romAddr; i < romDataBufferIndex; i++)       // Look at all the addresses we're about to write to
      {
        if(i % 256 == 0)                                    // If any of them are the beginning of a new page
        {
          pageBoundary = i;                                 // Keep the address of the page boundary.
          i = romDataBufferIndex;                           // Set i to break out of the for loop
        }
      }
      if(pageBoundary == 0)
      {
        spiTransceive(romDataBuffer, romDataBufferIndex);   // Write the contents of the romDataBuffer to the EEPROM
      }
      else
      {
        unsigned char firstDataSize = (unsigned char)pageBoundary - romAddr;
        spiTransceive(romDataBuffer, firstDataSize);
        for(i = firstDataSize; i < romDataBufferIndex; i++)
        {
          romDataBuffer[i - firstDataSize] = romDataBuffer[i];
        }
        spiReleaseSlave();
        delay(10);                                          // delay for 10 ms while the EEPROM is writing
        writeEnable[0] = wrenInstr;
        spiSelectSlave();                                   // Select the EEPROM chip
        spiTransceive(writeEnable, 1);                      // Send the Write Enable instruction
        spiReleaseSlave();                                  // De-Select the EEPROM chip (or writing will not be enabled)

        romAddr0 = pageBoundary & 0xFF;                          // Split the ROM address into three bytes
        pageBoundary = pageBoundary >> 8;
        romAddr1 = pageBoundary & 0xFF;
        pageBoundary = pageBoundary >> 8;
        romAddr2 = pageBoundary & 0xFF;

        data[0] = writeInstr;                               // Load the data array with the write instruction and three-byte ROM address
        data[1] = romAddr2;
        data[2] = romAddr1;
        data[3] = romAddr0;
        
        spiSelectSlave();
        spiTransceive(data, 4);                             // Send the write instruction and the address to write to
        spiTransceive(romDataBuffer, romDataBufferIndex - firstDataSize);
      }

      State->romAddr += romDataBufferIndex;               // Update the ROM address
      State->recordNum += romDataBufferIndex;             // Update the record number
      romDataBufferIndex = 0;                             // Reset the romDataBufferIndex to zero
      romAddr = State->romAddr;
      if(romAddr % 256 == 0)
      {
        spiReleaseSlave();
        delay(10);                                          // delay for 10 ms while the EEPROM is writing
        writeEnable[0] = wrenInstr;
        spiSelectSlave();                                   // Select the EEPROM chip
        spiTransceive(writeEnable, 1);                      // Send the Write Enable instruction
        spiReleaseSlave();                                  // De-Select the EEPROM chip (or writing will not be enabled)

        romAddr0 = romAddr & 0xFF;                          // Split the ROM address into three bytes
        romAddr = romAddr >> 8;
        romAddr1 = romAddr & 0xFF;
        romAddr = romAddr >> 8;
        romAddr2 = romAddr & 0xFF;

        data[0] = writeInstr;                               // Load the data array with the write instruction and three-byte ROM address
        data[1] = romAddr2;
        data[2] = romAddr1;
        data[3] = romAddr0;
        spiSelectSlave();
        spiTransceive(data, 4);                             // Send the write instruction and the address to write to
      }
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
