#ifndef HANDLESERIAL_H
#define HANDLESERIAL_H

#include "state.h"

void handleSerial(State_t* State);
void cleanSD(State_t* State);
void viewDateTime();
void exitSerial(State_t* State);
void ejectSD(State_t* State);
void printHelp();
void initSD(State_t* State);
void startLogging(State_t* State);
void stopLogging(State_t* State);
void updateDateTime();
char getInput();

#endif
