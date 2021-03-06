#ifndef RTCPCF8523_H
#define RTCPCF8523_H

#include <Arduino.h>
#include <Wire.h>
#include "powerSleep.h"

const byte deviceAddr = 0x68;

const byte reg_Control_1       = 0x00;
const byte reg_Control_2       = 0x01;
const byte reg_Control_3       = 0x02;
const byte reg_Seconds         = 0x03;
const byte reg_Minutes         = 0x04;
const byte reg_Hours           = 0x05;
const byte reg_Days            = 0x06;
const byte reg_Weekdays        = 0x07;
const byte reg_Months          = 0x08;
const byte reg_Years           = 0x09;
const byte reg_Minute_alarm    = 0x0A;
const byte reg_Hour_alarm      = 0x0B;
const byte reg_Day_alarm       = 0x0C;
const byte reg_Weekday_alarm   = 0x0D;
const byte reg_Offset          = 0x0E;
const byte reg_Tmr_CLKOUT_ctrl = 0x0F;
const byte reg_Tmr_A_freq_ctrl = 0x10;
const byte reg_Tmr_A_reg       = 0x11;
const byte reg_Tmr_B_freq_ctrl = 0x12;
const byte reg_Tmr_B_reg       = 0x13;

const byte WRITE = 0;
const byte READ  = 1;

typedef struct
{
  volatile byte seconds;
  volatile byte minutes;
  volatile byte hours;
  volatile byte days;
  volatile byte months;
  volatile byte years;
  
} Date_t;

byte rtcTransfer(byte reg, byte flag, byte value);
void registerDump();
void loadDateTime(Date_t* Date);

#endif
