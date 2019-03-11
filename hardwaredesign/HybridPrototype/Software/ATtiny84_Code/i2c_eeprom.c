#include "i2c_eeprom.h"
#include "usi_i2c_master.h"

void EEPROM_write(unsigned char chip_addr, unsigned int addr, unsigned char* data, unsigned int data_size)
{
	unsigned int i;
	unsigned char data_buffer[data_size + 3];						// Setup new data buffer
	unsigned char block = (addr >> 13) & 0x8;						// Isolate the Block select bit
	data_buffer[0] = (chip_addr << 1) + WRITE + block;				// Load data buffer with I2C address and Write bit
	data_buffer[1] = (addr >> 8) & 0x00FF;							// Load data buffer with EEPROM memory address
	data_buffer[2] = (addr & 0x00FF);
	for (i = 3; i < data_size + 3; i++)							// Load data buffer
	{
		data_buffer[i] = data[i - 3];
	}
	
	USI_I2C_Master_Start_Transmission(data_buffer, data_size + 3);	// Transmit
}

void EEPROM_read(unsigned char chip_addr, unsigned int addr, unsigned char* data, unsigned int data_size)
{
	unsigned int i;
	EEPROM_write(chip_addr, addr, data, 0);						// Transmit Address to read at
	char data_buffer[data_size + 1];								// Setup new data buffer
	data_buffer[0] = (chip_addr << 1) + READ;						// Load data buffer with I2C address and Read bit
	
	USI_I2C_Master_Start_Transmission(data_buffer, data_size + 1);	// Transmit Read request, Read EEPROM
	
	for (i = 1; i < data_size + 1; i++)							// Load data buffer
	{
		data[i - 1] = data_buffer[i];
	}
}

void EEPROM_init(void)
{
	USI_I2C_Master_Init();
	return;
}
