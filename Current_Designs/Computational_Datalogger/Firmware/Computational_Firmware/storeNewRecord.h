#ifndef STORENEWRECORD_H
#define STORENEWRECORD_H

#include "state.h"
#include "comm.h"
#include <Arduino.h>

#define romDataBufferSize 128

void writeDataSize(State_t* State);   // Writes the number of records to the first three bytes of the EEPROM chip
void writeDateAndTime(Date_t* Date);  // Stores the date and time of the first data byte in bytes 3 - 8 of the EEPROM chip
void storeNewRecord(State_t* State);  // Stores a new record, either on an external EEPROM chip or an array in RAM. Designed for Microchip's 25LC1024 SPI Serial EEPROM
                                      // If RAM array gets full, it assumes the host computer is frozen and sets the flag State.RPiFalseON accordingly. Change the value for romDataBufferSize above for different buffer sizes. Be careful, don't max out memory.
                                        // This prevents infinite loops and data loss if the host computer freezes before releasing control of the EEPROM or if it never turns on at all.
#endif
