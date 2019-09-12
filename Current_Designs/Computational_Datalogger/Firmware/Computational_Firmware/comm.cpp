#include "comm.h"

char decimalToBCD(char decVal);

void updateReport(char* report, Date_t* Date, State_t* State)
{
  if(report[0] != 0xFF)                           // If the new value for years is not 0xFF
  {
    Date->years = report[0];                            // Update the new year
    char years_BCD = decimalToBCD(Date->years);         // Convert to RTC format (BCD)
    rtcTransfer(reg_Years, WRITE, years_BCD);           // Send the new date and time to the RTC
  }
  if(report[1] != 0xFF)                           // If the new value for months is not 0xFF
  {
    Date->months = report[1];                           // Update the new month
    char months_BCD = decimalToBCD(Date->months);       // Convert to RTC format (BCD)
    rtcTransfer(reg_Months, WRITE, months_BCD);         // Send the new date and time to the RTC
  }
  if(report[2] != 0xFF)                           // If the new value for days is not 0xFF
  {
    Date->days = report[2];                             // Update the new day
    char days_BCD = decimalToBCD(Date->days);           // Convert to RTC format (BCD)
    rtcTransfer(reg_Days, WRITE, days_BCD);             // Send the new date and time to the RTC
  }
  if(report[3] != 0xFF)                           // If the new value for hours is not 0xFF
  {
    Date->hours = report[3];                            // Update the new hour
    char hours_BCD = decimalToBCD(Date->hours);         // Convert to RTC format (BCD)
    rtcTransfer(reg_Hours, WRITE, hours_BCD);           // Send the new date and time to the RTC
  }
  if(report[4] != 0xFF)                           // If the new value for minutes is not 0xFF
  {
    Date->minutes = report[4];                          // Update the new minute
    char minutes_BCD = decimalToBCD(Date->minutes);     // Convert to RTC format (BCD)
    rtcTransfer(reg_Minutes, WRITE, minutes_BCD);       // Send the new date and time to the RTC
  }
  if(report[5] != 0xFF)                           // If the new value for seconds is not 0xFF
  {
    Date->seconds = report[5];                          // Update the new second
    char seconds_BCD = decimalToBCD(Date->seconds);     // Convert to RTC format (BCD)
    rtcTransfer(reg_Seconds, WRITE, seconds_BCD);       // Send the new date and time to the RTC
  }
  if(report[8] != 0xFF)                           // If the new value for logging is not 0xFF
  {
    if((report[8] == 1) && !State->logging)       // If the new value is 1
    {
      State->logging = true;                            // Let the rest of the program know that the microcontroller is logging
      State->readMag = true;                            // Tell the program to read the magnetometer data (since the interrupt has already fired)
      State->recordNum = 1;                             // Reset record number counter
      State->romAddr = HEADER_SIZE;                     // Reset ROM Address
      writeDateAndTime(Date);                           // Store the date/time of the first data byte
      EIMSK |= (1 << INT0);                             // Enable Sensor interrupt
    }
    else if((report[8] == 0) && State->logging)   // Else
    {
      EIMSK &= ~(1 << INT0);                            // Disable Sensor interrupt
      State->logging = false;                           // Let the rest of the program know that the microcontroller is no longer logging
      State->readMag = false;                           // Let the rest of the program know that it does not need to read the magnetometer data
    }
  }
 
  return;
}

void powerRPiON()
{
  PORTC |= 0x4;     // Power up RPi by setting Analog Pin 2 HIGH (used as digital pin)
  UART_Init(UBRR);  // Initialize the UART module according to the parameters set in comm.h define statements
  spiOff();         // Set all SPI pins as inputs to avoid bus contention
  PORTB &= ~0x01;   // Enable Bus to host computer

  return;
}

void powerRPiOFF(void)
{
  UART_End();       // Disable the UART Module
  PORTC &= ~(0x4);  // Cut power to the host computer
                    // Note that SPI changes are reverted before this function gets called, when host computer releases EEPROM
  return;
}

void UART_Init(unsigned int ubrr)
{
  serialPowerUp();                        // Enable clock and power to UART Module
  UCSR0A &= ~0x02;
  
  DDRD |= 0x02;                           // Set TX pin OUTPUT (was input to avoid driving unpowered host computer RX pin)
  
  UBRR0H = (unsigned char) (ubrr >> 8);   // Load the ubrr value into high and low registers. This value sets Baud Rate (see comm.h defines)
  UBRR0L = (unsigned char) (ubrr);

  UCSR0B = (1<<RXEN0)|(1<<TXEN0);         // Enable both receiver and transmitter

  UCSR0C = 0x06;                          // Set data frame format: 8 data bits, 1 stop bit, no parity

  return;
}

void UART_Transmit(unsigned char data)
{
  UDR0 = data;  // Data to be transmitted put in UDR0 register (hardware takes care of data transmission)
  return;
}

unsigned char UART_Receive(void)
{
  return UDR0;  // New data byte received is stored by hardware in UDR0 register.
}

void UART_End()
{
  UCSR0B &= ~((1<<RXEN0)|(TXEN0));  // Disable both transmitter and receiver
  serialPowerDown();                // Disable clock and power to UART module
  DDRD &= ~(0x02);                  // Transmit pin set to input to avoid driving unpowered host computer receive pin.

  return;
}

void spiInit(void)
{
  char temp;
  spiPowerUp();   // Enable clock and power to SPI module
  DDRB = 0x2F;    // MOSI, SCK, PB1, and PB0 Outputs, MISO Input
  PORTB |= 0x02;  // EEPROM CS pin High at startup
  SPCR = 0x50;    // Enable SPI Module as Master at 2 MHz Clock
  temp = SPSR;    // Read SPI Status Register (Clears SPIF flag)
  temp = SPDR;    // Read SPI Data Register (Clears SPIF flag)
  spiPowerDown(); // Disable clock and power to SPI module

  return;
}

void spiOff(void)
{
  spiPowerUp(); // Enable clock and power to SPI module
  DDRB = 0x01;  // Set SPI pins to inputs, leave Host Computer SPI bus enable (buffer chip) output
  spiPowerDown();

  return;
}

void spiSelectSlave(void)
{
  spiPowerUp();   // Enable clock and power to SPI module
  PORTB &= ~0x02; // Set PB1 low (active-low chip select)

  return;
}

void spiReleaseSlave(void)
{
  PORTB |= 0x02;  // Set PB1 high (active-low chip select)
  spiPowerDown(); // Disable clock and power to SPI module

  return;
}

void spiTransceive(unsigned char* data, unsigned char dataSize)
{
  for(unsigned int i = 0; i < dataSize; i++)  // For all the data bytes in the data buffer passed in to this function:
  {
    SPDR = data[i];                             // Load data into the SPDR register (hardware transmits byte and receives new byte if any)
    while(!(SPSR & (1<<SPIF)));                 // Wait for the data transmission to be completed
    data[i] = SPDR;                             // Save new data byte
  }

  return;
}

char decimalToBCD(char decVal)
{
  char bcd_1 = decVal % 10;
  decVal = decVal / 10;
  char bcd_10 = (decVal % 10) << 4;
  
  return bcd_1 + bcd_10;
}
