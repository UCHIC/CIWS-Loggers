/*********************************************************************\
 * File:      handleSerial.h
 * Date:      04/17/2019
 * Authors:   Joshua Tracy and Daniel Henshaw
 * Hardware:  Pololu LIS3MDL 3-Axis magnetometer 
 *********************************************************************/

#ifndef HANDLESERIAL_H
#define HANDLESERIAL_H

#include <Arduino.h>


#include "state.h"
#include "RTC_PCF8523.h"
#include "configuration.h"

void handleSerial(volatile State_t* State, Date_t* Date, volatile SignalState_t* SignalState); // updated 4/17/19 by D.H.
void setConfiguration(volatile State_t* State);
void cleanSD(volatile State_t* State);
void viewDateTime(Date_t* Date);
void exitSerial(volatile State_t* State, Date_t* Date);
void ejectSD(volatile State_t* State);
void printHelp();
void initSD(volatile State_t* State);
void listFiles(void);
void startLogging(volatile State_t* State, volatile SignalState_t* SignalState, volatile Date_t* Date); // updated 4/17/19 by D.H.
void stopLogging(volatile State_t* State);
void updateDateTime(Date_t* Date);
void RTC_Doctor();
char getInput();
char getNestedInput();
void printWater(State_t* State);
void printConfig(State_t* State);
void createHeader(State_t* State);
void nameFile(State_t* State, Date_t* Date);
void incrementFileNumber(void);

#endif
