#include "powerSleep.h"

#include <Arduino.h>
#include <avr/sleep.h>
#include <avr/power.h>

void enterSleep()
{
  /*** Setup Sleep options ***/
  
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);  // Lowest powerdown mode
  sleep_enable();                       // Enable sleep

  /*** Put processor to sleep ***/

  sleep_mode();

  /*** Continue ***/
  
  sleep_disable();

  return;
}
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

void serialPowerUp()
{
  power_usart0_enable();
  Serial.begin(9600);
  while(!Serial);
  Serial.print(F(">> Logger: Logger ready.\n>> User:   "));

  return;
}

void serialPowerDown()
{
  power_usart0_disable();

  return;
}

void SDPowerUp()
{
  power_spi_enable();
  digitalWrite(4, LOW);

  return;
}

void SDPowerDown()
{
  digitalWrite(4, HIGH);
  power_spi_disable();

  return;
}
