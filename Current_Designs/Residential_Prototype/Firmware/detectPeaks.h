/*********************************************************************\
 * File:      detectPeaks.h
 * Date:      04/17/2019
 * Authors:   Joshua Tracy and Daniel Henshaw
 * Hardware:  Pololu LIS3MDL 3-Axis magnetometer 
 *********************************************************************/

#ifndef DETECTPEAKS_H
#define DETECTPEAKS_H

#include <Arduino.h>
#include "state.h"

bool peakDetected(volatile SignalState_t* signalState);   // Used to detect if a peak has occured. Attempts to filter noise.
bool peakDetected2(volatile SignalState_t* signalState);  // Used to detect if a peak has occured. This version removes the signal's DC offset and ignores noise through a software schmitt trigger.

#endif
