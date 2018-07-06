#include <avr/io.h>
#include <util/delay.h>

#include "rpi_pow.h"

void rpi_ioinit(void)
{
	DDRA |= (1 << PA7); 		// PA7 output -- PA7 is connected to a MOSFET switch, and determines if power is supplied to the Raspberry Pi.
	DDRA &= ~(1 << PA2);		// PA2 input  -- PA2 is connected to a GPIO on the Raspberry Pi. This signal is on as long as the Raspberry Pi is on, with the exception of startup and shutdown delays.
	DDRA &= ~(1 << PA1);		// PA1 input  -- PA1 is connected to a button, which the user will push in order to manually turn on the Raspberry Pi and keep it on in order to retrieve datalog files.
	DDRA |= (1 << PA3);		// PA3 output -- PA3 is connected to a GPIO on the Raspberry Pi. This signal is set on a manual button press, to tell the Raspberry Pi not to shut itself off as it normally does.
	DDRA &= ~(1 << PA5);		// PA5 input  -- PA5 is connected to a GPIO on the Raspberry Pi. This signal tells the ATtiny whether or not the EEPROM is free to be written.
	PORTA |= (1 << PA7);		// Start Raspberry Pi off (Signal Active LOW)
	PORTA &= ~(1 << PA3);		// Start "Stay On" Signal off
	asm("nop");
	return;
}

void rpi_on(void)
{
	PORTA &= ~(1 << PA7);		// Turn on RPi
	_delay_ms(5000);
	while(!(PINA & 0x4));		// Wait while RPi signal is low
	return;
}

void rpi_off(void)
{
	while(PINA & 0x4);			// Wait while RPi signal is high
	_delay_ms(5000);			// Wait 10 seconds
	_delay_ms(5000);
	PORTA |= (1 << PA7);		// Turn off RPi
	
	return;
}
