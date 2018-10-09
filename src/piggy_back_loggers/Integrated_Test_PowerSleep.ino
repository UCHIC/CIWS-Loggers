// Experimental code integrating a Hall Effect Sensor, Serial Interface, and a microSD card for the data logger project
// Arduino IDE ver. 1.8.7
// Utah Water Research Lab
// Updated: 10/2/2018
// Note: F("String") keeps string literals in program memory and out of RAM. Saves RAM space. Very good. Don't remove the F. I know it looks funny. But don't do it. Really. The program might crash if you do. And then you'll have dishonor on yourself, dishonor on your cow...

/******************************************************************************************\
* Hardware Description
*  ________           ___________________                             _______
* |        |         |                   |<------------------------->|       | microSD
* |        |<------->|                   |<-----[] Activate Serial   |______/
* |________|         |___________________|<-----[] Sensor
*  Serial             Controller
* 
* Serial: Serial interface for user interaction with the logger
* Controller: Arduino Pro/Pro Mini (used for SD library and lower power options)
* microSD: External storage for the logger.
* Activate Serial: Button to wake up the controller and activate the Serial interface
* Sensor: Hall Effect Sensor for logging pulses from water meters.
* 
* /!\ Watchdog timer timeout too long at 3.3 V. Consider options:
*         Run at 5V, level-shift for microSD
*         External RTC
* /!\ Power: Tracco does good job, but has low efficiency around 2 mA. Consider options:
*                 Be ok with it because it still has lots of battery life
*                 Find Switching Low Dropout Low Quiescient Regulator
* 
\******************************************************************************************/

/*********************************************************\
* Software Description
* Overview: 
*   User inputs:
*     Serial input.
*   Device outputs:
*     Serial output
*     Datalog file
*     
* Structure:
*   1. Setup
*   2. Loop
*      If serial is enabled
*        Handle serial input
*      Else
*        Sleep the processor (low-power mode)
*      On wakeup:
*        If awoken because 4 second-interval is up
*          Update time
*          If logging
*            Write data to microSD card
*      Repeat Loop
*      
* Interrupts:
*   1. INT0 (Hall Effect Sensor)
*      On interrupt, the number of pulses increments.
*   2. INT1 (Activate Serial Button)
*      On interrupt, activates serial interface for user
*   3. Watchdog Timer
*      On interrupt (~4 seconds), sets flag to tell
*      controller to update the time.
*      /!\ At 3.3 V, the Watchdog timeout is ~4.4 seconds
*          Considering other timing alternatives.
\*********************************************************/

#include <SPI.h>
#include <SD.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>

File dataFile;

//volatile byte dataBuffer[630];                     // Buffer of 9-byte samples to be written to the SD card. This permits 70 Records. It should fill up every 4 1/2 minutes or so. // The other option is to use an external EEPROM chip

volatile byte pulseCount = 0;
volatile unsigned long recordNum  = 1;
volatile bool logging    = false;
volatile bool timesUp    = false;
volatile bool serialOn   = true;
volatile bool SDinit     = false;

byte seconds = 0;
byte minutes = 55;
byte hours = 17;
byte days = 18;
byte months = 9;
short years = 2018;

/* Function: void setup()
 * Inputs: None
 * Outputs: None
 * Runs first.
 * Overview:
 *   setup() Sets up the hardware and software by:
 *   - Disabling uneeded peripherals to save power
 *   - Initializing serial communication between controller and host computer
 *   - Initializing external interrupts (INT0 and INT1)
 *   - Initializing the Watchdog Timer
 * Finally, setup() prompts the user over serial, letting the user know the logger is ready.
*/

void setup()                         
{  
  /*** Disable Uneeded Peripherals ***/
  ADCSRA = 0;   // Disable ADC
  power_adc_disable();
  power_timer0_disable();
  power_timer1_disable();
  power_timer2_disable();
  power_twi_disable();

  /*** Enable needed Peripherals ***/
  power_usart0_enable();
  power_spi_enable();

  /*** Initialize Serial Communication ***/
  Serial.begin(9600);                                               // Initialize Serial Communication
  while(!Serial);

  pinMode(7, OUTPUT);                                               // LED for debugging purposes

  /*** Set Up External Interrupts ***/
  
  pinMode(2, INPUT);                                                // Setup interrupt INT0 (Hall Effect Sensor, interrupt INT1 (Serial Start Button), and Timer Interrupt
  pinMode(3, INPUT);
  attachInterrupt(digitalPinToInterrupt(2), INT0_ISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(3), INT1_ISR, FALLING);

  EIMSK &= ~(1 << INT0);         // Disable Hall Effect Sensor interrupt
  EIMSK &= ~(1 << INT1);         // Disable Serial Start Button (Serial is active on power-up)

  /*** Setup the Watchdog Timer ***/

  MCUSR &= ~(1 << WDRF);              // Clear the Reset Flag
  WDTCSR |= (1 << WDCE) | (1 << WDE); // Allows change of prescaler value
  WDTCSR = (1 << WDP3);               // Prescale value 512K (~4.39 s)
  WDTCSR |= _BV(WDIE);                // Enable WDT interrupt (No Reset)

  /*** Prompt User ***/
  Serial.print(F(">> Logger: Logger ready.\n>> User 1: "));
}

/* Function: void loop()
 * Inputs: None
 * Outputs: None
 * Runs second (after void setup())
 * Overview:
 *   loop() is the main program loop, which executes as follows:
 *     If serial is active
 *       Handle the serial interaction
 *     Else
 *       Sleep // This "Sleep" halts just about everything on the microcontroller.
 *             // The microcontroller is woken up by:
 *             // 1. A 4-second timeout from the Watchdog Timer
 *             // 2. A signal from the Hall Effect Sensor
 *             // 3. A signal from the Activate Serial Button
 *     If the timesUp flag is set
 *       Unset timesUp flag
 *       Update the time
 *       If datalogging is active
 *         Write data out to the microSD card
 *         
 *   This loop executes for as long as the microcontroller is on. Data collection, timing, and serial interface activation
 *   are handled by interrupts. Note that the serial interface starts active until exited.
 *                
*/

void loop()
{
  /*** Serial Communication if active ***/
  
  if(serialOn)                  // This will eventually be turned on/off by the user. Right now, Serial is always on.
    handleSerial();
  else
  {
    enterSleep();
  }
  
  /*** Update time/data if time is up ***/  
  
  if(timesUp)                   // Update date/time     
  {
    timesUp = false;
    digitalWrite(7, HIGH);      // Debugging LED on
    upDateTime();
    if(logging)                   // If logging is set and 4 seconds have passed, write data to the SD card. 
    {                             // In the final setup, the SD will not be written every 4 seconds (power usage problems).
      SD_Write();
    }
    digitalWrite(7, LOW);       // Debugging LED off.
  }
}

void enterSleep()
{

  /*** Setup Sleep options ***/
  
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);  // Lowest powerdown mode
  sleep_enable();                       // Enable sleep

  /*** Put processor to sleep ***/

  sleep_mode();

  /*** Continue ***/
  
  sleep_disable();

  /*** Re-enable necessary peripherals ***/
  if(serialOn)
  {
    power_usart0_enable();
  }
  power_spi_enable();
}

void SD_Write()
{
  byte finalCount = pulseCount;
  pulseCount = 0;
  dataFile = SD.open(F("datalog.csv"), FILE_WRITE);
  if(!dataFile)
  {
    Serial.print(F("\n>> Logger: Error opening file. Re-initialize the SD card.\n>> User 1: "));
    SDinit = false;
    logging = false;
  }
  else
  {
    dataFile.print(F("\""));
    dataFile.print(months);
    dataFile.print(F("/"));
    dataFile.print(days);
    dataFile.print(F("/"));
    dataFile.print(years);
    dataFile.print(F(" "));
    dataFile.print(hours);
    dataFile.print(F(":"));
    if(minutes < 10)
      dataFile.print(F("0"));
    dataFile.print(minutes);
    dataFile.print(F(":"));
    if(seconds < 10)
      dataFile.print(F("0"));
    dataFile.print(seconds);
    dataFile.print(F("\","));
    dataFile.print(recordNum);
    dataFile.print(',');
    dataFile.print(finalCount);
    dataFile.print('\n');
    recordNum += 1;
    dataFile.close();
  }
}

void INT0_ISR()                 // Sensor ISR. Increment pulse count.
{
  pulseCount += 1;
}

void INT1_ISR()                 // Serial On Button ISR. Activate Serial Interface
{
  power_usart0_enable();
  serialOn = true;
  Serial.begin(9600);
  Serial.print(F("\n>> User 1: "));
  EIMSK &= ~(1 << INT1);
}

ISR(WDT_vect)          // WatchDog Timer ISR sets "4-seconds are up" flag
{
  timesUp = true;
}

bool isLeapYear(short year)
{
  return (((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0));
}

void upDateTime()
{
    seconds += 4;
    if(seconds >= 60)
    {
      seconds = 0;
      minutes += 1;
    }
    if(minutes >= 60)
    {
      minutes = 0;
      hours += 1;
    }
    if(hours >= 24)
    {
      hours = 0;
      days += 1;
    }
    switch(days)
    {
      case 29:
        switch(months)
        {
          case 2:
            if(!isLeapYear(years))
            {
              days = 1;
              months += 1;
            }
            break;
        }
        break;
      case 30:
        switch(months)
        {
          case 2:
            days = 1;
            months += 1;
            break;
        }
        break;
      case 31:
        switch(months)
        {
          case 4:
            days = 1;
            months += 1;
            break;
          case 6:
            days = 1;
            months += 1;
            break;
          case 9:
            days = 1;
            months += 1;
            break;
          case 11:
            days = 1;
            months += 1;
            break;
        }
        break;
      case 32:
        days = 1;
        months += 1;
        break;
    }
    if(months > 12)
    {
      months = 1;
      years += 1;
    }
}

void handleSerial()             // Serial Handling function
{
  char input; 
  if(Serial.available() > 0)    // Check if serial data is available.
  {
    input = Serial.read();
    if (input != '\n')          // Ignore newline characters.
      Serial.println(input);

    switch(input)               // Switch statement for all defined inputs
    {
      case 'c':
        if(!SDinit)
        {
          Serial.print(F(">> Logger: SD card not initialized.\n>> User 1: "));
          break;
        }
        Serial.println();
        dataFile = SD.open(F("datalog.csv"), FILE_READ);
        while(dataFile.available() > 0)
        {
          Serial.write(dataFile.read());
        }
        dataFile.close();
        Serial.println();
        Serial.print(F(">> User 1: "));
        break;

      case 'C':                                         // Start the SD card fresh. Useful. Protected from deleting while in use.
        if(!SDinit)
        {
          Serial.print(F(">> Logger: SD card not initialized.\n>> User 1: "));
          break;
        }
        if(logging == false)
        {
          Serial.print(F(">> Logger: Cleaning SD Card..."));
          SD.remove("datalog.csv");
          Serial.print(F("\n>> Logger: SD Card Clean!\n>> User 1: "));
        }
        else
        {
          Serial.print(F("\n>> Logger: Cannot clean SD Card while logging.\n>> User 1: "));
        }
        break;

      case 'd':
        Serial.print(F(">> Logger: (month/day/year) "));
        Serial.print(months);
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
        Serial.print(seconds);
        Serial.print(F("\n>> User 1: "));
        break;

      case 'e':
        serialOn = false;
        Serial.print(F(">> Logger: Exit\n"));    // This will turn serial off at somepoint. Right now, it just says exit.
        Serial.print(months);
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
        Serial.print(seconds);
        EIMSK |= (1 << INT1);
        Serial.end();
        power_usart0_disable();
        break;

      case 'E':
        if(logging)
        {
          Serial.print(F(">> Logger: SD busy. Use command 'S' to stop datalogging and try again.\n>> User 1: "));
          break;
        }
        if(!SDinit)
        {
          Serial.print(F(">> Logger: No SD card to eject.\n>> User 1: "));
          break;
        }
        SDinit = false;
        Serial.print(F(">> Logger: SD card may now be removed.\n>> User 1: "));
        break;

      case 'h':                                                                 // Help list
        Serial.print(F(">> Logger: List of commands:\n"));
        Serial.print(F("           c  -- Copy data file to terminal\n"));
        Serial.print(F("           C  -- Clean SD card\n"));
        Serial.print(F("           d  -- View date/time\n"));
        Serial.print(F("           e  -- Exit serial interface\n"));
        Serial.print(F("           E  -- Eject SD card\n"));
        Serial.print(F("           h  -- Display help\n"));
        Serial.print(F("           i  -- Initialize the SD card\n"));
        Serial.print(F("           s  -- Start datalogging (will overwrite old datalog.csv)\n"));
        Serial.print(F("           S  -- Stop datalogging\n"));
        Serial.print(F("           u  -- Update date/time\n"));
        Serial.print(F(">> User 1: "));
        break;

      case 'i':
        Serial.println(F(">> Loger: Initializing SD Card..."));                     // Initialize SD card
        if(!SD.begin())
        {  
          Serial.print(F(">> Logger: Initialization Failed\n>> User 1: "));         // If initialization fails, say so.
          SDinit = false;
          break;
        }
        SDinit = true;
        Serial.print(F(">> Logger: Initialization Complete\n>> User 1: "));
        break;

      case 's':
        if(!logging)
        {
          if(!SDinit)
          {
            Serial.print(F(">> Logger: SD card not initialized.\n>> User 1: "));
            break;
          }
          logging = true;                                                       // Start logging data
          EIMSK |= (1 << INT0);         // Enable Hall Effect Sensor interrupt
          recordNum = 1;
          if(SD.exists(F("datalog.csv")))                                       // If datalog.csv does not exist on the SD card, write a header.
          {
            SD.remove("datalog.csv");
          }
          dataFile = SD.open(F("datalog.csv"), FILE_WRITE);
          dataFile.println(F("Residential Datalogger"));
          dataFile.println(F("Date,Record Number,Pulse Count"));
          dataFile.close();
          Serial.print(F(">> Logger: Datalogging Started. \n>> User 1: "));
        }
        else
        {
          Serial.print(F(">> Logger: Already logging. \n>> User 1: "));
        }
        break;

      case 'S':
        if(logging)
        {
          logging = false;                                                      // Start logging data
          EIMSK &= ~(1 << INT0);         // Disable Hall Effect Sensor interrupt
          Serial.print(F(">> Logger: Datalogging Stopped. \n>> User 1: "));
        }
        else
        {
          Serial.print(F(">> Logger: No logging to stop. \n>> User 1: "));
        }
        break;

      case 'u':
        short shortInput;
        shortInput = 0;
        bool gotIt;
        Serial.print(F(">> Logger: Year: (yyyy)\n>> User 1: "));
        years = 0;
        gotIt = false;
        while(!Serial.available());
        while(!gotIt)
        {
          input = Serial.read();
          if(47 < input && input < 58)
          {
            input = input - 48;
            shortInput = input;
            shortInput = shortInput * 1000;
            years += shortInput;
            gotIt = true;
          }
        }
        gotIt = false;
        while(!gotIt)
        {
          input = Serial.read();
          if(47 < input && input < 58)
          {
            input = input - 48;
            shortInput = input;
            shortInput = shortInput * 100;
            years += shortInput;
            gotIt = true;
          }
        }
        gotIt = false;
        while(!gotIt)
        {
          input = Serial.read();
          if(47 < input && input < 58)
          {
            input = input - 48;
            input = input * 10;
            years += input;
            gotIt = true;
          }
        }
        gotIt = false;
        while(!gotIt)
        {
          input = Serial.read();
          if(47 < input && input < 58)
          {
            input = input - 48;
            years += input;
            gotIt = true;
          }
        }
        Serial.print(years);
        Serial.print(F("\n>> Logger: Month: (mm)\n>> User 1: "));
        months = 0;
        gotIt = false;
        while(!Serial.available());
        while(!gotIt)
        {
          input = Serial.read();
          if(47 < input && input < 58)
          {
            input = input - 48;
            input = input * 10;
            months += input;
            gotIt = true;
          }
        }
        gotIt = false;
        while(!gotIt)
        {
          input = Serial.read();
          if(47 < input && input < 58)
          {
            input = input - 48;
            months += input;
            gotIt = true;
          }
        }
        Serial.print(months);
        Serial.print(F("\n>> Logger: Day: (dd)\n>> User 1: "));
        days = 0;
        gotIt = false;
        while(!Serial.available());
        while(!gotIt)
        {
          input = Serial.read();
          if(47 < input && input < 58)
          {
            input = input - 48;
            input = input * 10;
            days += input;
            gotIt = true;
          }
        }
        gotIt = false;
        while(!gotIt)
        {
          input = Serial.read();
          if(47 < input && input < 58)
          {
            input = input - 48;
            days += input;
            gotIt = true;
          }
        }
        Serial.print(days);
        Serial.print(F("\n>> Logger: Hour: (hh)\n>> User 1: "));
        hours = 0;
        gotIt = false;
        while(!Serial.available());
        while(!gotIt)
        {
          input = Serial.read();
          if(47 < input && input < 58)
          {
            input = input - 48;
            input = input * 10;
            hours += input;
            gotIt = true;
          }
        }
        gotIt = false;
        while(!gotIt)
        {
          input = Serial.read();
          if(47 < input && input < 58)
          {
            input = input - 48;
            hours += input;
            gotIt = true;
          }
        }
        Serial.print(hours);
        Serial.print(F("\n>> Logger: Minute: (mm)\n>> User 1: "));
        minutes = 0;
        gotIt = false;
        while(!Serial.available());
        while(!gotIt)
        {
          input = Serial.read();
          if(47 < input && input < 58)
          {
            input = input - 48;
            input = input * 10;
            minutes += input;
            gotIt = true;
          }
        }
        gotIt = false;
        while(!gotIt)
        {
          input = Serial.read();
          if(47 < input && input < 58)
          {
            input = input - 48;
            minutes += input;
            gotIt = true;
          }
        }
        Serial.print(minutes);
        Serial.print(F("\n>> Logger: Done. \n>> User 1: "));
        break;
        
      case 'J':
        Serial.print(F(">> Logger: J <3 J\n>> User 1: "));
        break;

      case 'L':
        Serial.print(F(">> Logger: Haha. You can never stop GNU/Linux from taking over the world.\n>> User 1: "));
        break;

      case 'M':
        Serial.print(F(">> Logger: ...\"Have you mooed today?\"...\n>> User 1: "));
        break;

      case '\n':
        break;

      default:                                  // If the command is invalid, prompt the user and introduce 'h' option.
        Serial.print(F(">> Logger: Unknown command. Use the command \"h\" for a list of commands.\n>> User 1: "));
        break;
    }
  }
}
