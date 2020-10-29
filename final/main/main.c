/* UART Echo Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "../include/cmd.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include "esp_vfs_fat.h"
#include "esp_log.h"
#include <unistd.h>

#include "../include/sdcard.h"
#include "../include/manifest.h"

#define ECHO_TEST_RTS  (UART_PIN_NO_CHANGE)
#define ECHO_TEST_CTS  (UART_PIN_NO_CHANGE)

#define BUF_SIZE (1024)
#define TAG "UART-CMD"

#define PIN_TX 1
#define PIN_RX 3
#define BAUD_RATE 115200
#define STACK_SIZE 2048

/**************************************************
 * _clearDataBuffer
 * Set all values in data buffer to null char
 * 
 * input:
 * uint8_t* data - buffer to be cleared
 * int len - length of buffer
 * 
 * output:
 * void
**************************************************/
void _clearDataBuffer(uint8_t* data, int len) {
    for(int i = 0; i < len; i++)
        data[i] = '\0';
}

/**************************************************
 * gpioInit
 * Initialize GPIO pin for LEDs using GPIO_GREEN and
 * GPIO_RED and set them to output mode
 * 
 * input:
 * void
 * 
 * output:
 * void
**************************************************/
void gpioInit(void) {
    gpio_pad_select_gpio(GPIO_GREEN);
    gpio_pad_select_gpio(GPIO_RED);
    gpio_set_direction(GPIO_GREEN, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_RED, GPIO_MODE_OUTPUT);
    ESP_LOGI("GPIO", "Successfullly initialized GPIO");
}

/**************************************************
 * uartInit
 * Initialize UART port 0 (embedded USB port) pins
 * and install driver for UART.
 * 
 * input:
 * void
 * 
 * output:
 * void
**************************************************/
void uartInit () {
    uart_config_t config = {
        .baud_rate = BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB
    };
    int intr_alloc_flags = 0;
    ESP_ERROR_CHECK(uart_driver_install(PORT_NUM, BUF_SIZE*2, 0, 0, NULL, intr_alloc_flags));
    ESP_ERROR_CHECK(uart_param_config(PORT_NUM, &config));
    ESP_ERROR_CHECK(uart_set_pin(PORT_NUM, PIN_TX, PIN_RX, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    ESP_LOGI("UART", "Successfullly initialized UART");
}

/**************************************************
 * _clearDataBuffer
 * Read UART messages and call the corresponding
 * command functions. This function stays in an
 * infinite loop until the terminate command is given.
 * 
 * input:
 * void
 * 
 * output:
 * void
**************************************************/
void readCMD() {
    // Configure a temporary buffer for the incoming data
    uint8_t *data = (uint8_t *) malloc(BUF_SIZE);
    char *displayName, *username, *url, *pw;

    // while (1) {
        int i = 0;
        int returnStatus = 0;

        do {
            i += uart_read_bytes(PORT_NUM, &data[i], BUF_SIZE, 20 / portTICK_RATE_MS);
            ESP_LOGI(TAG, "len = %d, str = %s\n", i, data);
        } while(data[i-1] != '\n');
        ESP_LOGI(TAG, "reading from UART: %s\n", data);

        // uart_write_bytes(UART_NUM_1, (const char *) data, len);
        switch (data[1]) {
            case CMD_LED_RED:
                returnStatus = cmd_led_red(data[3]);
                break;
            case CMD_LED_GREEN:
                returnStatus = cmd_led_green(data[3]);
                break;
            case CMD_REQUEST_ENTRIES:
                returnStatus = cmd_request_entries();
                break;
            case CMD_REQUEST_CREDENTIAL:
                displayName = strtok((char*) &data[3], ",");
                username = strtok(NULL, ",");
                returnStatus = cmd_request_credential(displayName, username);
                break;
            case CMD_STORE_CREDENTIAL:
                displayName = strtok((char*) &data[3], ",");
                username = strtok(NULL, ",");
                url = strtok(NULL, ",");
                pw = strtok(NULL, ",");
                returnStatus = cmd_store_credential(displayName, username, url, pw);
                break;
        }
    // }

    free(data);
}

/**************************************************
 * app_main
 * Initialize SPI, GPIO, SD card, manifest and prepare
 * device for listening to incoming UART commands.
 * 
 * input:
 * void
 * 
 * output:
 * void
**************************************************/
void app_main(void) {
    spiInit();
    mountSD();
    sleep(2);
    gpioInit();
    uartInit();
    sleep(1);

    if (readManifestToMemory() == MANIFEST_FAILURE)
        return;

    // readCMD();

    if (writeManifestToFile() == MANIFEST_FAILURE)
        return;
    if (deallocateManifest() == MANIFEST_FAILURE)
        return;

    unmountSD();
}
