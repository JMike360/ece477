/* UART Echo Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_log.h"

#include "../../hspi-bus/main/include/spi_init.h"
#include "../../hspi-bus/main/include/sdcard.h"

/**
 * This is an example which echos any data it receives on UART1 back to the sender,
 * with hardware flow control turned off. It does not use UART driver event queue.
 *
 * - Port: UART1
 * - Receive (Rx) buffer: on
 * - Transmit (Tx) buffer: off
 * - Flow control: off
 * - Event queue: off
 * - Pin assignment: see defines below
 */

#define ECHO_TEST_RTS  (UART_PIN_NO_CHANGE)
#define ECHO_TEST_CTS  (UART_PIN_NO_CHANGE)

#define BUF_SIZE (1024)

#define GPIO_GREEN     (GPIO_NUM_32)
#define GPIO_RED       (GPIO_NUM_33)

#define STRCMP_UNEQUAL  0
#define STRCMP_EQUAL    1

//RX/TX pins on the feather board are labelled as 16RX - GPIO3 / 17TX - GPIO1 for the UART0 port
#define PIN_TX 1
#define PIN_RX 3
#define BAUD_RATE 115200
#define PORT_NUM 0
#define STACK_SIZE 2048

int mystrcmp(uint8_t* uart_input, const char* cmd, int start, int end) {
    int n = 0;
    for(int i = start; i <= end; i++) {
        if (uart_input[i] != cmd[n++])
            return STRCMP_UNEQUAL;
    }
    return STRCMP_EQUAL;
}

void resetBuffer(uint8_t* data, int len) {
    for(int i = 0; i < len; i++)
        data[i] = '\0';
}

void gpio_setter(void)
{
    /* Configure the IOMUX register for pad BLINK_GPIO (some pads are
       muxed to GPIO on reset already, but some default to other
       functions and need to be switched to GPIO. Consult the
       Technical Reference for a list of pads and their default
       functions.)
    */
    gpio_pad_select_gpio(GPIO_GREEN);
    gpio_pad_select_gpio(GPIO_RED);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(GPIO_GREEN, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_RED, GPIO_MODE_OUTPUT);
}


int uartEchoTest () {
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

    return 0;
}

static void echo_task()
{
    // uart_driver_install(UART_NUM, BUF_SIZE * 2, 0, 0, NULL, 0);
    // uart_param_config(UART_NUM, &uart_config);
    // uart_set_pin(UART_NUM, ECHO_TEST_TXD, ECHO_TEST_RXD, ECHO_TEST_RTS, ECHO_TEST_CTS);

    // Configure a temporary buffer for the incoming data
    uint8_t *data = (uint8_t *) malloc(BUF_SIZE);
    int buflen = 0;

    while (1) {
        // Read data from the UART
        uart_get_buffered_data_len(PORT_NUM, (size_t*)&buflen);
        if (buflen >= 7) {
            resetBuffer(data, BUF_SIZE);
            int len = uart_read_bytes(PORT_NUM, data, BUF_SIZE, 20 / portTICK_RATE_MS);
            // uart_write_bytes(PORT_NUM, (const char *)data, len);
            ESP_LOGI("GUI-BLINK", "data: %s\n", data);
            // Write data back to the UART
            // uart_write_bytes(UART_NUM_1, (const char *) data, len);
            if(mystrcmp(data, "LEDR", 1, 4) == STRCMP_EQUAL) {
                if(mystrcmp(data, "ON", 5, 6) == STRCMP_EQUAL) {
                    gpio_set_level(GPIO_RED, 1);
                    ESP_LOGI("GUI-BLINK", "turning red on\n");
                }
                else if(mystrcmp(data, "OF", 5, 6) == STRCMP_EQUAL) {
                    gpio_set_level(GPIO_RED, 0);
                    ESP_LOGI("GUI-BLINK", "turning red off\n");
                }
            }
            if(mystrcmp(data, "LEDG", 1, 4) == STRCMP_EQUAL) {
                if(mystrcmp(data, "ON", 5, 6) == STRCMP_EQUAL) {
                    gpio_set_level(GPIO_GREEN, 1);
                    ESP_LOGI("GUI-BLINK", "turning green on\n");
                }
                else if(mystrcmp(data, "OF", 5, 6) == STRCMP_EQUAL) {
                    gpio_set_level(GPIO_GREEN, 0);
                    ESP_LOGI("GUI-BLINK", "turning green off\n");
                }
            }
            resetBuffer(data, BUF_SIZE);
            // vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
    }
}

void app_main(void)
{
    spi_init(SPI2_HOST);
    mount_fs();
    sleep(3);

    gpio_setter();
    uartEchoTest();
    echo_task();
    // xTaskCreate(echo_task, "uart_echo_task", 1024, NULL, 10, NULL);

    esp_vfs_fat_sdcard_unmount(MOUNT_POINT, sdcard);
}
