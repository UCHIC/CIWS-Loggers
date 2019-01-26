#ifndef MAGNETOMETER_H
#define MAGNETOMETER_H

#include "state.h"
#include <Arduino.h>
#include <DebugMacros.h>
#include <LSM303CTypes.h>
#include <SparkFunIMU.h>
#include <SparkFunLSM303C.h>

/*********************************************************************\
 * 
 * Library for interfacing with the SparkFun LSM303C magnetometer.
 * 
 * Functions:
 *    Initialize Magnetomter 
 *    Read Data
 *    
\*********************************************************************/

void magnetometerInit(LSM303C *mag);
void readData(LSM303C* mag, volatile SignalState_t* SignalState);
void initializeData(LSM303C* mag, volatile SignalState_t* SignalState);

#endif
