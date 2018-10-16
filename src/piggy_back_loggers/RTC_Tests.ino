// Test code to practice talking to the PCF8523 RTC on the Datalogger Shield

#include <Wire.h>

char sunday[]    = "Sunday";
char monday[]    = "Monday";
char tuesday[]   = "Tuesday";
char wednesday[] = "Wednesday";
char thursday[]  = "Thursday";
char friday[]    = "Friday";
char saturday[]  = "Saturday";

char january[]   = "January";
char february[]  = "February";
char march[]     = "March";
char april[]     = "April";
char may[]       = "May";
char june[]      = "June";
char july []     = "July";
char august[]    = "August";
char september[] = "September";
char october[]   = "October";
char november[]  = "November";
char december[]  = "December";

byte deviceAddr = 0x68;

byte reg_Control_1       = 0x00;
byte reg_Control_2       = 0x01;
byte reg_Control_3       = 0x02;
byte reg_Seconds         = 0x03;
byte reg_Minutes         = 0x04;
byte reg_Hours           = 0x05;
byte reg_Days            = 0x06;
byte reg_Weekdays        = 0x07;
byte reg_Months          = 0x08;
byte reg_Years           = 0x09;
byte reg_Minute_alarm    = 0x0A;
byte reg_Hour_alarm      = 0x0B;
byte reg_Day_alarm       = 0x0C;
byte reg_Weekday_alarm   = 0x0D;
byte reg_Offset          = 0x0E;
byte reg_Tmr_CLKOUT_ctrl = 0x0F;
byte reg_Tmr_A_freq_ctrl = 0x10;
byte reg_Tmr_A_reg       = 0x11;
byte reg_Tmr_B_freq_ctrl = 0x12;
byte reg_Tmr_B_reg       = 0x13;

byte seconds     = 0;
byte minutes     = 0;
byte hours       = 0;
byte days        = 0;
byte weekdaynum  = 0;
char *weekdays   = 0;
byte monthnum    = 0;
char *months     = 0;
byte years       = 0;

volatile bool FLAG = false;

void setup()
{
  // put your setup code here, to run once:
  Wire.begin();
  Serial.begin(9600);
  while(!Serial);

  pinMode(7, OUTPUT);
  digitalWrite(7, HIGH);

  pinMode(3, INPUT);
  attachInterrupt(digitalPinToInterrupt(3), INT1_ISR, FALLING);
  
  Serial.println(F("PCF8523 Registers:"));
  Wire.beginTransmission(deviceAddr);
  Wire.write(reg_Control_1);
  Wire.endTransmission();
  
  Wire.requestFrom(deviceAddr, byte(20));
  byte reg = 0;
  while(Wire.available())
  {
    byte c = Wire.read();
    Serial.print(F("Register "));
    Serial.print(reg);
    Serial.print(F(": "));
    Serial.println(c);
    switch(reg)
    {
      case 3:
        seconds = c;
        break;
      case 4:
        minutes = c;
        break;
      case 5:
        hours = c;
        break;
      case 6:
        days = c;
        break;
      case 7:
        weekdaynum = c;
        break;
      case 8:
        monthnum = c;
        break;
      case 9:
        years = c;
        break;
      default:
        break;
    }
    reg += 1;
  }
  Serial.println();

  if(seconds >= 128)
  {
    Serial.println(F("/!\\ WARNING: RTC oscillator frozen. Recommend reset.\n"));
  }

  seconds  = (seconds & 0x0F)  + ((seconds >> 4) * 10);
  minutes  = (minutes & 0x0F)  + ((minutes >> 4) * 10);
  hours    = (hours & 0x0F)    + ((hours >> 4) * 10);
  days     = (days & 0x0F)     + ((days >> 4) * 10);
  monthnum = (monthnum & 0x0F) + ((monthnum >> 4) * 10);
  years    = (years & 0x0F)    + ((years >> 4) * 10);
  
  switch(weekdaynum)
  {
    case 0:
      weekdays = sunday;
      break;
    case 1:
      weekdays = monday;
      break;
    case 2:
      weekdays = tuesday;
      break;
    case 3:
      weekdays = wednesday;
      break;
    case 4:
      weekdays = thursday;
      break;
    case 5:
      weekdays = friday;
      break;
    case 6:
      weekdays = saturday;
      break;
    default:
      break;
  }

  switch(monthnum)
  {
    case 1:
      months = january;
      break;
    case 2:
      months = february;
      break;
    case 3:
      months = march;
      break;
    case 4:
      months = april;
      break;
    case 5:
      months = may;
      break;
    case 6:
      months = june;
      break;
    case 7:
      months = july;
      break;
    case 8:
      months = august;
      break;
    case 9:
      months = september;
      break;
    case 10:
      months = october;
      break;
    case 11:
      months = november;
      break;
    case 12:
      months = december;
      break;
    default:
      break;
  }
  Serial.println(F("UTC Time:"));
  Serial.print(monthnum);
  Serial.print(F("/"));
  Serial.print(days);
  Serial.print(F("/"));
  Serial.print(years);
  Serial.print(F(" "));
  Serial.print(hours);
  Serial.print(F(":"));
  if(minutes < 10)
    Serial.print(F("0"));
  Serial.print(minutes);
  Serial.print(F(":"));
  if(seconds < 10)
    Serial.print(F("0"));
  Serial.println(seconds);
  Serial.print(F("Month: "));
  Serial.println(months);
  Serial.print(F("Day of the week: "));
  Serial.println(weekdays);

  Wire.beginTransmission(deviceAddr);
  Wire.write(reg_Tmr_CLKOUT_ctrl);
  Wire.write(byte(0x3A));
  Wire.endTransmission();

  Wire.beginTransmission(deviceAddr);
  Wire.write(reg_Tmr_A_freq_ctrl);
  Wire.write(byte(0x02));
  Wire.endTransmission();

  Wire.beginTransmission(deviceAddr);
  Wire.write(reg_Tmr_A_reg);
  Wire.write(byte(0x04));
  Wire.endTransmission();

  Wire.beginTransmission(deviceAddr);
  Wire.write(reg_Control_2);
  Wire.write(byte(0x02));
  Wire.endTransmission();
}

void loop()
{  
  // put your main code here, to run repeatedly:
  while(1)
  {
    if(FLAG)
    {
      digitalWrite(7, LOW);
      FLAG = false;
      Wire.beginTransmission(deviceAddr);
      Wire.write(reg_Control_2);
      Wire.write(byte(0x02));
      Wire.endTransmission();
      digitalWrite(7, HIGH);
    }
  }
}

void INT1_ISR()                 // Serial On Button ISR. Activate Serial Interface
{
  FLAG = true;
}
