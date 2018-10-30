#ifndef POWERSLEEP_H
#define POWERSLEEP_H

#include <avr/sleep.h>
#include <avr/power.h>

void enterSleep();
void disableUnneededPeripherals();

#endif
