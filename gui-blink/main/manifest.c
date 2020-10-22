#include "../include/manifest.h"
#include <stdlib.h>
#include <string.h>

static ManifestContent* content = NULL;

void _clearBuffer(char* buff, int size) {
    for (int i = 0; i < size; i++)
        buff[i] = '\0';
}

void parseManifest() {
    FILE* fp = fopen("/sdcard/manifest", "r");

    // allocate data structure to store info
    content = malloc(sizeof(ManifestContent));
    content->numEntry = 0;
    content->head = NULL;
    content->tail = NULL;

    if (fp == NULL)
        return;

    // store manifest info to structure
    char displayName_buffer[32];
    char userName_buffer[32];
    char url_buffer[2048];
    int r = 0;
    while (feof(fp) && r < MAX_ENTRY) {
        ManifestEntry* newEntry = calloc(1, sizeof(ManifestEntry));

        // assign as head
        if (content->head == NULL)
            content->head = newEntry;

        // assign latest node as tail
        content->tail = newEntry;

        // store displayname
        int i = 0;
        _clearBuffer(displayName_buffer, 32);
        do {
            displayName_buffer[i] = fgetc(fp);
        } while (displayName_buffer[i++] != ':');
        displayName_buffer[--i] = '\0'; // removal of ':'

        // store username
        i = 0;
        _clearBuffer(userName_buffer, 32);
        do {
            userName_buffer[i] = fgetc(fp);
        } while (userName_buffer[i++] != ',');
        userName_buffer[--i] = '\0'; // removal of ','

        // store url
        i = 0;
        _clearBuffer(url_buffer, 2048);
        do {
            url_buffer[i] = fgetc(fp);
        } while (url_buffer[i++] != ',');
        url_buffer[--i] = '\0'; // removal of ','

        addManifestEntry(displayName_buffer, userName_buffer, url_buffer);
        r++;
    }
}

int manifestEntryExist(char* displayName) {
    if (content == NULL)
        parseManifest();
    ManifestEntry* currEntry = content->head;
    while (currEntry != NULL) {
        if (strcmp(currEntry->next->displayName, displayName) == 0)
            return 1;
    }
    return 0;
}

int writeManifestToFile() {
    FILE* fp = fopen("/sdcard/manifest", "w");
    if (fp == NULL)
        return 0;
    ManifestEntry* currEntry = content->head;
    while (currEntry != NULL) {
        fprintf(fp, currEntry->displayName);
        fputc(':', fp);
        fprintf(fp, currEntry->username);
        fputc(',', fp);
        fprintf(fp, currEntry->url);
        fputc(',', fp);
        fputc('\n', fp);
        currEntry = currEntry->next;
    }
    fclose(fp);
    return 1;
}

void addManifestEntry(char* displayName, char* username, char* url) {
    if (content == NULL)
        parseManifest();
    ManifestEntry* newEntry = calloc(1, sizeof(ManifestEntry));
    strcpy(newEntry->displayName, displayName);
    strcpy(newEntry->username, username);
    strcpy(newEntry->url, url);
    if (content->head == NULL) {
        content->head = newEntry;
        content->tail = newEntry;
    }
    else {
        content->tail->next = newEntry;
    }
    content->tail = newEntry;
    newEntry->next = NULL;
    content->numEntry += 1;
}

ManifestEntry* getManifestEntry(char* displayName) {
    if (content == NULL)
        parseManifest();
    ManifestEntry* currEntry = content->head;
    while(currEntry != NULL) {
        if (strcmp(currEntry->displayName, displayName) == 0)
            break;
        currEntry = currEntry->next;
    };
    return currEntry;
}

int removeManifestEntry(char* displayName) {
    if (content == NULL)
        parseManifest();
    ManifestEntry* currEntry = content->head;
    if (currEntry == NULL)
        return 0;

    if (strcmp(currEntry->displayName, displayName) == 0) {
        content->head = currEntry->next;
        free(currEntry);
        content->numEntry -= 1;
        return 1;
    }
    
    while (currEntry->next != NULL) {
        ManifestEntry* prevEntry = currEntry;
        currEntry = currEntry->next;
        if (strcmp(currEntry->displayName, displayName) == 0) {
            prevEntry->next = currEntry->next;
            if (currEntry->next == NULL)
                content->tail = prevEntry;
            free(currEntry);
            content->numEntry -= 1;
            return 1;
        }
    };

    return 0;
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