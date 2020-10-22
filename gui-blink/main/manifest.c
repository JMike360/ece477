#include "../include/manifest.h"
#include "esp_err.h"
#include <string.h>

void _clearBuffer(uint8_t* buff, int size) {
    for (int i = 0; i < size; i++)
        buff[i] = '\0';
}

void parseManifest() {
    FILE* fp = fopen("/sdcard/manifest", "r");
    if (fp == NULL)
        return ESP_FAIL;

    // allocate data structure to store info
    content = malloc(sizeof(ManifestContent));
    content->numEntry = 0;

    // store manifest info to structure
    uint8_t buffer[100];
    int r = 0;
    while (feof(fp) && r < MAX_ENTRY) {
        // store displayname
        int i = 0;
        _clearBuffer(buffer, sizeof(buffer) / sizeof(buffer[0]));
        do {
            buffer[i] = fgetc(fp);
        } while (buffer[i++] != ':');
        buffer[--i] = '\0'; // removal of ':'
        strcpy(content->entryArray[r].displayName, buffer);

        // store username
        i = 0;
        _clearBuffer(buffer, sizeof(buffer) / sizeof(buffer[0]));
        do {
            buffer[i] = fgetc(fp);
        } while (buffer[i++] != ',');
        buffer[--i] = '\0'; // removal of ','
        strcpy(content->entryArray[r].username, buffer);

        // store url
        i = 0;
        _clearBuffer(buffer, sizeof(buffer) / sizeof(buffer[0]));
        do {
            buffer[i] = fgetc(fp);
        } while (buffer[i++] != ',');
        buffer[--i] = '\0'; // removal of ','
        strcpy(content->entryArray[r].url, buffer);

        r++;
    }
}

int manifestEntryExist(uint8_t* displayname) {
    int e = 0;
    while (e < content->numEntry) {
        if (strcmp(content->entryArray[e].displayName, displayname) == 0)
            return 1;
    }
    return 0;
}

esp_err_t writeManifestToFile() {
    FILE* fp = fopen("/sdcard/manifest", "w");
    if (fp == NULL)
        return ESP_FAIL;
    for(int i = 0; i < content->numEntry; i++) {
        fprintf(fp, content->entryArray[i].displayName);
        fputc(':', fp);
        fprintf(fp, content->entryArray[i].username);
        fputc(',', fp);
        fprintf(fp, content->entryArray[i].url);
        fputc(',', fp);
        fputc('\n', fp);
    }
    fclose(fp);
    return ESP_OK;
}

void addManifestEntry(uint8_t* displayName, uint8_t* username, uint8_t* url) {
    strcpy(content->entryArray[content->numEntry].displayName, displayName);
    strcpy(content->entryArray[content->numEntry].username, username);
    strcpy(content->entryArray[content->numEntry].url, url);
    content->numEntry += 1;
}

void getManifestEntry() {

}

esp_err_t removeManifestEntry(uint8_t* displayName) {
    int i = 0;
    while (i < content->numEntry) {
        if (strcmp(content->entryArray[i].displayName, displayName) == 0) {
            content->entryArray[i].displayName[0] = '\0';
            content->entryArray[i].username[0] = '\0';
            content->entryArray[i].url[0] = '\0';
            return ESP_OK;
        }
    }
    return ESP_FAIL;
}

//     mount_fs();

//     ESP_LOGI(TAG, "Opening file");
//     FILE* f = fopen(MOUNT_POINT"/hello.txt", "w");
//     if (f == NULL) {
//         ESP_LOGE(TAG, "Failed to open file for writing");
//         return;
//     }
//     fprintf(f, "Hello %s!\n", sdcard->cid.name);
//     fclose(f);
//     ESP_LOGI(TAG, "File written");
//     sleep(3);
//     esp_vfs_fat_sdcard_unmount(MOUNT_POINT, sdcard);