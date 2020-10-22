/* UART Echo Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <stdbool.h>
#include "../include/cmd.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_log.h"

#include "../include/spi_init.h"
#include "../include/sdcard.h"

static sdmmc_card_t* sdcard;

#define ECHO_TEST_RTS  (UART_PIN_NO_CHANGE)
#define ECHO_TEST_CTS  (UART_PIN_NO_CHANGE)

#define BUF_SIZE (1024)

#define PIN_TX 1
#define PIN_RX 3
#define BAUD_RATE 115200
#define PORT_NUM 0
#define STACK_SIZE 2048

bool _mystrcmp(uint8_t* uart_input, const char* cmd, int start, int end) {
    for(int i = start; i <= end; i++) {
        if (uart_input[i] != cmd[i - start])
            return false;
    }
    return true;
}

void _clearBuffer(uint8_t* data, int len) {
    for(int i = 0; i < len; i++)
        data[i] = '\0';
}

void gpioInit(void)
{
    gpio_pad_select_gpio(GPIO_GREEN);
    gpio_pad_select_gpio(GPIO_RED);
    gpio_set_direction(GPIO_GREEN, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_RED, GPIO_MODE_OUTPUT);
}


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
}

void readCMD()
{
    // uart_driver_install(UART_NUM, BUF_SIZE * 2, 0, 0, NULL, 0);
    // uart_param_config(UART_NUM, &uart_config);
    // uart_set_pin(UART_NUM, ECHO_TEST_TXD, ECHO_TEST_RXD, ECHO_TEST_RTS, ECHO_TEST_CTS);

    // Configure a temporary buffer for the incoming data
    uint8_t *data = (uint8_t *) malloc(BUF_SIZE);
    int buflen = 0;

    while (1) {
        // pause while no command is received
        do {
            uart_get_buffered_data_len(PORT_NUM, (size_t*)&buflen);
        } while (buflen == 0);

        // need a way to wait until command is fully sent

        _clearBuffer(data, BUF_SIZE);

        int len = uart_read_bytes(PORT_NUM, data, BUF_SIZE, 20 / portTICK_RATE_MS);
        ESP_LOGI("UART-CMD", "data: %s\n", data);
        // uart_write_bytes(UART_NUM_1, (const char *) data, len);
        switch (data[0]) {
            case CMD_LED_RED:
                cmd_led_red(data[1]);
                break;
            case CMD_LED_GREEN:
                cmd_led_green(data[1]);
                break;
            case CMD_REQUEST_ENTRIES:
                cmd_request_entries();
                break;
            case CMD_REQUEST_CREDENTIAL:
                cmd_request_credential(&data[1]);
                break;
            case CMD_STORE_CREDENTIAL:
                cmd_store_credential(sdcard, );
                break;
            default:
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void app_main(void)
{
    spi_init(SPI2_HOST);
    mount_fs(MOUNT_POINT, sdcard);
    sleep(3);

    gpioInit();
    uartInit();
    readCMD();

    esp_vfs_fat_sdcard_unmount(MOUNT_POINT, sdcard);
}
