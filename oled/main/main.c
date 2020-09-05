#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "device/gpio.h"
#include "esp_err.h"
#include "esp_log.h"

static const char *TAG = "oled";

#define SPI_DMA_CHANNEL 1

// Pin mapping when using SPI mode.
#define PIN_NUM_MISO 12
#define PIN_NUM_MOSI 13
#define PIN_NUM_CLK  14
#define PIN_NUM_CS   15

esp_err_t oled_init() {
    gpio_config_t conf;
    conf.pull_up_en = GPIO_PULL_UP_DISABLE;
    conf.pull_down_en = GPIO_PULL_DOWN_DISABLE;
    conf.intr_type = GPIO_INTR_DISABLE;

    // configure GPIO 13, 14, 15 as output mode
    conf.pin_bit_mask = GPIO_SEL_13 | GPIO_SEL_14 | GPIO_SEL_15;
    conf.mode = GPIO_MODE_OUTPUT;
    esp_err_t ret1 = gpio_config(&conf);
    ESP_LOGE(TAG, "Failed to init GPIO pins for OLED (%s).", esp_err_to_name(ret1));

    if(ret1 != ESP_OK) return ret1;

    // configure GPIO 12 as input mode
    conf.pin_bit_mask = GPIO_SEL_12;
    conf.mode = GPIO_MODE_INPUT;
    esp_err_t ret2 = gpio_config(&conf);
    ESP_LOGE(TAG, "Failed to init GPIO pins for OLED (%s).", esp_err_to_name(ret2));

    return ret2;
}

void display_upper(const char* str) {
    gpio_set_level(PIN_NUM_CS, 0);

    /* write into DR0 of HSPI's MOSI pin */

    gpio_set_level(PIN_NUM_CS, 1);
}

void display_lower(const char* str) {
    gpio_set_level(PIN_NUM_CS, 0);

    /* write into DR0 of HSPI's MOSI pin */

    gpio_set_level(PIN_NUM_CS, 1);
}