#include <string.h>
#include "../include/cmd.h"
#include "../include/manifest.h"
#include "esp_log.h"

// one function per command

esp_err_t cmd_led_red(int status) {
    return gpio_set_level(GPIO_RED, status);
}

esp_err_t cmd_led_green(int status) {
    return gpio_set_level(GPIO_GREEN, status);
}

esp_err_t cmd_request_entries() {
    return ESP_OK;
}

esp_err_t cmd_request_credential(char* filename) {
    return ESP_OK;
}

esp_err_t cmd_store_credential(char* displayname, char* url, char* username, char* pw) {
    if (!manifestEntryExist(displayname))
        addManifestEntry(displayname, username, url);
    char path[256] = {'\0'};
    strcat(path, "/sdcard/");
    strcat(path, displayname);
    FILE* fp = fopen(path, "w");
    if (fp == NULL)
        return ESP_FAIL;
    fprintf(fp, pw);
    fclose(fp);
    return ESP_OK;
}