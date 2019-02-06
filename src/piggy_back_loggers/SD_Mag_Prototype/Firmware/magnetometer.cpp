#include "magnetometer.h"
#include <Arduino.h>
#include <SD.h>
#include <DebugMacros.h>
#include <LSM303CTypes.h>
#include <SparkFunIMU.h>
#include <SparkFunLSM303C.h>
#include "powerSleep.h"
#include "state.h"

/*********************************************************************\
 * 
 * Library for interfacing with the SparkFun LSM303C magnetometer.
 * 
 * Functions:
 *    Initialize Magnetomter 
 *    Read Data
 *    Initialize Data
 *    
\*********************************************************************/

void magnetometerInit(LSM303C *mag)
{
  twiPowerUp();
  
  if (mag->begin(
    MODE_I2C,
    MAG_DO_80_Hz,
    MAG_FS_8_Ga,
    MAG_BDU_DISABLE,
    MAG_OMXY_LOW_POWER,
    MAG_OMZ_MEDIUM_PERFORMANCE,
    MAG_MD_CONTINUOUS,
    ACC_FS_2g,
    ACC_BDU_DISABLE,
    ACC_DISABLE_ALL,
    ACC_ODR_POWER_DOWN
    ) != IMU_SUCCESS)
    {
      Serial.println("Magnetometer setup failed. Check connection and reset.");
      while(1);
    }
    twiPowerDown();

    return;
}

void readData(LSM303C* mag, volatile SignalState_t* SignalState)
{
  twiPowerUp();
  SignalState->x[0] = SignalState->x[1];
  SignalState->x[1] = mag->readMagZ();
  twiPowerDown();
  
  return;
}


void initializeData(LSM303C* mag, volatile SignalState_t* SignalState)
{
  twiPowerUp();
  SignalState->x[0] = mag->readMagZ();
  SignalState->x[1] = SignalState->x[0];
  if(SignalState->firstRun)
  {
    SignalState->firstRun = false;
    SignalState->x_max = SignalState->x[1];
    SignalState->x_min = SignalState->x[1];
    SignalState->currentMax = SignalState->x[1];
    SignalState->currentMin = SignalState->x[1];
  }
  twiPowerDown();
  
  return;
}
