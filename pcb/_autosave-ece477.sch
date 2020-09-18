EESchema Schematic File Version 4
EELAYER 30 0
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
$Comp
L Connector:Micro_SD_Card J?
U 1 1 5F55F02A
P 8050 5000
F 0 "J?" H 8000 5717 50  0000 C CNN
F 1 "Micro_SD_Card" H 8000 5626 50  0000 C CNN
F 2 "" H 9200 5300 50  0001 C CNN
F 3 "http://katalog.we-online.de/em/datasheet/693072010801.pdf" H 8050 5000 50  0001 C CNN
	1    8050 5000
	1    0    0    -1  
$EndComp
$Comp
L RF_Module:ESP32-PICO-D4 U?
U 1 1 5F5657D7
P 4500 4050
F 0 "U?" H 4500 2361 50  0000 C CNN
F 1 "ESP32-PICO-D4" H 4500 2270 50  0000 C CNN
F 2 "Package_DFN_QFN:QFN-48-1EP_7x7mm_P0.5mm_EP5.3x5.3mm" H 4500 2350 50  0001 C CNN
F 3 "https://www.espressif.com/sites/default/files/documentation/esp32-pico-d4_datasheet_en.pdf" H 4750 3050 50  0001 C CNN
	1    4500 4050
	1    0    0    -1  
$EndComp
Wire Bus Line
	5300 3650 5800 3650
Wire Bus Line
	5800 3650 5800 5600
Wire Bus Line
	5300 3750 6150 3750
Wire Bus Line
	6150 3750 6150 5600
Wire Bus Line
	5300 3850 6500 3850
Wire Bus Line
	6500 3850 6500 5600
Wire Wire Line
	7150 4900 6150 4900
Wire Wire Line
	5450 4800 5450 4850
Wire Wire Line
	5450 4850 5300 4850
Wire Wire Line
	5450 4800 7150 4800
Wire Wire Line
	7150 5100 6500 5100
$Comp
L power:GND #PWR?
U 1 1 5F5864AA
P 7000 5200
F 0 "#PWR?" H 7000 4950 50  0001 C CNN
F 1 "GND" H 7005 5027 50  0000 C CNN
F 2 "" H 7000 5200 50  0001 C CNN
F 3 "" H 7000 5200 50  0001 C CNN
	1    7000 5200
	1    0    0    -1  
$EndComp
Wire Wire Line
	7150 5000 6950 5000
Wire Bus Line
	10150 1450 2800 1450
$Comp
L power:+3.3V #PWR?
U 1 1 5F59BC5A
P 2800 1450
F 0 "#PWR?" H 2800 1300 50  0001 C CNN
F 1 "+3.3V" H 2815 1623 50  0000 C CNN
F 2 "" H 2800 1450 50  0001 C CNN
F 3 "" H 2800 1450 50  0001 C CNN
	1    2800 1450
	1    0    0    -1  
$EndComp
Wire Wire Line
	4300 2450 4300 1450
Wire Wire Line
	7000 5200 7150 5200
Wire Wire Line
	7150 5300 7100 5300
Wire Wire Line
	7100 5300 7100 5500
Wire Wire Line
	7100 5500 5800 5500
Text Notes 7400 7500 0    50   ~ 0
Senior Design Skeleton Key
Text Notes 8150 7650 0    50   ~ 0
Sep 6, 2020
Text Notes 10650 7650 0    50   ~ 0
0.1
Text Notes 7250 7350 0    50   ~ 0
pcb/ece477.sch
$EndSCHEMATC
