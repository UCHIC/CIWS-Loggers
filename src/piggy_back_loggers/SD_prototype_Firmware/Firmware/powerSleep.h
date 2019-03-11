#ifndef POWERSLEEP_H
#define POWERSLEEP_H

#include <avr/sleep.h>
#include <avr/power.h>

void enterSleep();
void disableUnneededPeripherals();
void twiPowerUp();
void twiPowerDown();
void serialPowerUp();
void serialPowerDown();
void SDPowerUp();
void SDPowerDown();

#endif
