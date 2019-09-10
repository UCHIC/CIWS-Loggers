#include "powerSleep.h"

#include <Arduino.h>
#include <Wire.h>
#include <avr/sleep.h>
#include <avr/power.h>

#define NOP __asm__ __volatile__ ("nop\n\t")

/***************************************\
 * Function Name: enterSleep()
 * Purpose:       Put the controller to
 *                "sleep" (low-power).
 * Inputs:        None
 * Outputs:       None
 * pseudocode:
 *   Setup sleep options
 *   Put processor to sleep
 *   Continue on wake-up
 *   Return
\***************************************/
/*
void enterSleep()
{
  set_sleep_mode(SLEEP_MODE_STANDBY);  // Lowest powerdown mode
  sleep_enable();                       // Enable sleep

  sleep_mode();
  
  sleep_disable();

  return;
}
*/
/**********************************************\
 * Function Name: disableUnneededPeripherals
 * Purpose:       Cut all power to peripherals
 *                to save battery life. Some 
 *                will be turned on again
 *                temporarily.
 * Inputs:        None
 * Outputs:       None
 * Pseudocode:
 *   Disable ADC
 *   Disable Timer0
 *   Disable Timer1
 *   Disable Timer2
 *   Disable TWI
 *   Disable USART0
 *   Disable SPI
 *   Return
\**********************************************/

void disableUnneededPeripherals()
{
  /*** Disable Uneeded Peripherals ***/
  ADCSRA = 0;   // Disable ADC
  power_adc_disable();
  power_timer0_disable();
  power_timer1_disable();
  power_timer2_disable();
  power_twi_disable();
  power_usart0_disable();
  power_spi_disable();

  return;
}

/**********************************\
 * Function Name: twiPowerUp()
 * Purpose:       Enable power to
 *                the TWI interface
 *                when needed.
 * Inputs:        None
 * Outputs:       None
 * pseudocode:
 *  Power on the TWI interface
 *  Initialize the TWI class
 *  Return
\**********************************/

void twiPowerUp()
{
  power_twi_enable();
  NOP;
  Wire.begin();
  NOP;

  return;
}

/**********************************\
 * Function Name: twiPowerDown()
 * Purpose:       Disable power to
 *                the TWI interface
 *                when finished.
 * Inputs:        None
 * Outputs:       None
 * pseudocode:
 *  Power down the TWI interface
 *  Return
\**********************************/

void twiPowerDown()
{
  power_twi_disable();

  return;
}

/*************************************\
 * Function Name: serialPowerUp()
 * Purpose:       Enable power to
 *                the serial interface
 *                when needed.
 * Inputs:        None
 * Outputs:       None
 * pseudocode:
 *  Power on the serial interface
 *  Pause 10 miliseconds
 *  Initialize Serial at 9600 Baud
 *  Pause 10 miliseconds
 *  Prompt user
 *  Return
\*************************************/

void serialPowerUp()
{
  power_usart0_enable();
  NOP;

  return;
}

/*************************************\
 * Function Name: serialPowerDown()
 * Purpose:       Disable power to
 *                the serial interface
 *                when finished.
 * Inputs:        None
 * Outputs:       None
 * pseudocode:
 *  Power down the serial interface
 *  Return
\*************************************/

void serialPowerDown()
{
  power_usart0_disable();

  return;
}

/**********************************\
 * Function Name: SDPowerUp()
 * Purpose:       Enable power to
 *                the SPI module
 *                when needed.
 * Inputs:        None
 * Outputs:       None
 * pseudocode:
 *  Power on the SPI interface
 *  Return
\**********************************/

void spiPowerUp()
{
  power_spi_enable();
  NOP;

  return;
}

/**********************************\
 * Function Name: SDPowerDown()
 * Purpose:       Disable power to
 *                the SPI module
 *                when finished.
 * Inputs:        None
 * Outputs:       None
 * pseudocode:
 *  Power down the SPI interface
 *  Return
\**********************************/

void spiPowerDown()
{
  power_spi_disable();    // Power down the SPI interface

  return;
}
