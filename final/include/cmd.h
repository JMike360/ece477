#ifndef __CMD_H__
#define __CMD_H__

#include "driver/gpio.h"
#include "driver/uart.h"

#define CMD_SUCCESS 1
#define CMD_FAILURE 0

#define GPIO_GREEN     (GPIO_NUM_32)
#define GPIO_RED       (GPIO_NUM_33)

#define BT_MODE     0
#define UART_MODE   1

#define CMD_LED_RED                     0x0
#define CMD_LED_GREEN                   0x1
#define CMD_REQUEST_ENTRIES             0x2
#define CMD_REQUEST_CREDENTIAL          0x3
#define CMD_STORE_CREDENTIAL            0x4
#define CMD_STORE_FINGERPRINT           0x5
#define CMD_DELETE_CREDENTIAL           0x6
#define CMD_DELETE_FINGERPRINT          0x7
#define CMD_MODIFY_CREDENTIAL           0x8
#define CMD_POWER_OFF                   0x9
#define CMD_QUERY_COMM_MODE             0xa
#define CMD_RSA_KEY_EXCHANGE            0xb
#define CMD_BT_DISCONNECT               0xc

void doCMD(uint8_t* data, int mode);
int getRunning();
int cmd_led_red(int status);
int cmd_led_green(int status);
int cmd_request_entries(int mode);
int cmd_request_credential(char* displayname, char* userName, int mode);
int cmd_store_credential(char* displayName, char* username, char* url, char* pw);
int cmd_modify_credential(char* displayName, char* username, char* pw);
int cmd_delete_credential(char* displayName, char* userName);
int cmd_store_fingerprint();
int cmd_delete_fingerprint();

#endif