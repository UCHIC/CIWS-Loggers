/*********************************************************************\
 * File:      detectPeaks.cpp
 * Date:      04/17/2019
 * Authors:   Joshua Tracy and Daniel Henshaw
 * Hardware:  Pololu LIS3MDL 3-Axis magnetometer 
 *********************************************************************/

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

  signalState->y[1] = signalState->y[0] + (signalState->a * (signalState->x[1] - signalState->y[0]));        // Low Pass Filter: x to y
 
  signalState->s = signalState->y[1] - signalState->y[0];                                                 // Take discrete derivative of y
  
  signalState->sf[1] = signalState->sf[0] +(signalState->a * (signalState->s - signalState->sf[0]));      // Low Pass Filter: s to sf

  signalState->y[0] = signalState->y[1];                                                                  // Saving current y as old y
  signalState->sf[0] = signalState->sf[1];                                                                // Saving current sf as old sf
  
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

/*********************************************************\
 * Function Name: peakDetected2
 * 
 * Purpose:       Analyze the magnetometer data in 
 *                real-time to see if a peak  has
 *                been detected. Designed for LIS3MDL
 *                
 * Inputs:        Pointer to SignalState_t struct
 * Outputs:       Bool peak: has a peak been detected?
 * 
 * Pseudocode:
 *  Filter: Remove DC Offset
 *  Save current y as old y
 *  Save current x as old x
 *  
 *  If the schmitt trigger has not triggered
 *    If the signal has exceeded the positive threshold
 *      Then the schmitt trigger is triggered
 *      A peak has been detected
 *  If the schmitt trigger has triggered
 *    If the signals has exceeded the negative threshold
 *      Then the schmitt trigger is untriggered
 *  
 *  Return whether or not a peak was detected
\*********************************************************/

bool peakDetected2(volatile SignalState_t* signalState)
{
  bool peak = false;                                                                                      // Default: No peak has been detected
  float pos_thresh = 1;                                                                                 // Positive threshold for schmitt triggering
  float neg_thresh = -1;                                                                                // Negative threshold for schmitt triggering

  signalState->y[1] = signalState->a * signalState->y[0] + signalState->x[1] - signalState->x[0];         // Filter: Remove DC Offset

  signalState->y[0] = signalState->y[1];                                                                  // Save current y as old y
  signalState->x[0] = signalState->x[1];                                                                  // Save current x as old x

  switch(signalState->triggered)
  {
    case false:                                                                                           // If the schmitt trigger has not triggered
      if(signalState->y[1] > pos_thresh) {                                                                  // If the signal has exceeded the positive threshold
        signalState->triggered = true;                                                                        // Then the schmitt trigger is triggered
        peak = true;                                                                                          // A peak has been detected
      }
      break;
    case true:                                                                                            // If the schmitt trigger has triggered
      if(signalState->y[1] < neg_thresh)                                                                    // If the signal has exceeded the negative threshold
        signalState->triggered = false;                                                                       // Then the schmitt trigger is untriggered
        break;
    default:
      break;
  }

  return peak;                                                                                            // Return whether or not a peak was detected.
}
