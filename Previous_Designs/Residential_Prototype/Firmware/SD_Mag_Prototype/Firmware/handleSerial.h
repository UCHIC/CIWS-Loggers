#ifndef HANDLESERIAL_H
#define HANDLESERIAL_H

#include <Arduino.h>
#include <DebugMacros.h>
#include <LSM303CTypes.h>
#include <SparkFunIMU.h>
#include <SparkFunLSM303C.h>

#include "state.h"
#include "RTC_PCF8523.h"

void handleSerial(volatile State_t* State, Date_t* Date, volatile SignalState_t* SignalState, LSM303C* mag);
void setMeter(volatile SignalState_t* SignalState);
void cleanSD(volatile State_t* State);
void viewDateTime(Date_t* Date);
void exitSerial(volatile State_t* State, Date_t* Date);
void ejectSD(volatile State_t* State);
void printHelp();
void initSD(volatile State_t* State);
void startLogging(volatile State_t* State, LSM303C* mag, volatile SignalState_t* SignalState);
void stopLogging(volatile State_t* State);
void updateDateTime(Date_t* Date);
void RTC_Doctor();
char getInput();
char getNestedInput();

#endif
