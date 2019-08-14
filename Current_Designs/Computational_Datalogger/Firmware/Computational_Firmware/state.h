#ifndef STATE_H
#define STATE_H

#include <Arduino.h>

#define HEADER_SIZE 9

// State Struct

typedef struct
{
  volatile byte pulseCount;
  volatile byte lastCount;
  volatile unsigned int totalCount;
  volatile unsigned long recordNum;
  volatile unsigned long romAddr;
  volatile bool logging;
  volatile bool flag4;
  volatile bool SDin;
  volatile bool readMag;
  volatile bool newReport;
  volatile bool RPiON;
  volatile bool powerGood;
  volatile bool romFree;
  volatile bool RPiFalseON;
} State_t;

// Signal State Struct

typedef struct
{
  volatile float x[2] = {0, 0};
  volatile float a = 0.95;
  volatile float y[2] = {0, 0};
  bool slopeWasPositive = false;
  bool slopeIsPositive = false;
  bool triggered = false;
} SignalState_t;

void resetState(volatile State_t* State);

#endif
