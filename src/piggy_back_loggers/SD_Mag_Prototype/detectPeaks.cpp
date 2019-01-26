#include "detectPeaks.h"

bool peakDetected(volatile SignalState_t* signalState)
{
  float s = 0;
  bool peak = false;

  signalState->offset = (signalState->x_max - signalState->x_min) / 2.208;

  float high_thresh = signalState->x_max - signalState->offset;
  float low_thresh = signalState->x_min + signalState->offset;

  s = signalState->x[1] - signalState->x[0];
  
  if(s > 0)
  {
    signalState->slopeIsPositive = true;
    signalState->slopeIsNegative = false;
    if (signalState->x[1] > signalState->x_max)
    {
      signalState->x_max = signalState->x[1];
      high_thresh = signalState->x_max - signalState->offset;
    }
  }
  else if(s < 0)
  {
    signalState->slopeIsPositive = 0;
    signalState->slopeIsNegative = 1;
    if (signalState->x[1] < signalState->x_min)
    {
      signalState->x_min = signalState->x[1];
      low_thresh = signalState->x_min + signalState->offset;
    }
  }

  if(!signalState->slopeIsNegative && signalState->slopeWasNegative)
  {
    signalState->currentMin = signalState->x[0];
  }

  if(!signalState->slopeIsPositive && signalState->slopeWasPositive)
  {
    signalState->currentMax = signalState->x[0];
    if((signalState->currentMax > high_thresh) && (signalState->currentMin < low_thresh))
    {
      peak = true;
    }
  }

  if(signalState->slopeIsPositive)
    signalState->slopeWasPositive = true;
  else
    signalState->slopeWasPositive = false;

  if(signalState->slopeIsNegative)
    signalState->slopeWasNegative = true;
  else
    signalState->slopeWasNegative = false;
  
  return peak;
}
