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

#include "../include/bt.h"
#include "../include/sdcard.h"
#include "../include/manifest.h"
#include "../include/fingerprint.h"
#include "../include/my_rsa.h"

#define BUF_SIZE_MAIN (1024)
#define TAG "MAIN"

/**************************************************
 * ledInit
 * Initialize GPIO pin for LEDs using GPIO_GREEN and
 * GPIO_RED and set them to output mode
 * 
 * input:
 * void
 * 
 * output:
 * void
**************************************************/
void ledInit(void) {
    gpio_pad_select_gpio(GPIO_GREEN);
    gpio_pad_select_gpio(GPIO_RED);
    gpio_set_direction(GPIO_GREEN, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_RED, GPIO_MODE_OUTPUT);
    ESP_LOGI(TAG, "Successfully initialized LED GPIO");
}

/**************************************************
 * _clearDataBuffer
 * Read UART or Bluetooth messages depending on mode
 * and obtains the command string. This function stays
 * in an infinite loop until the terminate command is given.
 * 
 * input:
 * uint8_t* - buffer for storing message
 * 
 * output:
 * void
**************************************************/
void readUARTCMD(uint8_t* data) {
    int i = 0;
    do {
        i += uart_read_bytes(UART_NUM_0, &data[i], BUF_SIZE_MAIN, 20 / portTICK_RATE_MS);
    } while(data[i-1] != '\n');
    // ESP_LOGI(TAG, "reading from UART: %s\n", data);
    if (data[0] != '#')
        return;
    doCMD(data, UART_MODE);
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
    sdspiInit();
    mountSD();
    ledInit();
    btInit();
    btRegister();
    btSetPairing();
    sleep(2);
    
    uart_begin(UART_NUM_0);
    uart_begin(UART_NUM_2);
    my_rsa_init();

    sleep(2);
    ESP_LOGI(TAG, "All initialization complete");

    while (checkFingerEnrolled() == 0) {
        if (enrollFinger(1) == -1)
            ESP_LOGE(TAG, "Fingerprint enrollment failed");
    }

    if (readManifestToMemory() == MANIFEST_FAILURE)
        return;
    
    testRSA();

    uint8_t* data = (uint8_t*) malloc(BUF_SIZE_MAIN);
    while(getRunning()) {
        memset(data, 0, BUF_SIZE_MAIN);
        readUARTCMD(data);
    }
    free(data);

    if (writeManifestToFile() == MANIFEST_FAILURE)
        return;
    if (deallocateManifest() == MANIFEST_FAILURE)
        return;

    unmountSD();
    uart_end(UART_NUM_0);
    uart_end(UART_NUM_2);
}
