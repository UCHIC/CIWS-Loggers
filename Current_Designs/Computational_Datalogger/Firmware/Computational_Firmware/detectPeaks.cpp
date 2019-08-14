/*********************************************************************\
 * File:      detectPeaks.cpp
 * Date:      04/17/2019
 * Authors:   Joshua Tracy and Daniel Henshaw
 * Hardware:  Pololu LIS3MDL 3-Axis magnetometer 
 *********************************************************************/

#include "detectPeaks.h"

/*********************************************************\
 * Function Name: peakDetected
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

bool peakDetected(volatile SignalState_t* signalState)
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
