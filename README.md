# ece477
Purdue ECE senior design project of team 8 for Fall 2020  
  
## Team members
John Michael Bolinder (Team leader)  
Ethan Rodriguez (Systems engineer, secondary software engineer)  
Jiahui Zhu (Software, secondary hardware engineer)  
Andrew Gan (Hardware, secondary systems engineer)  
  
## Tools used
USB URT module  
ESP32  
Fingerprint reader  
SD card  
SD card reader  
LCD module  
  
## GPIO table
| PIN | Function  | Endpoint   |
|-----|-----------|------------|
| 12  | HSPI_MISO | OLED 13, MicroSD Dout |
| 13  | HSPI_MOSI | OLED 14, MicroSD Din  |
| 14  | HSPI_CLK  | OLED 12, MIcroSD CLK  |
| 15  | HSPI_CS0  | OLED 16    |
| 32  | HSPI_CS1  | MicroSD CS |
  
GPIO 6-11 - SPI flash  
GPIO 34-39 - input mode only    
[Full layout](https://microcontrollerslab.com/wp-content/uploads/2019/02/ESP32-pinout-mapping.png)  
ESP32 tech ref manual page 57-58  

