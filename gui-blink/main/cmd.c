#include <string.h>
#include "../include/cmd.h"
#include "../include/manifest.h"
#include "esp_log.h"

// one function per command

int cmd_led_red(int status) {
    return gpio_set_level(GPIO_RED, status);
}

int cmd_led_green(int status) {
    return gpio_set_level(GPIO_GREEN, status);
}

int cmd_request_entries() {
    
    return CMD_SUCCESS;
}

int cmd_request_credential(char* displayName, char* userName) {
    if (!manifestEntryExist(displayName))
        return CMD_FAILURE;
    ManifestEntry* entry = getManifestEntry(displayName, userName);
    if (entry == NULL)
        return CMD_FAILURE;
    FILE* fp = fopen("/sdcard/"displayName);
    if (fp == NULL)
        return CMD_FAILURE;
    fseek(fp, 0, SEEK_END);
    size_t filesize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char* buffer = calloc(filesize, sizeof(char));
    fread(buffer, sizeof(char), filesize, fp);
    uart_write_bytes(PORT_NUM, fp, filesize);
    free(buffer);

    return CMD_SUCCESS;
}

int cmd_store_credential(char* displayName, char* username, char* url, char* pw) {
    if (manifestEntryExist(displayName))
        return CMD_FAILURE;

    addManifestEntry(displayName, username, url);

    char path[256] = {'\0'};
    strcat(path, "/sdcard/");
    strcat(path, displayName);
    FILE* fp = fopen(path, "w");
    if (fp == NULL)
        return CMD_FAILURE;
    
    fprintf(fp, pw);
    fclose(fp);

    return CMD_SUCCESS;
}