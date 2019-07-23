EESchema Schematic File Version 2
LIBS:power
LIBS:device
LIBS:switches
LIBS:relays
LIBS:motors
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:micro_sd_card
LIBS:Regulator_Switching
LIBS:SD card ATmega328 prototype-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Text Notes 8150 7650 0    60   ~ 0
09/19/2018
Text Notes 10600 7650 0    60   ~ 0
A.1
Text Notes 7400 7500 0    60   ~ 0
SD Card Microcontroller Prototype (ATmega328P)
Text Notes 7300 7250 0    60   ~ 0
1 1
$Comp
L ATMEGA328P-PU U?
U 1 1 5BA2A8AC
P 5750 3650
F 0 "U?" H 5000 4900 50  0000 L BNN
F 1 "ATMEGA328P-PU" H 6150 2250 50  0000 L BNN
F 2 "DIL28" H 5750 3650 50  0001 C CIN
F 3 "" H 5750 3650 50  0001 C CNN
	1    5750 3650
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR?
U 1 1 5BA2A966
P 4750 4950
F 0 "#PWR?" H 4750 4700 50  0001 C CNN
F 1 "GND" H 4750 4800 50  0000 C CNN
F 2 "" H 4750 4950 50  0001 C CNN
F 3 "" H 4750 4950 50  0001 C CNN
	1    4750 4950
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR?
U 1 1 5BA2A98A
P 1050 2900
F 0 "#PWR?" H 1050 2650 50  0001 C CNN
F 1 "GND" H 1050 2750 50  0000 C CNN
F 2 "" H 1050 2900 50  0001 C CNN
F 3 "" H 1050 2900 50  0001 C CNN
	1    1050 2900
	1    0    0    -1  
$EndComp
$Comp
L +3.3V #PWR?
U 1 1 5BA2AE41
P 850 2400
F 0 "#PWR?" H 850 2250 50  0001 C CNN
F 1 "+3.3V" H 850 2540 50  0000 C CNN
F 2 "" H 850 2400 50  0001 C CNN
F 3 "" H 850 2400 50  0001 C CNN
	1    850  2400
	1    0    0    -1  
$EndComp
$Comp
L +3.3V #PWR?
U 1 1 5BA2AEA1
P 4700 2450
F 0 "#PWR?" H 4700 2300 50  0001 C CNN
F 1 "+3.3V" H 4700 2590 50  0000 C CNN
F 2 "" H 4700 2450 50  0001 C CNN
F 3 "" H 4700 2450 50  0001 C CNN
	1    4700 2450
	1    0    0    -1  
$EndComp
NoConn ~ 4850 3150
NoConn ~ 4850 2850
$Comp
L Crystal_Small Y?
U 1 1 5BA2AEF7
P 9250 2700
F 0 "Y?" H 9250 2800 50  0000 C CNN
F 1 "Crystal_Small" H 9250 2600 50  0000 C CNN
F 2 "" H 9250 2700 50  0001 C CNN
F 3 "" H 9250 2700 50  0001 C CNN
	1    9250 2700
	0    -1   -1   0   
$EndComp
$Comp
L C C?
U 1 1 5BA2B072
P 9700 3000
F 0 "C?" H 9725 3100 50  0000 L CNN
F 1 "10 pF ?" H 9725 2900 50  0000 L CNN
F 2 "" H 9738 2850 50  0001 C CNN
F 3 "" H 9700 3000 50  0001 C CNN
	1    9700 3000
	0    1    1    0   
$EndComp
$Comp
L C C?
U 1 1 5BA2B0BE
P 9700 2400
F 0 "C?" H 9725 2500 50  0000 L CNN
F 1 "10 pF ?" H 9725 2300 50  0000 L CNN
F 2 "" H 9738 2250 50  0001 C CNN
F 3 "" H 9700 2400 50  0001 C CNN
	1    9700 2400
	0    1    1    0   
$EndComp
$Comp
L GND #PWR?
U 1 1 5BA2B0E2
P 10100 3100
F 0 "#PWR?" H 10100 2850 50  0001 C CNN
F 1 "GND" H 10100 2950 50  0000 C CNN
F 2 "" H 10100 3100 50  0001 C CNN
F 3 "" H 10100 3100 50  0001 C CNN
	1    10100 3100
	1    0    0    -1  
$EndComp
Text Label 1700 3000 2    60   ~ 0
SCK
Text Label 6850 3050 0    60   ~ 0
SCK
Text Label 1700 3200 2    60   ~ 0
SPI_MOSI
Text Label 1700 2700 2    60   ~ 0
CD
$Comp
L SW_Push SW?
U 1 1 5BA2B5B4
P 9700 4500
F 0 "SW?" H 9750 4600 50  0000 L CNN
F 1 "SW_Push_Reset" H 9700 4440 50  0000 C CNN
F 2 "" H 9700 4700 50  0001 C CNN
F 3 "" H 9700 4700 50  0001 C CNN
	1    9700 4500
	1    0    0    -1  
$EndComp
$Comp
L LED D?
U 1 1 5BA2B8EE
P 2550 4600
F 0 "D?" H 2550 4700 50  0000 C CNN
F 1 "LED" H 2550 4500 50  0000 C CNN
F 2 "" H 2550 4600 50  0001 C CNN
F 3 "" H 2550 4600 50  0001 C CNN
	1    2550 4600
	1    0    0    -1  
$EndComp
$Comp
L R R?
U 1 1 5BA2B9A9
P 1900 4600
F 0 "R?" V 1980 4600 50  0000 C CNN
F 1 "10K" V 1900 4600 50  0000 C CNN
F 2 "" V 1830 4600 50  0001 C CNN
F 3 "" H 1900 4600 50  0001 C CNN
	1    1900 4600
	0    1    1    0   
$EndComp
Text Label 6850 2750 0    60   ~ 0
~SS
$Comp
L Screw_Terminal_01x03 J?
U 1 1 5BA2BDE6
P 9850 5600
F 0 "J?" H 9850 5800 50  0000 C CNN
F 1 "Screw_Terminal_01x03" H 9850 5400 50  0000 C CNN
F 2 "" H 9850 5600 50  0001 C CNN
F 3 "" H 9850 5600 50  0001 C CNN
	1    9850 5600
	1    0    0    -1  
$EndComp
$Comp
L R R?
U 1 1 5BA2BE31
P 8800 5650
F 0 "R?" V 8880 5650 50  0000 C CNN
F 1 "10K" V 8800 5650 50  0000 C CNN
F 2 "" V 8730 5650 50  0001 C CNN
F 3 "" H 8800 5650 50  0001 C CNN
	1    8800 5650
	1    0    0    -1  
$EndComp
Text Label 6850 2950 0    60   ~ 0
SPI_MISO
Text Label 6850 2850 0    60   ~ 0
SPI_MOSI
Text Label 1700 2800 2    60   ~ 0
SPI_MISO
$Comp
L R R?
U 1 1 5BA2C485
P 9250 4300
F 0 "R?" V 9330 4300 50  0000 C CNN
F 1 "10K" V 9250 4300 50  0000 C CNN
F 2 "" V 9180 4300 50  0001 C CNN
F 3 "" H 9250 4300 50  0001 C CNN
	1    9250 4300
	-1   0    0    1   
$EndComp
$Comp
L +3.3V #PWR?
U 1 1 5BA2C543
P 9250 4150
F 0 "#PWR?" H 9250 4000 50  0001 C CNN
F 1 "+3.3V" H 9250 4290 50  0000 C CNN
F 2 "" H 9250 4150 50  0001 C CNN
F 3 "" H 9250 4150 50  0001 C CNN
	1    9250 4150
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR?
U 1 1 5BA2C6B7
P 10100 4550
F 0 "#PWR?" H 10100 4300 50  0001 C CNN
F 1 "GND" H 10100 4400 50  0000 C CNN
F 2 "" H 10100 4550 50  0001 C CNN
F 3 "" H 10100 4550 50  0001 C CNN
	1    10100 4550
	1    0    0    -1  
$EndComp
NoConn ~ 6750 4850
NoConn ~ 6750 4750
NoConn ~ 6750 4650
NoConn ~ 6750 4550
NoConn ~ 6750 4450
NoConn ~ 6750 4250
NoConn ~ 6750 4150
NoConn ~ 6750 3900
NoConn ~ 6750 3800
NoConn ~ 6750 3700
NoConn ~ 6750 3600
NoConn ~ 6750 3500
NoConn ~ 6750 3400
NoConn ~ 6750 2550
NoConn ~ 6750 2650
Text Notes 8750 5250 0    60   ~ 0
Hall Effect Sensor Connector
Text Notes 8800 2100 0    60   ~ 0
External Oscillator Block
Text Notes 8950 3900 0    60   ~ 0
Reset Button Block\n(Active Low)
Text Label 1550 4600 2    60   ~ 0
CD
$Comp
L +3.3V #PWR?
U 1 1 5BA2D351
P 3050 4600
F 0 "#PWR?" H 3050 4450 50  0001 C CNN
F 1 "+3.3V" H 3050 4740 50  0000 C CNN
F 2 "" H 3050 4600 50  0001 C CNN
F 3 "" H 3050 4600 50  0001 C CNN
	1    3050 4600
	1    0    0    -1  
$EndComp
Text Notes 800  4400 0    60   ~ 0
The Card Missing LED is ON when CD (Card Detect) is LOW
Text Notes 2200 4800 0    60   ~ 0
Card Missing LED
Text Notes 5300 2100 0    60   ~ 0
Arduino Pro Mini 3.3V
Text Notes 1800 2200 0    60   ~ 0
Sparkfun microSD TransFlash Breakout\nBOB-00544
Text Notes 8250 7750 0    60   ~ 0
Drawn By: Daniel Henshaw and
Text Label 1700 3300 2    60   ~ 0
DigitalOutCS
Text Label 1050 3300 2    60   ~ 0
~SS
$Comp
L Sparkfun_Micro_SD_Card_Breakout J?
U 1 1 5BA3F69E
P 2600 3000
F 0 "J?" H 1900 3600 50  0000 C CNN
F 1 "Sparkfun_Micro_SD_Card_Breakout" H 3300 3600 50  0000 R CNN
F 2 "" H 3750 3300 50  0001 C CNN
F 3 "" H 2600 3000 50  0001 C CNN
	1    2600 3000
	1    0    0    -1  
$EndComp
$Comp
L +3.3V #PWR?
U 1 1 5BA40172
P 9100 5500
F 0 "#PWR?" H 9100 5350 50  0001 C CNN
F 1 "+3.3V" H 9100 5640 50  0000 C CNN
F 2 "" H 9100 5500 50  0001 C CNN
F 3 "" H 9100 5500 50  0001 C CNN
	1    9100 5500
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR?
U 1 1 5BA401A0
P 9100 5600
F 0 "#PWR?" H 9100 5350 50  0001 C CNN
F 1 "GND" H 9100 5450 50  0000 C CNN
F 2 "" H 9100 5600 50  0001 C CNN
F 3 "" H 9100 5600 50  0001 C CNN
	1    9100 5600
	1    0    0    -1  
$EndComp
Text Label 8600 6050 2    60   ~ 0
SensorOutput
Wire Wire Line
	4850 4850 4750 4850
Wire Wire Line
	4750 4750 4750 4950
Wire Wire Line
	4850 4750 4750 4750
Connection ~ 4750 4850
Wire Wire Line
	4700 2450 4700 2550
Wire Wire Line
	4700 2550 4850 2550
Wire Wire Line
	8750 2400 9550 2400
Wire Wire Line
	8750 3000 9550 3000
Connection ~ 9250 2400
Connection ~ 9250 3000
Wire Wire Line
	9850 2400 10100 2400
Wire Wire Line
	10100 2400 10100 3100
Wire Wire Line
	10100 3000 9850 3000
Connection ~ 10100 3000
Wire Wire Line
	2050 4600 2400 4600
Wire Wire Line
	1750 4600 1550 4600
Wire Wire Line
	9250 4450 9250 4500
Connection ~ 9250 4500
Wire Wire Line
	10100 4550 10100 4500
Wire Wire Line
	10100 4500 9900 4500
Wire Wire Line
	2700 4600 3050 4600
Wire Wire Line
	1050 2900 1700 2900
Wire Wire Line
	850  2400 850  3100
Wire Wire Line
	850  3100 1700 3100
Wire Wire Line
	8800 5500 9650 5500
Wire Wire Line
	8800 5800 9350 5800
Wire Wire Line
	9350 5700 9350 6050
Wire Wire Line
	9350 5700 9650 5700
Wire Wire Line
	9100 5600 9650 5600
Wire Wire Line
	9350 6050 8600 6050
Connection ~ 9350 5800
Text Notes 9950 5700 0    60   ~ 0
to red\nto black\nto green
Text Label 6850 4350 0    60   ~ 0
SensorOutput
Text Label 6850 4000 0    60   ~ 0
~Reset
Wire Wire Line
	8600 4500 9500 4500
Text Label 8600 4500 2    60   ~ 0
~Reset
Wire Wire Line
	6750 4000 6850 4000
Wire Wire Line
	6750 4350 6850 4350
Wire Wire Line
	6750 2750 6850 2750
Wire Wire Line
	6750 2850 6850 2850
Wire Wire Line
	6750 2950 6850 2950
Wire Wire Line
	6750 3050 6850 3050
Wire Wire Line
	9250 2400 9250 2600
Wire Wire Line
	9250 2800 9250 3000
Text Label 6850 3150 0    60   ~ 0
CRYSTAL1
Text Label 8750 2400 2    60   ~ 0
CRYSTAL1
Text Label 8750 3000 2    60   ~ 0
CRYSTAL2
Wire Wire Line
	6850 3150 6750 3150
Wire Wire Line
	6850 3250 6750 3250
Text Label 6850 3250 0    60   ~ 0
CRYSTAL2
Text Notes 4700 6650 0    60   ~ 0
Connection Legend for Arduino UNO R3 Digital Pins\n\nUNO pin \nname     Used for\n-----  ---------    \n13        SCK\n12        DO  SPI MISO\n11        DI   SPI MOSI\n10        CS\n\n2          Sensor Output
Text Notes 8850 3150 0    60   ~ 0
8 MHz crystal
Text Notes 1200 1250 0    60   ~ 0
To do: figure out where to connect \nDigitalOutCS and ~SS\n(are they the same thing?)
Wire Wire Line
	1050 3300 1700 3300
Text Notes 1200 6500 0    60   ~ 0
Voltage Regulator Block
$Comp
L TSR_1-2433 U?
U 1 1 5BA5505E
P 2100 7000
F 0 "U?" H 1800 7250 50  0000 L CNN
F 1 "TSR_1-2433" H 2050 7250 50  0000 L CNN
F 2 "Converter_DCDC:Converter_DCDC_TRACO_TSR-1_THT" H 2100 6850 50  0001 L CIN
F 3 "" H 2100 7000 50  0001 C CNN
	1    2100 7000
	1    0    0    -1  
$EndComp
Text Notes 850  7750 0    60   ~ 0
NOTE: TRACO 3.3V @ 1A MAX. SWITCHING REGULATOR
$Comp
L +3.3V #PWR?
U 1 1 5BA55106
P 2750 6750
F 0 "#PWR?" H 2750 6600 50  0001 C CNN
F 1 "+3.3V" H 2750 6890 50  0000 C CNN
F 2 "" H 2750 6750 50  0001 C CNN
F 3 "" H 2750 6750 50  0001 C CNN
	1    2750 6750
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR?
U 1 1 5BA55136
P 2100 7450
F 0 "#PWR?" H 2100 7200 50  0001 C CNN
F 1 "GND" H 2100 7300 50  0000 C CNN
F 2 "" H 2100 7450 50  0001 C CNN
F 3 "" H 2100 7450 50  0001 C CNN
	1    2100 7450
	1    0    0    -1  
$EndComp
$Comp
L +12V #PWR?
U 1 1 5BA55174
P 850 6750
F 0 "#PWR?" H 850 6600 50  0001 C CNN
F 1 "+12V" H 850 6890 50  0000 C CNN
F 2 "" H 850 6750 50  0001 C CNN
F 3 "" H 850 6750 50  0001 C CNN
	1    850  6750
	1    0    0    -1  
$EndComp
Wire Wire Line
	850  6750 850  6900
$Comp
L C C?
U 1 1 5BA553B6
P 1450 7150
F 0 "C?" H 1475 7250 50  0000 L CNN
F 1 "10uF  25V" H 1475 7050 50  0000 L CNN
F 2 "" H 1488 7000 50  0001 C CNN
F 3 "" H 1450 7150 50  0001 C CNN
	1    1450 7150
	1    0    0    -1  
$EndComp
Wire Wire Line
	2100 7200 2100 7450
Connection ~ 2100 7350
Connection ~ 850  6900
$Comp
L C C?
U 1 1 5BA557B7
P 2750 7150
F 0 "C?" H 2775 7250 50  0000 L CNN
F 1 "10uF  25V" H 2775 7050 50  0000 L CNN
F 2 "" H 2788 7000 50  0001 C CNN
F 3 "" H 2750 7150 50  0001 C CNN
	1    2750 7150
	1    0    0    -1  
$EndComp
Wire Wire Line
	2750 7350 2750 7300
Connection ~ 2750 6900
$Comp
L Conn_02x03_Odd_Even J?
U 1 1 5BA55D22
P 5350 7300
F 0 "J?" H 5400 7500 50  0000 C CNN
F 1 "Conn_02x03_Odd_Even" H 5400 7100 50  0000 C CNN
F 2 "" H 5350 7300 50  0001 C CNN
F 3 "" H 5350 7300 50  0001 C CNN
	1    5350 7300
	1    0    0    -1  
$EndComp
Text Notes 5150 7050 0    60   ~ 0
ISP Header
$Comp
L D_Schottky D?
U 1 1 5BA55F75
P 1150 6900
F 0 "D?" H 1150 7000 50  0000 C CNN
F 1 "D_Schottky" H 1150 6800 50  0000 C CNN
F 2 "" H 1150 6900 50  0001 C CNN
F 3 "" H 1150 6900 50  0001 C CNN
	1    1150 6900
	-1   0    0    1   
$EndComp
Wire Wire Line
	2750 6750 2750 7000
Wire Wire Line
	850  6900 1000 6900
Wire Wire Line
	1300 6900 1700 6900
Wire Wire Line
	2500 6900 2750 6900
Wire Wire Line
	1450 7300 1450 7350
Connection ~ 1450 7350
Wire Wire Line
	1450 7350 2750 7350
Wire Wire Line
	1450 6900 1450 7000
Connection ~ 1450 6900
$EndSCHEMATC
