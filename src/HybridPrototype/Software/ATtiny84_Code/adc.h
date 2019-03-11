/*-----------------------------------------------------*\
|  ATtiny84 ADC                               		    |
|                                                       |
| This library provides an interface to the ADC 	    |
| hardware found on the ATtiny84 (single-ended mode). 	|
|                                                       |
| Joshua Tracy - 6/2/2018      							|
|            -Utah Water Research Lab                   |
\*-----------------------------------------------------*/

#ifndef ADC_H
#define ADC_H

#define vcc 0x00			// Values passed to select_ADC_reference
#define aref 0x01
#define int_1V1 0x02

#define adc0 0x00			// Values passed to select_ADC_input
#define adc1 0x01
#define adc2 0x02
#define adc3 0x03
#define adc4 0x04
#define adc5 0x05
#define adc6 0x06
#define adc7 0x07
#define agnd 0x20
#define i_1v1 0x21
#define temp_sensor 0x22

void ADC_init(void);              // Initialize ADC
void ADC_Enable(void);						// Enable ADC
void select_ADC_reference(char reference);	// Select ADC Reference Voltage
void select_ADC_input(char input);			// Select ADC Input
void ADCSRA_config(char config);			// Configure ADCSRA
void ADCSRB_config(char config);			// Configure ADCSRB
void start_conversion(void);				// Start Conversion
char poll_interrupt_flag(void);				// Poll Interrupt Flag
void clear_interrupt_flag(void);			// Clear Interrupt Flag
unsigned char read_ADCL(void);				// Read ADCL
unsigned char read_ADCH(void);				// Read ADCH

#endif
