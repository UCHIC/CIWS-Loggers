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
A
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
L Micro_SD_Card_Det J?
U 1 1 5BA2A8E7
P 2600 3100
F 0 "J?" H 1950 3800 50  0000 C CNN
F 1 "Micro_SD_Card_Det" H 3250 3800 50  0000 R CNN
F 2 "" H 4650 3800 50  0001 C CNN
F 3 "" H 2600 3200 50  0001 C CNN
	1    2600 3100
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
P 1150 3200
F 0 "#PWR?" H 1150 2950 50  0001 C CNN
F 1 "GND" H 1150 3050 50  0000 C CNN
F 2 "" H 1150 3200 50  0001 C CNN
F 3 "" H 1150 3200 50  0001 C CNN
	1    1150 3200
	1    0    0    -1  
$EndComp
$Comp
L +3.3V #PWR?
U 1 1 5BA2AE41
P 1150 2350
F 0 "#PWR?" H 1150 2200 50  0001 C CNN
F 1 "+3.3V" H 1150 2490 50  0000 C CNN
F 2 "" H 1150 2350 50  0001 C CNN
F 3 "" H 1150 2350 50  0001 C CNN
	1    1150 2350
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
P 7700 3200
F 0 "Y?" H 7700 3300 50  0000 C CNN
F 1 "Crystal_Small" H 7700 3100 50  0000 C CNN
F 2 "" H 7700 3200 50  0001 C CNN
F 3 "" H 7700 3200 50  0001 C CNN
	1    7700 3200
	0    -1   -1   0   
$EndComp
$Comp
L C C?
U 1 1 5BA2B072
P 8150 3350
F 0 "C?" H 8175 3450 50  0000 L CNN
F 1 "10 pF ?" H 8175 3250 50  0000 L CNN
F 2 "" H 8188 3200 50  0001 C CNN
F 3 "" H 8150 3350 50  0001 C CNN
	1    8150 3350
	0    1    1    0   
$EndComp
$Comp
L C C?
U 1 1 5BA2B0BE
P 8150 3050
F 0 "C?" H 8175 3150 50  0000 L CNN
F 1 "10 pF ?" H 8175 2950 50  0000 L CNN
F 2 "" H 8188 2900 50  0001 C CNN
F 3 "" H 8150 3050 50  0001 C CNN
	1    8150 3050
	0    1    1    0   
$EndComp
$Comp
L GND #PWR?
U 1 1 5BA2B0E2
P 8500 3350
F 0 "#PWR?" H 8500 3100 50  0001 C CNN
F 1 "GND" H 8500 3200 50  0000 C CNN
F 2 "" H 8500 3350 50  0001 C CNN
F 3 "" H 8500 3350 50  0001 C CNN
	1    8500 3350
	1    0    0    -1  
$EndComp
Text Label 1700 3100 2    60   ~ 0
SCK
Text Label 6750 3050 0    60   ~ 0
SCK
Text Label 1300 2850 0    60   ~ 0
MOSI
NoConn ~ 1700 3600
NoConn ~ 1700 3500
Text Label 1700 2800 2    60   ~ 0
CD
$Comp
L GND #PWR?
U 1 1 5BA2B462
P 3450 3700
F 0 "#PWR?" H 3450 3450 50  0001 C CNN
F 1 "GND" H 3450 3550 50  0000 C CNN
F 2 "" H 3450 3700 50  0001 C CNN
F 3 "" H 3450 3700 50  0001 C CNN
	1    3450 3700
	1    0    0    -1  
$EndComp
$Comp
L SW_Push SW?
U 1 1 5BA2B5B4
P 7750 4000
F 0 "SW?" H 7800 4100 50  0000 L CNN
F 1 "SW_Push_Reset" H 7750 3940 50  0000 C CNN
F 2 "" H 7750 4200 50  0001 C CNN
F 3 "" H 7750 4200 50  0001 C CNN
	1    7750 4000
	1    0    0    -1  
$EndComp
$Comp
L LED D?
U 1 1 5BA2B8EE
P 10200 4250
F 0 "D?" H 10200 4350 50  0000 C CNN
F 1 "LED" H 10200 4150 50  0000 C CNN
F 2 "" H 10200 4250 50  0001 C CNN
F 3 "" H 10200 4250 50  0001 C CNN
	1    10200 4250
	1    0    0    -1  
$EndComp
$Comp
L LED D?
U 1 1 5BA2B971
P 9550 5000
F 0 "D?" H 9550 5100 50  0000 C CNN
F 1 "LED" H 9550 4900 50  0000 C CNN
F 2 "" H 9550 5000 50  0001 C CNN
F 3 "" H 9550 5000 50  0001 C CNN
	1    9550 5000
	1    0    0    -1  
$EndComp
$Comp
L R R?
U 1 1 5BA2B9A9
P 9550 4250
F 0 "R?" V 9630 4250 50  0000 C CNN
F 1 "10K" V 9550 4250 50  0000 C CNN
F 2 "" V 9480 4250 50  0001 C CNN
F 3 "" H 9550 4250 50  0001 C CNN
	1    9550 4250
	0    1    1    0   
$EndComp
$Comp
L R R?
U 1 1 5BA2BA2C
P 10200 5000
F 0 "R?" V 10280 5000 50  0000 C CNN
F 1 "10K" V 10200 5000 50  0000 C CNN
F 2 "" V 10130 5000 50  0001 C CNN
F 3 "" H 10200 5000 50  0001 C CNN
	1    10200 5000
	0    1    1    0   
$EndComp
Text Label 6750 2750 0    60   ~ 0
~SS
$Comp
L Screw_Terminal_01x03 J?
U 1 1 5BA2BDE6
P 8500 1350
F 0 "J?" H 8500 1550 50  0000 C CNN
F 1 "Screw_Terminal_01x03" H 8500 1150 50  0000 C CNN
F 2 "" H 8500 1350 50  0001 C CNN
F 3 "" H 8500 1350 50  0001 C CNN
	1    8500 1350
	1    0    0    -1  
$EndComp
$Comp
L R R?
U 1 1 5BA2BE31
P 9300 1300
F 0 "R?" V 9380 1300 50  0000 C CNN
F 1 "R" V 9300 1300 50  0000 C CNN
F 2 "" V 9230 1300 50  0001 C CNN
F 3 "" H 9300 1300 50  0001 C CNN
	1    9300 1300
	1    0    0    -1  
$EndComp
Text Label 6750 2950 0    60   ~ 0
MISO
Text Label 6750 2850 0    60   ~ 0
MOSI
Text Label 1300 2650 0    60   ~ 0
MISO
$Comp
L GND #PWR?
U 1 1 5BA2C242
P 9200 5150
F 0 "#PWR?" H 9200 4900 50  0001 C CNN
F 1 "GND" H 9200 5000 50  0000 C CNN
F 2 "" H 9200 5150 50  0001 C CNN
F 3 "" H 9200 5150 50  0001 C CNN
	1    9200 5150
	1    0    0    -1  
$EndComp
Text Label 10350 5000 0    60   ~ 0
LED2
$Comp
L R R?
U 1 1 5BA2C485
P 7400 3800
F 0 "R?" V 7480 3800 50  0000 C CNN
F 1 "10K" V 7400 3800 50  0000 C CNN
F 2 "" V 7330 3800 50  0001 C CNN
F 3 "" H 7400 3800 50  0001 C CNN
	1    7400 3800
	-1   0    0    1   
$EndComp
$Comp
L +3.3V #PWR?
U 1 1 5BA2C543
P 7400 3650
F 0 "#PWR?" H 7400 3500 50  0001 C CNN
F 1 "+3.3V" H 7400 3790 50  0000 C CNN
F 2 "" H 7400 3650 50  0001 C CNN
F 3 "" H 7400 3650 50  0001 C CNN
	1    7400 3650
	1    0    0    -1  
$EndComp
Wire Wire Line
	1150 3200 1700 3200
Wire Wire Line
	4850 4850 4750 4850
Wire Wire Line
	4750 4750 4750 4950
Wire Wire Line
	1150 2350 1150 3000
Wire Wire Line
	1150 3000 1700 3000
Wire Wire Line
	4850 4750 4750 4750
Connection ~ 4750 4850
Wire Wire Line
	4700 2450 4700 2550
Wire Wire Line
	4700 2550 4850 2550
Wire Wire Line
	6750 3150 7500 3150
Wire Wire Line
	7500 3150 7500 3100
Wire Wire Line
	7500 3100 7900 3100
Wire Wire Line
	6750 3250 7500 3250
Wire Wire Line
	7500 3250 7500 3300
Wire Wire Line
	7500 3300 7900 3300
Wire Wire Line
	7900 3100 7900 3050
Wire Wire Line
	7900 3050 8000 3050
Connection ~ 7700 3100
Wire Wire Line
	7900 3300 7900 3350
Wire Wire Line
	7900 3350 8000 3350
Connection ~ 7700 3300
Wire Wire Line
	8300 3050 8400 3050
Wire Wire Line
	8400 3050 8400 3350
Wire Wire Line
	8300 3350 8500 3350
Connection ~ 8400 3350
Wire Wire Line
	3400 3600 3450 3600
Wire Wire Line
	3450 3600 3450 3700
Wire Wire Line
	9700 4250 10050 4250
Wire Wire Line
	9700 5000 10050 5000
Wire Wire Line
	9400 4250 9200 4250
Wire Wire Line
	9400 5000 9200 5000
Wire Wire Line
	6750 4000 7550 4000
Wire Wire Line
	7400 3950 7400 4000
Connection ~ 7400 4000
$Comp
L GND #PWR?
U 1 1 5BA2C6B7
P 8150 4050
F 0 "#PWR?" H 8150 3800 50  0001 C CNN
F 1 "GND" H 8150 3900 50  0000 C CNN
F 2 "" H 8150 4050 50  0001 C CNN
F 3 "" H 8150 4050 50  0001 C CNN
	1    8150 4050
	1    0    0    -1  
$EndComp
Wire Wire Line
	8150 4050 8150 4000
Wire Wire Line
	8150 4000 7950 4000
NoConn ~ 6750 4850
NoConn ~ 6750 4750
NoConn ~ 6750 4650
NoConn ~ 6750 4550
NoConn ~ 6750 4450
NoConn ~ 6750 4350
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
Text Notes 8900 5550 0    60   ~ 0
To Do: connect LED2 to what drives it.
Text Notes 7850 1000 0    60   ~ 0
Hall Effect Sensor Connector
Text Notes 7450 2850 0    60   ~ 0
External Oscillator Block
Text Notes 7350 4450 0    60   ~ 0
Reset Button Block\n(Active Low)
Text Notes 1350 2100 0    60   ~ 0
To do: figure out where to connect MISO MOSI and ~SS
Wire Wire Line
	10350 4250 10700 4250
Text Label 9200 4250 2    60   ~ 0
CD
$Comp
L +3.3V #PWR?
U 1 1 5BA2D351
P 10700 4250
F 0 "#PWR?" H 10700 4100 50  0001 C CNN
F 1 "+3.3V" H 10700 4390 50  0000 C CNN
F 2 "" H 10700 4250 50  0001 C CNN
F 3 "" H 10700 4250 50  0001 C CNN
	1    10700 4250
	1    0    0    -1  
$EndComp
Wire Wire Line
	9200 5000 9200 5150
Text Notes 8450 4050 0    60   ~ 0
The Card Missing LED is ON when CD (Card Detect) is LOW
Text Notes 9850 4450 0    60   ~ 0
Card Missing LED
Text Notes 4900 2100 0    60   ~ 0
Arduino UNO R3 or Arduino Pro Mini 3.3V
Text Notes 1850 1850 0    60   ~ 0
Sparkfun microSD TransFlash Breakout\nBOB-00544
Text Notes 1250 4050 0    60   ~ 0
To do: look for or make a custom symbol \nthat matches the Sparkfun breakout board better
Text Notes 8250 7750 0    60   ~ 0
Drawn By: Daniel Henshaw and
$EndSCHEMATC
