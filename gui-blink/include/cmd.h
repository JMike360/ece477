#ifndef __CMD_H__
#define __CMD_H__

#include "driver/gpio.h"

#define GPIO_GREEN     (GPIO_NUM_32)
#define GPIO_RED       (GPIO_NUM_33)

#define CMD_LED_RED                     0x0
#define CMD_LED_GREEN                   0x1
#define CMD_REQUEST_ENTRIES             0x2
#define CMD_REQUEST_CREDENTIAL          0x3
#define CMD_STORE_CREDENTIAL            0x4
#define CMD_STORE_FINGERPRINT           0x5
#define CMD_DELETE_CREDENTIAL           0x6
#define CMD_DELETE_FINGERPRINT          0x7
#define CMD_MODIFY_CREDENTIAL           0x8
#define CMD_RESP_REQUEST_ENTRIES        0x9
#define CMD_RESP_REQUEST_CREDENTIAL     0xa
#define CMD_RESP_STORE_CREDENTIAL       0xb
#define CMD_RESP_DELETE_CREDENTIAL      0xc
#define CMD_RESP_MODIFY_CREDENTIAL      0xd
#define CMD_RESP_STORE_FINGERPRINT      0xe
#define CMD_RESP_DELETE_FINGERPRINT     0xf
#define CMD_ERR_INCORRECT_PACKET_FORMAT 0x10
#define CMD_ERR_UNRECOGNIZED_COMMAND    0x11

esp_err_t cmd_led_red(int status);
esp_err_t cmd_led_green(int status);
esp_err_t cmd_request_entries();
esp_err_t cmd_request_credential(char* filename);
esp_err_t cmd_store_credential(char* filename, char* url, char* username, char* pw);

#endif