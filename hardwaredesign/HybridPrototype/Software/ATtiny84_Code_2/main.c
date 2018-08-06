#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "i2c_eeprom.h"
#include "rpi_pow.h"

void int0_init(void);					// Function definitions
void timer_init(void);

#const unsigned short recordMax = 32000;		// Global constant which tracks how full the external EEPROM is. When 32000 records are reached, the Raspberry Pi will be woken up. At 32000, the wake-up will be ~ 1/day, and will not quite fill one EEPROM chip (128 kB). 
const unsigned short recordMax = 10;

unsigned char pulseCount = 0;				// Global Variable which holds the current pulse count from the register.
unsigned short recordNum = 1;				// Global Variable which holds the current record number.
unsigned char EEPROM_Busy = 0;				// Global Flag used to note when the EEPROM is being read by the Raspberry Pi, and should therefore not be written to.

unsigned char dataStorage[360];			// Global Array used to store records (Record Number, Pulse Count, Battery Voltage). Each record will be 3 bytes. When the Timer interrupt triggers and the Raspberry Pi is NOT reading the EEPROMS, then the contents of this array will be written to EEPROM.
unsigned char lenData[2];				// Global Array used to store the length of the Data in the EEPROM. An array is used because the functions to write to EEPROM expect an array. This array is written to the first two bytes of the EEPROM chip.
unsigned char dataIndex = 0;				// Global variable which holds the current array index. This is used to know how much of the dataStorage[] needs to be written to EEPROM.
unsigned int EEPROMindex = 2;				// Global variable which holds the current EEPROM index, so memory in EEPROM is not overwritten. This index starts at two to leave two bytes to specify the length of the data.

const char chipAddr = 0x50;				// I2C address of the EEPROM chip

int main(void)
{
	/*===========================================*\
	| The main function	for the Residential   |
	| Piggy-Back CIWS Meterloggers. An ATtiny84   |
	| reads pulses from a residential water meter |
	| and prepares the data to be read by a       |
	| Raspberry Pi Embedded Computer. The ATtiny  |
	| controls when the Raspberry Pi is on or off |
	| to reduce power consumption. The aim is to  |
	| keep the Raspberry Pi's computing power but |
	| reduce the power consumption of the device. |
	|					      |
	| This is accomplished as follows:            |
	|                                             |
	| 1) Initialization:                          |
	|					      |
	|      The Raspberry Pi's power controlling   |
	|      hardware is initialized.               |
	|            				      |
	|      Interrupt INT0 is initialized. INT0 is |
	|      triggered by a pulse from the water    |
	|      meter. INT0 also begins initialization |
	|      of the timer interrupt.                |
	|					      |
	|      The EEPROM interface is initialized.   |
	|      The EEPROM interface uses I2C to write |
	|      collected data to an external EEPROM   |
	|      chip to be read by the Raspberry Pi.   |
	|	   				      |
	|      The Analog to Digital Converter (ADC)  |
	|      is initialized. The ADC is used to     |
	|      monitor battery voltage.		      |
	|					      |
	|      The timer is initialized with an       |
	|      interrupt to trigger every four        |
	|      seconds.				      |
	|                                             |
	| 2) Main loop:				      |
	|					      |
	|      In the main loop, the program will     |
	|      check to see if the EEPROM is "full".  |
	|      If so, the program will turn on the    |
	|      Raspberry Pi and turn it back off when |
	|      the Raspberry Pi is finished. Updating |
	|      data points and writing to the EEPROMs |
	|      is handled by interrupts.              |
	|      The Raspberry Pi is also turned on     |
	|      when a button is pushed. The ATtiny84  |
	|      sets a signal on PA4 to tell the RPi   |
	|      to stay on and not turn itself off.    |
	|      The user is in charge of shutting down |
	|      the Rasperry Pi. This allows a user    |
	|      to pull data files from the Raspb. Pi. |
	|                                             |
	|      Eventually, it may be prudent to put   |
	|      the microcontroller to sleep and wake  |
	|      it up with interrupts.                 |
	|                                             |
	\*===========================================*/
	
	_delay_ms(1000);
	rpi_ioinit();				// Initialize Raspberry Pi power control and signalling hardware
	I2C_stop();				// Provide a solid stop condition for the I2C buffer chip
	int0_init();				// Initialize Interrupts
	EEPROM_init();				// Initialize EEPROM Interface
	timer_init();				// Initialize Timer
	unsigned short lastRecord = 0;
	while(1)					// Loop:
	{	 
		if (recordNum >= recordMax)		// Check if it's time to wake up RPi
		{
			lastRecord = --recordNum;			// Use the last record number written
			lenData[0] = (lastRecord >> 8);			// Store the number of records in the lenData array.
			lenData[1] = lastRecord & 0x00FF;
			EEPROM_write(chipAddr, 0, lenData, 2); 		// Write length to EEPROM address 0
			
			recordNum = 1;					// Reset recordNum. recordNum is relative to the ATtiny loop and the full record number will be computed on the Raspberry Pi.
			EEPROM_Busy = 1;				// Let the other functions know that the EEPROM is being read by the Pi and should not be written to.
			EEPROMindex = 2;				// Reset EEPROM index (remember, data starts at 2)
			rpi_on();					// If it is time, wake up RPi
			while(PINA & (1 << PA5));			// Wait until Raspberry Pi has finished writing to EEPROM
			PORTA &= ~(1 << PA0);				// Unenable the I2C buffer, disconnect RPi from I2C bus
			EEPROM_Busy = 0;				// Let the other functions know that the EEPROM is free to be written
			rpi_off();					// Turn off Raspberry Pi (The Raspberry Pi shuts itself off. This function will wait until the Raspberry Pi sends a signal to cut its power).
		}
		if ((PINA & (1 << PA1)) == 0)		// Check if the manual Raspberry Pi power-on button has been pushed
		{
			lastRecord = --recordNum;
			lenData[0] = (lastRecord >> 8);			// Store the number of records in the lenData array.
			lenData[1] = lastRecord & 0x00FF;
			EEPROM_write(chipAddr, 0, lenData, 2); 		// Write length to EEPROM address 0
		
			recordNum = 1;					// Reset recordNum. recordNum is relative to the ATtiny loop and the full record number will be computed on the Raspberry Pi.
			EEPROM_Busy = 1;				// Let the other functions know that the EEPROM is being read by the Pi and should not be written to.
			EEPROMindex = 2;				// Reset EEPROM index
			rpi_on();					// Turn on Raspberry Pi
			PORTA |= (1 << PA3);				// Set a signal to keep Raspberry Pi on so user can retrieve data
			while(PINA & (1 << PA5));			// Wait until Raspberry Pi has finished writing to EEPROM
			PORTA &= ~(1 << PA0);				// Unenable the I2C buffer, disconnect RPi from I2C bus
			EEPROM_Busy = 0;				// Let the other functions know that the EEPROM is free to be written
			rpi_off();					// Turn off Raspberry Pi (The Raspberry Pi is shut off by the user. This function will wait until the Raspberry Pi sends a signal to cut its power).
			PORTA &= ~(1 << PA3);				// Turn off "Stay On" Signal
		}
	}
}

void int0_init(void)
{
	sei();				// Initialize Global Interrupts
	MCUCR |= 0x02;			// Interrupt Int0 is connected to the Hall Effect Sensor. This setup triggers Int0 on the falling edge of the input signal, 
	GIMSK |= 0x40;			// which is the behavior of the sensor when it detects a magnet.
	DDRB = (0 << PB2); 		// Int0 is on PB2.
	
	return;
}

void timer_init(void)
{
	TCCR1C = 0x00;			// Disable Force Compare
	TIMSK1 = 0x02;			// Enable CTC OCR1A Interrupt
	TIFR1 |= 0x20;			// Clear Interrupt Flags
	
	TCNT1 = 0x0000;			// Reset Counter	
	OCR1A = 0x3D09;			// Set Compare Value (4 MHz Clock)
	
	TCCR1A = 0x00;			// Control (1024 Prescale, CTC on OCR1A mode)
	TCCR1B = 0x0D;
	return;
}

ISR(INT0_vect)
{
	pulseCount++;			// Increase the pulse count. When the Hall Effect sensor sends a pulse, we wanna track it.
}

ISR(TIM1_COMPA_vect)
{
	char currentPulseCount = pulseCount;		// Store current pulseCount
	pulseCount = 0;					// Reset pulseCount to zero
							// Store Data in Memory Array
	dataStorage[dataIndex] = (recordNum >> 8);		// record number
	dataStorage[dataIndex + 1] = recordNum & 0x00FF;
	dataStorage[dataIndex + 2] = currentPulseCount;		// stored pulse count
	dataIndex += 3;						// Update Array size information
	if(!EEPROM_Busy)					// If the EEPROM is not being read by the Raspberry Pi
	{	
		EEPROM_write(chipAddr, EEPROMindex, dataStorage, dataIndex); // Write Data to EEPROM
		EEPROMindex += dataIndex;
		dataIndex = 0;
	}
	recordNum++;						// Increment record number
	
}
