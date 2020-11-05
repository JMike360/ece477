#include <string.h>
#include "../include/cmd.h"
#include "../include/manifest.h"
#include "esp_log.h"
#include "../include/bt.h"
#include <time.h>

// one function per command

static int running = 1;

int getRunning() {
    return running;
}

int cmd_led_red(int status) {
    return gpio_set_level(GPIO_RED, status);
}

int cmd_led_green(int status) {
    return gpio_set_level(GPIO_GREEN, status);
}

int cmd_request_entries() {
    writeManifestToFile();
    FILE* fp = fopen(MANIFEST_FILENAME, "r");
    if (fp == NULL)
        return CMD_FAILURE;
    fseek(fp, 0, SEEK_END);
    size_t filesize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char* buffer = calloc(filesize, sizeof(char));
    fread(buffer, sizeof(char), filesize, fp);
    uart_write_bytes(PORT_NUM, buffer, filesize);
    free(buffer);
    fclose(fp);
    return CMD_SUCCESS;
}

int cmd_request_credential(char* displayName, char* username) {
    if (getManifestEntry(displayName, username) == NULL)
        return CMD_FAILURE;
    ManifestEntry* entry = getManifestEntry(displayName, username);
    if (entry == NULL)
        return CMD_FAILURE;
    char path[256] = {'\0'};
    strcat(path, "/sdcard/");
    strcat(path, displayName); 
    FILE* fp = fopen(path, "r");
    if (fp == NULL)
        return CMD_FAILURE;
    fseek(fp, 0, SEEK_END);
    size_t filesize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char* buffer = calloc(filesize + 1, sizeof(char));
    fread(buffer, sizeof(char), filesize, fp);
    buffer[filesize] = '\n';
    uart_write_bytes(PORT_NUM, buffer, filesize + 1);
    free(buffer);
    fclose(fp);
    return CMD_SUCCESS;
}

int cmd_modify_credential(char* displayName, char* username, char* pw) {
    if (getManifestEntry(displayName, username) == NULL)
        return MANIFEST_FAILURE;
        
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

int cmd_store_credential(char* displayName, char* username, char* url, char* pw) {
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

int cmd_delete_credential(char* displayName, char* userName) {
    removeManifestEntry(displayName, userName);
    char path[256] = {'\0'};
    strcat(path, "/sdcard/");
    strcat(path, displayName);
    remove(path);
    return CMD_SUCCESS;
}

void doCMD(uint8_t* data, int mode) {
    int returnStatus = 0;
    char *displayName, *username, *url, *pw;
    switch (data[1]) {
        case CMD_LED_RED:
            returnStatus = cmd_led_red(data[3]);
            break;
        case CMD_LED_GREEN:
            returnStatus = cmd_led_green(data[3]);
            break;
        case CMD_REQUEST_ENTRIES:
            cmd_request_entries();
            break;
        case CMD_REQUEST_CREDENTIAL:
            displayName = strtok((char*) &data[3], ",");
            username = strtok(NULL, ",");
            cmd_request_credential(displayName, username);
            break;
        case CMD_STORE_CREDENTIAL:
            displayName = strtok((char*) &data[3], ",");
            username = strtok(NULL, ",");
            url = strtok(NULL, ",");
            pw = strtok(NULL, ",");
            returnStatus = cmd_store_credential(displayName, username, url, pw);
            break;
        case CMD_MODIFY_CREDENTIAL:
            displayName = strtok((char*) &data[3], ",");
            username = strtok(NULL, ",");
            pw = strtok(NULL, ",");
            returnStatus = cmd_modify_credential(displayName, username, pw);
            break;
        case CMD_DELETE_CREDENTIAL:
            displayName = strtok((char*) &data[3], ",");
            username = strtok(NULL, ",");
            returnStatus = cmd_delete_credential(displayName, username);
            break;
        case CMD_POWER_OFF:
            running = 0;
            break;
    }

    switch (data[1]) {
        case CMD_LED_RED:
        case CMD_LED_GREEN:
        case CMD_STORE_CREDENTIAL:
        case CMD_MODIFY_CREDENTIAL:
        case CMD_DELETE_CREDENTIAL:
            if (mode == BT_MODE)
                btSendData((uint8_t*) &returnStatus, 1);
            else if (mode == UART_MODE) 
                uart_write_bytes(PORT_NUM, (char*) &returnStatus, 1);
    }
}