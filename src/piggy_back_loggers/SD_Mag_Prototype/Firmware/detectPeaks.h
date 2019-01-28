#ifndef DETECTPEAKS_H
#define DETECTPEAKS_H

#include <Arduino.h>
#include "state.h"

/*TODO: Add a calibration period to ensure noise is ignored.*/

bool peakDetected(volatile SignalState_t* signalState); // Used to detect if a peak has occured. Attempts to filter noise.

#endif
