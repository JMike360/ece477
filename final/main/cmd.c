#include <string.h>
#include "../include/cmd.h"
#include "../include/manifest.h"
#include "esp_log.h"
#include "../include/bt.h"
#include "../include/my_aes.h"
#include "../include/fingerprint.h"

// one function per command

#define TAG "CMD"

static int running = 1;

int getRunning() {
    return running;
}

int cmd_led_red(int status) {
    ESP_LOGI(TAG, "Successfully set red LED to %d", status);
    return gpio_set_level(GPIO_RED, status);
}

int cmd_led_green(int status) {
    ESP_LOGI(TAG, "Successfully set green LED to %d", status);
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
    if (filesize == 0)
        return CMD_SUCCESS;
    if (mode == UART_MODE)
        uart_write_bytes(UART_NUM_0, buffer, filesize);
    else if (mode == BT_MODE)
        btSendData((uint8_t*) buffer, filesize);
    free(buffer);
    fclose(fp);
    ESP_LOGI(TAG, "Successfully requested manifest entry");
    return CMD_SUCCESS;
}

int cmd_request_credential(char* displayName, char* username, int mode) {
    if (authenticateFinger() == 0) {
        ESP_LOGE(TAG, "Failed to request credential for %s. Fingerprint authentication failed", displayName);
        return CMD_FAILURE;
    }

    for(int i = 0; i < strlen(displayName); i++) {
        ESP_LOGI("displayname", "disp[%d] = %c", i, displayName[i]);
    }
    for(int i = 0; i < strlen(username); i++) {
        ESP_LOGI("username", "user[%d] = %c", i, username[i]);
    }

    uint8_t* key = NULL;
    int keysize = 0;
    if (getCryptoKey(&key, &keysize) == -1)
        return CMD_FAILURE;
    
     for(int i = 0; i < keysize; i++) {
        ESP_LOGI("request cred", "key[%d] = %02x", i, key[i]);
    }

    if (getManifestEntry(displayName, username) == NULL) {
        ESP_LOGE(TAG, "Failed to request credential. Entry not found");
        return CMD_FAILURE;
    }

    char path[256] = {'\0'};
    strcat(path, "/sdcard/");
    strcat(path, displayName); 
    FILE* fp = fopen(path, "rb");
    if (fp == NULL)
        return CMD_FAILURE;
    fseek(fp, 0, SEEK_END);
    size_t filesize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char* buffer = calloc(filesize, sizeof(char));
    char* plaintext = NULL;
    fread(buffer, sizeof(char), filesize, fp);
    my_aes_decrypt((uint8_t*)buffer, key, (uint8_t**)&plaintext);
    int decryptedLen = strlen(plaintext);
    plaintext[decryptedLen] = '\n';

    if (mode == UART_MODE)
        uart_write_bytes(UART_NUM_0, plaintext, decryptedLen+1);
    else if (mode == BT_MODE)
        btSendData((uint8_t*) plaintext, decryptedLen+1);
        
    free(buffer);
    free(plaintext);
    free(key);
    fclose(fp);
    ESP_LOGI(TAG, "Successfully requested credential for %s", displayName);
    return CMD_SUCCESS;
}

int cmd_modify_credential(char* displayName, char* username, char* pw) {
    if (authenticateFinger() == 0) {
        ESP_LOGE(TAG, "Failed to modify credential for %s. Fingerprint authentication failed", displayName);
        return CMD_FAILURE;
    }
    
    if (getManifestEntry(displayName, username) == NULL) {
        ESP_LOGE(TAG, "Failed to modify credential for %s. Entry does not exist", displayName);
        return CMD_FAILURE;
    }

    uint8_t* key = NULL;
    int keysize = 0;
    if (getCryptoKey(&key, &keysize) == -1) {
        ESP_LOGE(TAG, "Failed to modify credential for %s. Failed to hash AES key", displayName);
        return CMD_FAILURE;
    }
        
    char path[256] = {'\0'};
    strcat(path, "/sdcard/");
    strcat(path, displayName);
    FILE* fp = fopen(path, "wb");
    if (fp == NULL) {
        ESP_LOGE(TAG, "Failed to modify credential for %s. Unable to write pw to file", displayName);
        return CMD_FAILURE;
    }

    char* encryptedText = NULL;
    my_aes_encrypt((uint8_t*)pw, key, (uint8_t**)&encryptedText);
    fwrite(encryptedText, sizeof(*encryptedText), AES_BLOCKSIZE, fp);
    free(key);
    free(encryptedText);
    fclose(fp);
    ESP_LOGI(TAG, "Successfully modified credential for %s", displayName);
    return CMD_SUCCESS;
}

int cmd_store_credential(char* displayName, char* username, char* url, char* pw) {
    if (authenticateFinger() == 0) {
        ESP_LOGE(TAG, "Failed to stored credential for %s. Fingerprint authentication failed", displayName);
        return CMD_FAILURE;
    }

    if (getManifestEntry(displayName, username) != NULL) {
        ESP_LOGE(TAG, "Failed to stored credential for %s. Entry already exists", displayName);
        return CMD_FAILURE;
    }

    uint8_t* key = NULL;
    int keysize = 0;
    if (getCryptoKey(&key, &keysize) == -1) {
        ESP_LOGE(TAG, "Failed to stored credential for %s. Failed to hash AES key", displayName);
        return CMD_FAILURE;
    }

    // for(int i = 0; i < keysize; i++) {
    //     ESP_LOGI("store cred", "key[%d] = %02x", i, key[i]);
    // }
    
    addManifestEntry(displayName, username, url);
    char path[256] = {'\0'};
    strcat(path, "/sdcard/");
    strcat(path, displayName);
    FILE* fp = fopen(path, "wb");
    if (fp == NULL) {
        ESP_LOGE(TAG, "Failed to stored credential for %s. Unable to write pw to file", displayName);
        return CMD_FAILURE;
    }

    char* encryptedText = NULL;
    my_aes_encrypt((uint8_t*)pw, key, (uint8_t**)&encryptedText);
    fwrite(encryptedText, sizeof(*encryptedText), AES_BLOCKSIZE, fp);
    free(key);
    free(encryptedText);
    fclose(fp);
    ESP_LOGI(TAG, "Successfully stored credential for %s", displayName);
    return CMD_SUCCESS;
}

int cmd_delete_credential(char* displayName, char* userName) {
    if (authenticateFinger() == 0)
        return CMD_FAILURE;

    if(!removeManifestEntry(displayName, userName)) {
        ESP_LOGE(TAG, "Failed to delete credential for %s", displayName);
        return CMD_FAILURE;
    }
    char path[256] = {'\0'};
    strcat(path, "/sdcard/");
    strcat(path, displayName);
    remove(path);
    ESP_LOGI(TAG, "Successfully deleted credential for %s", displayName);
    return CMD_SUCCESS;
}

int cmd_store_fingerprint() {
    if (authenticateFinger() == 0)
        return CMD_FAILURE;

    if (clearFingerprintData() == -1)
        return CMD_FAILURE;
    
    if (wipeStorageData() == MANIFEST_FAILURE)
        return MANIFEST_FAILURE;
    
    if (enrollFinger(0) == -1)
        return CMD_FAILURE;

    ESP_LOGI(TAG, "Successfully enrolled fingerprint");
    return CMD_SUCCESS;
}

int cmd_delete_fingerprint() {
    if (authenticateFinger() == 0)
        return CMD_FAILURE;

    if (clearFingerprintData() == -1)
        return CMD_FAILURE;
    
    if (wipeStorageData() == MANIFEST_FAILURE)
        return MANIFEST_FAILURE;

    ESP_LOGI(TAG, "Successfully unenrolled fingerprint");
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
        case CMD_STORE_FINGERPRINT:
            cmd_store_fingerprint();
            break;
        case CMD_DELETE_CREDENTIAL:
            displayName = strtok((char*) &data[3], ",");
            username = strtok(NULL, ",");
            returnStatus = cmd_delete_credential(displayName, username);
            break;
        case CMD_DELETE_FINGERPRINT:
            returnStatus = cmd_delete_fingerprint();
            break;
        case CMD_MODIFY_CREDENTIAL:
            displayName = strtok((char*) &data[3], ",");
            username = strtok(NULL, ",");
            pw = strtok(NULL, ",");
            returnStatus = cmd_modify_credential(displayName, username, pw);
            break;
        case CMD_POWER_OFF:
            running = 0;
            break;
        default:
            ESP_LOGE(TAG, "Unrecognized command received: %x", data[1]);
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
                    btSendData((uint8_t*) toSend, 2);
                else if (mode == UART_MODE)
                    uart_write_bytes(UART_NUM_0, toSend, 2);
            }
            break;
        case CMD_LED_RED:
        case CMD_LED_GREEN:
        case CMD_STORE_CREDENTIAL:
        case CMD_MODIFY_CREDENTIAL:
        case CMD_DELETE_CREDENTIAL:
        case CMD_STORE_FINGERPRINT:
        case CMD_DELETE_FINGERPRINT:
            toSend[0] = returnStatus + '0';
            toSend[1] = '\n';
            if (mode == BT_MODE)
                btSendData((uint8_t*) toSend, 2);
            else if (mode == UART_MODE)
                uart_write_bytes(UART_NUM_0, toSend, 2);
    }
}