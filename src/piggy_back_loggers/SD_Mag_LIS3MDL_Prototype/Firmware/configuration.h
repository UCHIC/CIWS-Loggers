#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <Arduino.h>
#include <avr/io.h>
#include "state.h"

#define addr_siteNum100  0
#define addr_siteNum10   1
#define addr_siteNum1    2
#define addr_meterSize   3
#define addr_logID100    4
#define addr_logID10     5
#define addr_logID1      6
#define addr_fileNum1000 7
#define addr_fileNum100  8
#define addr_fileNum10   9
#define addr_fileNum1    10

#define addr_chs0 11
#define addr_chs1 12
#define addr_chs2 13
#define addr_chs3 14

bool configurationExists(void);
uint8_t readConfiguration(uint8_t segment);
void writeConfiguration(uint8_t segment, char data);

#endif
