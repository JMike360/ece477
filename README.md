
SKeletonKEyFinal needs to be deleted
# ece477
Purdue ECE senior design project of team 8 for Fall 2020  
  
## Team members
John Michael Bolinder (Team leader)  
Ethan Rodriguez (Systems engineer, secondary software engineer)  
Jiahui Zhu (Software engineer, secondary hardware engineer)  
Andrew Gan (Hardware engineer, secondary systems engineer)  
  
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

GPIO 6-11 - SPI flash  
GPIO 34-39 - input mode only    
[Full layout](https://microcontrollerslab.com/wp-content/uploads/2019/02/ESP32-pinout-mapping.png)  
ESP32 tech ref manual page 57-58  
  
## References
[ESP32 SPI MASTER API](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/spi_master.html)  
