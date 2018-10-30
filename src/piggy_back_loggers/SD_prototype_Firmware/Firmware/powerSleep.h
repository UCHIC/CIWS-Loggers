#ifndef POWERSLEEP_H
#define POWERSLEEP_H

#include <avr/sleep.h>
#include <avr/power.h>
#include "state.h"

void enterSleep(State_t* State);
void disableUnneededPeripherals();

#endif
