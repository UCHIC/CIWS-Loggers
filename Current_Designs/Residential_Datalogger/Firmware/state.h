#ifndef STATE_H
#define STATE_H

#include <Arduino.h>

// State Struct

typedef struct
{
  volatile byte pulseCount;
  volatile byte lastCount;
  volatile unsigned int totalCount;
  volatile unsigned long recordNum;
  volatile bool logging;
  volatile bool flag4;
  volatile bool serialOn;
  volatile bool SDin;
  volatile bool readMag;
  volatile char meterSize;
  volatile bool configured;
  volatile char filename[13];
} State_t;

// Signal State Struct

typedef struct
{
  volatile float x[2] = {0, 0};           // changed from float to float array of size two. Date changed: 4/22/19 by D.H. per J.T.'s request (to support new Schmitt Trigger algorithm)
  volatile float s = 0;
  volatile float sf[2] = {0, 0};
  volatile float a = 0.95;
  volatile float y[2] = {0, 0};
  volatile float offset = -0.005;
  bool slopeWasPositive = false;
  bool slopeIsPositive = false;
  bool triggered = false;
} SignalState_t;

void resetState(volatile State_t* State);

#endif
