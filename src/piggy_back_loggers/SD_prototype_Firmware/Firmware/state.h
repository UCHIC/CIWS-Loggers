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
  const char filename[12] = "datalog.csv";
} State_t;

void resetState(volatile State_t* State);

#endif
