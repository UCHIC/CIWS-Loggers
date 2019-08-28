/*********************************************************************\
 * File:      magnetometer.h
 * Date:      04/17/2019
 * Authors:   Joshua Tracy and Daniel Henshaw
 * Hardware:  Pololu LIS3MDL 3-Axis magnetometer 
 *********************************************************************/

#ifndef MAGNETOMETER_H
#define MAGNETOMETER_H

#include <Arduino.h>
#include <Wire.h>

/*********************************************************************\
 * 
 * Library for interfacing with the Pololu LIS3MDL magnetometer.
 * 
 * Functions:       Short description:
 *    mag_init      Initialize magnetomter 
 *    read_mag      Read Data from magnetometer
 *    mag_transfer  Read/write data between magnetometer and ATmega328P
 *    
\*********************************************************************/

/*========= MACROS =========*/

/* Device Address */
#define DEV_ADDR 0x1E
//#define DEV_ADDR 0xF

/* Read/Write Flags */
#define MAG_READ  0
#define MAG_WRITE 1

/* Output Data Indices */
#define MAG_X 0
#define MAG_Y 1
#define MAG_Z 2

/* Registers */
#define WHO_AM_I 0x0F

#define CTRL_REG1 0x20
#define CTRL_REG2 0x21
#define CTRL_REG3 0x22
#define CTRL_REG4 0x23
#define CTRL_REG5 0x24

#define STATUS_REG 0x27

#define OUT_X_L 0x28
#define OUT_X_H 0x29
#define OUT_Y_L 0x2A
#define OUT_Y_H 0x2B
#define OUT_Z_L 0x2C
#define OUT_Z_H 0x2D

#define TEMP_OUT_L 0x2E
#define TEMP_OUT_H 0x2F

#define INT_CFG 0x30
#define INT_SRC 0x31

#define INT_THS_L 0x32
#define INT_THS_H 0x33

/*========= VARIABLE DECLARATIONS =========*/
extern int8_t mag_out[]; 

/*========= LIBRARY FUNCTIONS =========*/
bool mag_init();              // replaces the original magnetometerInit() function
void read_mag(int8_t* data);
void mag_transfer(int8_t* data, uint8_t reg, uint8_t numBytes, uint8_t RW);

#endif
