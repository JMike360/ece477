#include <string.h>
#include "../include/cmd.h"
#include "esp_err.h"
#include "../include/manifest.h"

// one function per command

esp_err_t cmd_led_red(int status) {
    esp_err_t err = ESP_OK;
    if (status == 1) {
        err = gpio_set_level(GPIO_RED, 1);
        ESP_LOGI("UART-CMD", "turning red on\n");
    }
    else if (status == 0) {
        err = gpio_set_level(GPIO_RED, 0);
        ESP_LOGI("UART-CMD", "turning red off\n");
    }
    return err;
}

esp_err_t cmd_led_green(int status) {
    esp_err_t err = ESP_OK;
    if(status == 1) {
        err = gpio_set_level(GPIO_GREEN, 1);
        ESP_LOGI("UART-CMD", "turning green on\n");
    }
    else if(status == 0) {
        err = gpio_set_level(GPIO_GREEN, 0);
        ESP_LOGI("UART-CMD", "turning green off\n");
    }
    return err;
}

esp_err_t cmd_request_entries() {

}

esp_err_t cmd_request_credential(uint8_t* filename) {

}

esp_err_t cmd_store_credential(uint8_t* displayname, uint8_t* url, uint8_t* username, uint8_t* pw) {
    if (!manifestEntryExist(displayname)) {
        
    }
    uint8_t path[256] = {'\0'};
    strcat(path, "/sdcard/");
    strcat(path, displayname);
    FILE* fp = fopen(path, "w");
    if (fp == NULL)
        return ESP_FAIL;
    fclose(fp);
}