#include "state.h"

void resetState(volatile State_t* State)
{
  State->pulseCount = 0;
  State->lastCount = 0;
  State->totalCount = 0;
  State->recordNum = 1;
  State->logging = false;
  State->flag4 = false;
  State->serialOn = false;
  State->SDin = false;
  State->readMag = false;
  State->configured = false;

  return;
}
