#ifndef _UART_SETUP

#include "driver/uart.h"
#include <stdlib.h>

#define PIN_TX_0 1
#define PIN_RX_0 3
#define PIN_TX_1 10
#define PIN_RX_1 9
#define PIN_TX_2 17
#define PIN_RX_2 16

#define BAUD_RATE 57600
#define BAUD_RATE_IO 115200
#define STACK_SIZE 2048
#define BUF_SIZE 1024


void uart_begin(int port);
void uart_end(int port);

#define _UART_SETUP
#endif