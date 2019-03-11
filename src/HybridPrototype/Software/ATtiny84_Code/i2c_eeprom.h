#ifndef I2C_EEPROM_H
#define I2C_EEPROM_H

#include "usi_i2c_master.h"

void EEPROM_write(unsigned char chip_addr, unsigned int addr, unsigned char* data, unsigned int data_size);
void EEPROM_read(unsigned char chip_addr, unsigned int addr, unsigned char* data, unsigned int data_size);
void EEPROM_init(void);

#endif