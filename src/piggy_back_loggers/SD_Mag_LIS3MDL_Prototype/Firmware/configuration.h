#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <Arduino.h>
#include <avr/io.h>
#include "state.h"

#define addr_siteNum 0
#define addr_meterSize 1
#define addr_logID 2
#define addr_sensorType 3

#define addr_chs0 6
#define addr_chs1 7
#define addr_chs2 8
#define addr_chs3 9

bool configurationExists(void);
bool checkConfiguration(State_t* State);
uint8_t readConfiguration(uint8_t segment);
void writeConfiguration(uint8_t segment, char data);

#endif
