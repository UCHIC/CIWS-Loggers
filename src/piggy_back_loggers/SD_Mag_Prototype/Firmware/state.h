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
  volatile float x[2];
  volatile float currentMax;
  volatile float currentMin;
  volatile float x_max;
  volatile float x_min;
  volatile float offset = 0;
  bool slopeWasPositive = false;
  bool slopeIsPositive = false;
  bool slopeWasNegative = false;
  bool slopeIsNegative = false;
  bool firstRun = true;
} SignalState_t;

void resetState(volatile State_t* State);

#endif
