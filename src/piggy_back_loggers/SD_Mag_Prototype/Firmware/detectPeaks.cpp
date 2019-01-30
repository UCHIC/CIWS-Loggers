#include "detectPeaks.h"
#include "powerSleep.h"
#include <SD.h>

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
    signalState->slopeIsPositive = false;
    signalState->slopeIsNegative = true;
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

  /* Data to plot the waveform measured by the magnetometer. For testing purposes. To be removed when no longer needed. */
  /* Along with the wavefrom, the calculated peaks are marked, where 1 indicates a peak and -2 indicates no peak.*/
  SDPowerUp();
  File waveFile = SD.open("waveform.csv", FILE_WRITE);
  waveFile.print(signalState->x[1], 4);
  if(peak)
    waveFile.print(",1");   // 1 on the graph means a peak
  else
    waveFile.print(",-2");  // -2 on the graph means no peak
  waveFile.println();
  waveFile.close();
  SDPowerDown();
  
  return peak;
}
