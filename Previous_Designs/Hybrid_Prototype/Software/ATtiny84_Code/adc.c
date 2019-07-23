/*-----------------------------------------------------*\
|  ATtiny84 ADC                               		    |
|                                                       |
| This library provides an interface to the ADC 	    |
| hardware found on the ATtiny84 (single-ended mode). 	|
|                                                       |
| Joshua Tracy - 6/2/2018      							|
|            -Utah Water Research Lab                   |
\*-----------------------------------------------------*/

#include "adc.h"

#include <avr/io.h>

void ADC_init(void)
{
	ADC_Enable();
	select_ADC_reference(vcc);
	select_ADC_input(adc0);
	ADCSRA_config(0x90);
	ADCSRB_config(0x10);
	
	return;
}

void ADC_Enable(void)						// Enable ADC
{
	ADCSRA |= 0x80;
	return;
}

void select_ADC_reference(char reference)	// Select ADC Reference Voltage
{
	reference = reference << 6;
	ADMUX |= reference;
	return;
}

void select_ADC_input(char input)			// Select ADC Input
{
	ADMUX |= input;
	return;
}

void ADCSRA_config(char config)				// Configure ADCSRA
{
	ADCSRA = config;
	return;
}

void ADCSRB_config(char config)				// Configure ADCSRB
{
	ADCSRB = config;
	return;
}

void start_conversion(void)					// Start Conversion
{
	ADCSRA |= 0x40;
	return;
}

char poll_interrupt_flag(void)				// Poll Interrupt Flag
{
	char flag = 0;
	if((ADCSRA & 0x10) == 0x10)
		flag = 1;
	return flag;
}

void clear_interrupt_flag(void)				// Clear Interrupt Flag
{
	ADCSRA |= 0x10;
}

unsigned char read_ADCL(void)				// Read ADCL
{
	unsigned char result_low = ADCL;
	return result_low;
}

unsigned char read_ADCH(void)				// Read ADCH
{
	unsigned char result_high = ADCH;
	return result_high;
}
