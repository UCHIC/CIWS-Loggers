#include "configuration.h"

bool configurationExists(void)
{
  bool exist = false;

  uint8_t addr = addr_chs0;
  uint8_t i;
  uint8_t chs[4];

  for(i = 0; i < 4; i++)
  {
    chs[i] = readConfiguration(addr);
    addr++;
  }

  uint8_t check1 = chs[3] + chs[2];
  uint8_t check0 = chs[1] + chs[0];

  if((check1 == 0xFF) && (check0 == 0xFF))
    exist = true;

  return exist;
  
}

uint8_t readConfiguration(uint8_t segment)
{
  while(EECR & (1 << EEPE));  // Wait for completion of any previous write
  EEAR = segment;             // Load EEPROM Address Register with address to read
  EECR |= (1 << EERE);        // Start EEPROM Read

  return EEDR;                // Return read data
}

void writeConfiguration(uint8_t segment, char data)
{
  while(EECR & (1 << EEPE));  // Wait for completion of previous write
  EEAR = segment;             // Load EEPROM Address Register with address to write
  EEDR = data;                // Load EEPROM Data Register with data to write
  EECR |= (1 << EEMPE);       // Enable EEPROM Writes
  EECR |= (1 << EEPE);        // Start EEPROM Write

  return;
}
