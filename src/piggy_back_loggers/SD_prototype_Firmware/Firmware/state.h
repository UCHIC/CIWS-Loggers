#ifndef STATE_H
#define STATE_H

// State Struct

typedef struct
{
  volatile char pulseCount;
  volatile unsigned long recordNum;
  volatile bool logging;
  volatile bool flag4;
  volatile bool serialOn;
  volatile bool SDin;
  const char filename[12] = "datalog.csv";
} State_t;

void resetState(State_t* State);

#endif
