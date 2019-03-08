#include "detectPeaks.h"
#include "powerSleep.h"
#include <SD.h>

/*****************************************************\
 * Function Name: peakDetected
 * 
 * Purpose:       Analyze the magnetometer data in 
 *                real-time to see if a peak  has
 *                been detected.
 *                
 * Inputs:        Pointer to State_t struct
 * Outputs:       Bool peak: has a peak been detected?
 * 
 * Pseudocode:
 *  Low Pass Filter: x to y
 *  Take discrete derivative of y
 *  Low Pass Filter: s to sf
 *  Add DC offset to derivative
 *  If the derivative is greater than zero
 *    Then the slope of the signal is positive
 *  Else If the derivative is less than zero
 *    Then the slope of the signal is negative
 *  If the slope is not positive and it was positive
 *    Then a peak has been detected
 *  If the slope is positive
 *    Then on the next loop, the slope was positive
 *  Otherwise,
 *    On the next loop, the slope was not positive
 *  
 *  Return whether or not a peak was detected
\*****************************************************/

bool peakDetected(volatile SignalState_t* signalState)
{
  bool peak = false;                                                                                      // Default: No peak has been detected

  signalState->y[1] = signalState->y[0] + (signalState->a * (signalState->x - signalState->y[0]));        // Low Pass Filter: x to y
 
  signalState->s = signalState->y[1] - signalState->y[0];                                                 // Take discrete derivative of y
  
  signalState->sf[1] = signalState->sf[0] +(signalState->a * (signalState->s - signalState->sf[0]));      // Low Pass Filter: s to sf
  signalState->sf[1] += signalState->offset;                                                              // Add DC offset to derivative (ignores noise at zero flow)
  
  if(signalState->sf[1] > 0)                                                                              // If the magnitude of the derivative is greater than zero
    signalState->slopeIsPositive = true;                                                                  //    Then the slope of the signal is positive
  else if(signalState->sf[1] < 0)                                                                         // Else If the magnitude of the derivative is less than zero
    signalState->slopeIsPositive = false;                                                                 //    Then the slope of the signal is negative
    
  if(!signalState->slopeIsPositive && signalState->slopeWasPositive)                                      // If the slope is not positive and it was positive
    peak = true;                                                                                          //    Then a peak has been detected

  if(signalState->slopeIsPositive)                                                                        // If the slope is positive
    signalState->slopeWasPositive = true;                                                                 //    Then on the next loop, the slope was positive
  else                                                                                                    // Otherwise
    signalState->slopeWasPositive = false;                                                                //    On the next loop, the slope was not positive
  
  return peak;                                                                                            // Return whether or not a peak was detected.
}
