#include "state.h"

void resetState(volatile State_t* State)
{
  State->pulseCount = 0;
  State->lastCount = 0;
  State->totalCount = 0;
  State->recordNum = 1;
  State->romAddr = HEADER_SIZE;
  State->logging = false;
  State->flag4 = false;
  State->SDin = false;
  State->readMag = false;
  State->newReport = false;
  State->RPiON = false;
  State->powerGood = false;
  State->romFree = true;
  State->RPiFalseON = false;
  return;
}
