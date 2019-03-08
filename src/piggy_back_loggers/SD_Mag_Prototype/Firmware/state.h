#ifndef STATE_H
#define STATE_H

#include <Arduino.h>

// State Struct

typedef struct
{
  volatile byte pulseCount;
  volatile unsigned long recordNum;
  volatile bool logging;
  volatile bool flag4;
  volatile bool serialOn;
  volatile bool SDin;
  volatile bool readMag;
  const char filename[12] = "datalog.csv";
} State_t;

// Signal State Struct

typedef struct
{
  volatile float x = 0;
  volatile float s = 0;
  volatile float sf[2] = {0, 0};
  volatile float a = 0.2;
  volatile float y[2] = {0, 0};
  volatile float offset = 0.0005;
  bool slopeWasPositive = false;
  bool slopeIsPositive = false;
} SignalState_t;

void resetState(volatile State_t* State);

#endif
