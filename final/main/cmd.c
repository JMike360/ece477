#include <string.h>
#include "../include/cmd.h"
#include "../include/manifest.h"
#include "esp_log.h"
#include "../include/bt.h"
#include "../include/my_aes.h"
#include "../include/fingerprint_driver.h"

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

int cmd_request_entries(int mode) {
    writeManifestToFile();
    FILE* fp = fopen(MANIFEST_FILENAME, "r");
    if (fp == NULL)
        return CMD_FAILURE;
    fseek(fp, 0, SEEK_END);
    size_t filesize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char* buffer = calloc(filesize, sizeof(char));
    fread(buffer, sizeof(char), filesize, fp);
    if (mode == UART_MODE)
        uart_write_bytes(PORT_NUM_CMD, buffer, filesize);
    else if (mode == BT_MODE)
        btSendData((uint8_t*) buffer);
    free(buffer);
    fclose(fp);
    return CMD_SUCCESS;
}

int cmd_request_credential(char* displayName, char* username, int mode) {
    if (authenticateFinger() == 0)
        return CMD_FAILURE;

    uint8_t* key = NULL;
    int keysize = 0;
    if (getCryptoKey(&key, &keysize) == -1)
        return CMD_FAILURE;

    if (getManifestEntry(displayName, username) == NULL)
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

    char* buffer = calloc(filesize, sizeof(char));
    char* plaintext = calloc(filesize + 1, sizeof(char));
    fread(buffer, sizeof(char), filesize, fp);
    my_aes_decrypt((uint8_t*)buffer, key, (uint8_t*)plaintext);
    plaintext[filesize] = '\n';

    if (mode == UART_MODE)
        uart_write_bytes(PORT_NUM_CMD, plaintext, filesize + 1);
    else if (mode == BT_MODE)
        btSendData((uint8_t*) plaintext);
        
    free(buffer);
    free(plaintext);
    free(key);
    fclose(fp);
    return CMD_SUCCESS;
}

int cmd_modify_credential(char* displayName, char* username, char* pw) {
    if (authenticateFinger() == 0)
        return CMD_FAILURE;

    uint8_t* key = NULL;
    int keysize = 0;
    if (getCryptoKey(&key, &keysize) == -1)
        return CMD_FAILURE;
    
    if (getManifestEntry(displayName, username) == NULL)
        return MANIFEST_FAILURE;
        
    char path[256] = {'\0'};
    strcat(path, "/sdcard/");
    strcat(path, displayName);
    FILE* fp = fopen(path, "w");
    if (fp == NULL)
        return CMD_FAILURE;

    char* encryptedText = calloc(strlen(pw), sizeof(char));
    my_aes_encrypt((uint8_t*)pw, key, (uint8_t*)encryptedText);

    fprintf(fp, encryptedText);
    free(key);
    free(encryptedText);
    fclose(fp);
    return CMD_SUCCESS;
}

int cmd_store_credential(char* displayName, char* username, char* url, char* pw) {
    if (authenticateFinger() == 0)
        return CMD_FAILURE;

    uint8_t* key = NULL;
    int keysize = 0;
    if (getCryptoKey(&key, &keysize) == -1)
        return CMD_FAILURE;
    
    addManifestEntry(displayName, username, url);
    char path[256] = {'\0'};
    strcat(path, "/sdcard/");
    strcat(path, displayName);
    FILE* fp = fopen(path, "w");
    if (fp == NULL)
        return CMD_FAILURE;

    char* encryptedText = calloc(strlen(pw), sizeof(char));
    my_aes_encrypt((uint8_t*)pw, key, (uint8_t*)encryptedText);
    fprintf(fp, encryptedText);

    fclose(fp);
    return CMD_SUCCESS;
}

int cmd_delete_credential(char* displayName, char* userName) {
    if (authenticateFinger() == 0)
        return CMD_FAILURE;

    if(!removeManifestEntry(displayName, userName))
        return CMD_FAILURE;
    char path[256] = {'\0'};
    strcat(path, "/sdcard/");
    strcat(path, displayName);
    remove(path);
    return CMD_SUCCESS;
}

int cmd_unenroll_fingerprint() {
    if (authenticateFinger() == 0)
        return CMD_FAILURE;

    clearAllData();
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
            returnStatus = cmd_request_entries(mode);
            break;
        case CMD_REQUEST_CREDENTIAL:
            displayName = strtok((char*) &data[3], ",");
            username = strtok(NULL, ",");
            returnStatus = cmd_request_credential(displayName, username, mode);
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
        case CMD_UNENROLL_FINGERPRINT:
            cmd_unenroll_fingerprint();
            break;
        default:
            returnStatus = 0;
    }

    char toSend[2];

    switch (data[1]) {
        case CMD_REQUEST_CREDENTIAL:
        case CMD_REQUEST_ENTRIES:
            if (returnStatus == CMD_FAILURE) {
                toSend[0] = '0';
                toSend[1] = '\n';
                if (mode == BT_MODE)
                    btSendData((uint8_t*) toSend);
                else if (mode == UART_MODE)
                    uart_write_bytes(PORT_NUM_CMD, toSend, 2);
            }
            break;
        case CMD_LED_RED:
        case CMD_LED_GREEN:
        case CMD_STORE_CREDENTIAL:
        case CMD_MODIFY_CREDENTIAL:
        case CMD_DELETE_CREDENTIAL:
            toSend[0] = returnStatus + '0';
            toSend[1] = '\n';
            if (mode == BT_MODE)
                btSendData((uint8_t*) toSend);
            else if (mode == UART_MODE)
                uart_write_bytes(PORT_NUM_CMD, toSend, 2);
    }
}