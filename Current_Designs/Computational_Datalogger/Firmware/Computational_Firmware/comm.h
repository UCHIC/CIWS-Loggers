#ifndef COMM_H
#define COMM_H

#include "powerSleep.h"
#include "avr/io.h"
#include "avr/interrupt.h"
#include "RTC_PCF8523.h"
#include "state.h"
#include "storeNewRecord.h"

#define FOSC 8000000
#define BAUD 9600
#define UBRR FOSC/16/BAUD-1

#define usartReceiveComplete() ((UCSR0A&0x80)==0x80)              // Check to see if a new byte has been received

void updateReport(char* report, Date_t* Date, State_t* State);    // Update information regarding System State and Date/Time based on the new report (9 received bytes)

void powerRPiON(void);                                            // Power up the host computer (designed for a Raspberry Pi). Also enables host computer SPI bus and switches microcontroller chip select pin to input in order to avoid bus contention
void powerRPiOFF(void);                                           // Power off the host computer

void UART_Init(unsigned int ubrr);                                // Initialzie the UART Module with a specific value for the ubrr (use UBRR for preset BAUD above)
void UART_Transmit(unsigned char data);                           // Transmit a byte to the host computer
unsigned char UART_Receive(void);                                 // Read the last received byte from the host computer
void UART_End(void);                                              // Disable the UART module, set TX pin as input to avoid driving unpowered host computer receive pin

void spiInit(void);                                               // Initialize the SPI module
void spiOff(void);                                                // Set SPI pins to input so as not to contend with host computer SPI bus
void spiSelectSlave(void);                                        // Assert the chip select pin
void spiReleaseSlave(void);                                       // De-assert (is that a word?) the chip select pin
void spiTransceive(unsigned char* data, unsigned char dataSize);  // Transmit data to the SPI slave

#endif
