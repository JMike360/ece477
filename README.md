
# ece477
Purdue ECE senior design project of team 8 for Fall 2020  
  
## Team members
John Michael Bolinder (Team leader, software engineer, systems engineer)  
Ethan Rodriguez (Systems engineer, secondary software engineer)  
Jiahui Zhu (Software engineer, secondary hardware engineer)  
Andrew Gan (Hardware engineer, secondary systems engineer)  

## Overview
Skeleton Key is a convenient, fully-featured password management solution, completely client-side with no dependence on a cloud service provider or network connection. The device utilizes an ESP32 microcontroller and onboard peripherals to manage encrypted-at-rest password storage, fingerprint-based authentication for easy and secure access to passwords, and interfaces with a client desktop app to wirelessly retrieve passwords via bluetooth or retrieve passwords through a wired USB connection. Passwords transmitted by USB or Bluetooth are encrypted in-flight, and the desktop application provides easy access to passwords on the target user device.
  
## Tools used
USB URT module  
ESP32  
Fingerprint reader  
SD card  
SD card reader  
LCD module  
  
## GPIO table
| GPIO PIN      | Function  | Endpoint     |
|---------------|-----------|--------------|
| 1             | U0TXD     | FT232 RXD    |
| 3             | U0RXD     | FT232 TXD    |
| 9             | U1RXD     | Console in   |
| 10            | U1TXD     | Console_out  |
| 12            | HSPI_MISO | MicroSD Dout |
| 13            | HSPI_MOSI | MicroSD Din  |
| 14            | HSPI_CLK  | MicroSD CLK  |
| 15            | HSPI_CS0  | MicroSD CS   |
| 16            | U2RXD     | Fingerprint Scanner TX |
| 17            | U2TXD     | Fingerprint Scanner RX |
  
UART 0: console/debug  
UART 1: ESP32 - GUI  
UART 2: ESP32 - Fingerprint  
  
GPIO 6-11 - SPI flash  
GPIO 34-39 - input mode only    
[Full layout](https://microcontrollerslab.com/wp-content/uploads/2019/02/ESP32-pinout-mapping.png)  
ESP32 tech ref manual page 57-58  
  
## References
[ESP32 SPI MASTER API](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/spi_master.html)  
