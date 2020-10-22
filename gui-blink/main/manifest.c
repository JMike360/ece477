#include "../include/manifest.h"
#include "esp_err.h"
#include <string.h>

ManifestContent* parseManifest(char* filepath) {
    FILE* fp = fopen(MOUNT_POINT"/manifest", "r");
    if (fp == NULL)
        return ESP_FAIL;

    // count number of entries
    int numLines = 0;
    while(!feof(fp)) {
        if (fgetc(fp) == ':')
            numLines++;
    }

    // allocate data structure to store info
    ManifestContent* myContent = malloc(sizeof(ManifestContent));
    myContent->numEntry = 0;
    myContent->entryArray = calloc(numLines, sizeof(ManifestEntry));

    // store manifest info to structure
    char buffer[100] = {'\0'};
    int i = 0;
    do {
        fread(&buffer[i], sizeof(char), 1, fp);
    } while(buffer[i++] != ':');
    buffer[--i] = '\0'; // removal of ':'
    strcpy()
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