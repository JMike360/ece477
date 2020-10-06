#include "driver/uart.h"

#define PIN_TX 17
#define PIN_RX 16
#define BAUD_RATE 57600
#define PORT_NUM 2
#define STACK_SIZE 2048
#define BUF_SIZE 1024

void uart_begin();