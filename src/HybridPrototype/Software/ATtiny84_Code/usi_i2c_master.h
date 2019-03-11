/*-----------------------------------------------------*\
|  USI I2C Slave Master                                 |
|                                                       |
| This library provides a robust I2C master protocol    |
| implementation on top of Atmel's Universal Serial     |
| Interface (USI) found in many ATTiny microcontrollers.|
|                                                       |
| Adam Honse (GitHub: CalcProgrammer1) - 7/29/2012      |
|            -calcprogrammer1@gmail.com                 |
\*-----------------------------------------------------*/

/*--------------------------------------------------------------------------------*\
| MIT License																	   |
| 																				   |
| Copyright (c) 2016 JDat														   |
| 																				   |
| Permission is hereby granted, free of charge, to any person obtaining a copy	   |
| of this software and associated documentation files (the "Software"), to deal    |
| in the Software without restriction, including without limitation the rights     |
| to use, copy, modify, merge, publish, distribute, sublicense, and/or sell        |
| copies of the Software, and to permit persons to whom the Software is			   |
| furnished to do so, subject to the following conditions:						   |
| 																				   |
| The above copyright notice and this permission notice shall be included in all   |
| copies or substantial portions of the Software.								   |
| 																				   |
| THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR	   |
| IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,	       |
| FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE	   |
| AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER		   |
| LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,	   |
| OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE	   |
| SOFTWARE.																		   |
\*--------------------------------------------------------------------------------*/

#ifndef USI_I2C_MASTER_H
#define USI_I2C_MASTER_H

#include <avr/io.h>
#include <util/delay.h>

//I2C Bus Specification v2.1 FAST mode timing limits
#ifdef I2C_FAST_MODE	
	#define I2C_TLOW	1.3
	#define I2C_THIGH	0.6

//I2C Bus Specification v2.1 STANDARD mode timing limits
#else
	#define I2C_TLOW	4.7
	#define I2C_THIGH	4.0
#endif

//Microcontroller Dependent Definitions
#if defined (__AVR_ATtiny24__) | \
	defined (__AVR_ATtiny44__) | \
	defined (__AVR_ATtiny84__)
	#define DDR_USI			DDRA
	#define PORT_USI		PORTA
	#define PIN_USI			PINA
	#define PORT_USI_SDA	PA6
	#define PORT_USI_SCL	PA4
	#define PIN_USI_SDA		PINA6
	#define PIN_USI_SCL		PINA4
#endif

#if defined(__AVR_AT90Tiny2313__) | \
	defined(__AVR_ATtiny2313__)
    #define DDR_USI             DDRB
    #define PORT_USI            PORTB
    #define PIN_USI             PINB
    #define PORT_USI_SDA        PB5
    #define PORT_USI_SCL        PB7
    #define PIN_USI_SDA         PINB5
    #define PIN_USI_SCL         PINB7
#endif

#define READ 1
#define WRITE 0
//USI I2C Master Transceiver Start
// Starts the transceiver to send or receive data based on the r/w bit
char USI_I2C_Master_Start_Transmission(char *msg, char msg_size);
void USI_I2C_Master_Init(void);

#endif
