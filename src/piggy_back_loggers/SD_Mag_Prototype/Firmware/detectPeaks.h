#ifndef DETECTPEAKS_H
#define DETECTPEAKS_H

#include <Arduino.h>
#include "state.h"

bool peakDetected(volatile SignalState_t* signalState);

#endif
