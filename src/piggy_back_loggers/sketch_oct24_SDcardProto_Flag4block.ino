

/* Title:      4-sec Flag code block for SD card Prototype (Residential data logger)
 *  
 * Note:       This file also contains the Sensor and RTC_out interrupt service routines 
 *  
 * Programmer: Daniel Henshaw
 * Date:       10/24/18
 * 
 * Change Log:
 * 10/24/18    File created, pseudocode started, ISR code written
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * Pseudocode: 
 * 
 * Begin
 *   Loop
 *     ...
 *     
 *     If 4-second flag is set (set means TRUE)
 *     Then
 *       Update time
 *         Add four to the number of seconds and store
 *       Construct timestamp
 *         Read day, month, year, hour, minute, second variables 
 *         Write these out concatenated into a single string
 *       Power ON the SD card
 *         Assert digital output to activate MOSFET that switches SD card power
 *         Initialize SD card by calling SD.begin() function 
 *         Set SD init flag
 *       Write the data out to the SD card
 *       Power OFF the SD card
 *         If SD card is not being accessed (FIND HOW TO CHECK THIS STATUS)
 *         Then
 *           Close SD connection
 *           Assert digital output to deactivate MOSFET that switches SD card power
 *         Else  
 *           Loop while SD card is busy
 *           EndLoop
 *           Close SD connection (might not be necessary?)
 *           Assert digital output to deactivate MOSFET that switches SD card power
 *         EndIf  
 *     EndIf  
 *   EndLoop  
 * End
 */

// Include libraries
#include <SD.h>
#include <SPI.h>

/* these next three aren't used in my section of code, but will be needed for the complete program */
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>

void setup() {
  // put your setup code here, to run once:

  // Declare variables used in interrupt service routines, which should be of "volatile" qualifier
  volatile bool flag4;          // Note: a bool occupies a single byte in memory.
  volatile byte pulseCount;
}

void loop() {
  // put your main code here, to run repeatedly:




}


/* Function Title: INT0_ISR()
 * 
 * Friendly Name:  Sensor Interrupt Service Routine (ISR)
 * 
 * Description: increments the value of the pulse count variable by one, each time this 
 *              function is called by hardware.
 */
 void INT0_ISR()
 {
    pulseCount += 1;  // increment pulseCount variable by one
 }


/* Function Title: INT1_ISR()
 * 
 * Friendly Name:  Real Time Clock OUT Interrupt Service Routine (ISR)
 * 
 * Description: sets the 4-second flag to true each time this function
 *              is called by hardware. The Real Time Clock generates
 *              the signal that calls this ISR once every four seconds.
 */
 void INT1_ISR()
 {
    flag4 = true;     // sets the "four second flag" to true
 }
