#include "../include/manifest.h"
#include <stdlib.h>
#include <string.h>

#include "esp_log.h"

#define TAG "MANIFEST"

static ManifestContent* content = NULL;

void _clearBuffer(char* buff, int size) {
    for (int i = 0; i < size; i++)
        buff[i] = '\0';
}

int readManifestToMemory() {
    FILE* fp = fopen("/sdcard/manifest", "r");
    if (fp == NULL) {
        fp = fopen("/sdcard/manifest", "w");
        if (fp == NULL) {
            ESP_LOGE(TAG, "Unable to open manifest file for reading");
            return MANIFEST_FAILURE;
        }
    }

    if (content != NULL) {
        ESP_LOGE(TAG, "Manifest already read to memory. Deallocate before reading again");
        return MANIFEST_FAILURE;
    }

    // allocate data structure to store info
    content = malloc(sizeof(ManifestContent));
    content->numEntry = 0;
    content->head = NULL;
    content->tail = NULL;

    // store manifest info to structure
    char displayName_buffer[32];
    char userName_buffer[32];
    char url_buffer[2048];
    int r = 0;
    while (feof(fp)) {
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

    ESP_LOGI(TAG, "Successfully read manifest to memory");
    return MANIFEST_SUCCESS;
}

int manifestEntryExist(char* displayName) {
    if (content == NULL)
        return MANIFEST_FAILURE;
    ManifestEntry* currEntry = content->head;
    while (currEntry != NULL) {
        if (strcmp(currEntry->next->displayName, displayName) == 0)
            return MANIFEST_SUCCESS;
    }
    return MANIFEST_FAILURE;
}

int writeManifestToFile() {
    FILE* fp = fopen("/sdcard/manifest", "w");
    if (fp == NULL) {
        ESP_LOGE(TAG, "Unable to open manifest file for writing");
        return MANIFEST_FAILURE;
    }

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

    ESP_LOGI(TAG, "Successfully written manifest to file");
    return MANIFEST_SUCCESS;
}

int deallocateManifest() {
    if (content == NULL) {
        ESP_LOGE(TAG, "Attempting to deallocate manifest that was previously deallocated");
        return MANIFEST_FAILURE;
    }

    ManifestEntry* currEntry = content->head;
    while(currEntry != NULL) {
        ManifestEntry* nextEntry = currEntry->next;
        free(currEntry);
        currEntry = nextEntry;
    }
    free(content);
    content = NULL;

    ESP_LOGI(TAG, "Successfully deallocated manifest on memory");
    return MANIFEST_SUCCESS;
}

int addManifestEntry(char* displayName, char* username, char* url) {
    if (content == NULL)
        return MANIFEST_FAILURE;
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

    ESP_LOGI(TAG, "Successfully added manifest entry");
    return MANIFEST_SUCCESS;
}

ManifestEntry* getManifestEntry(char* displayName, char* userName) {
    if (content == NULL)
        return NULL;
    ManifestEntry* currEntry = content->head;
    while(currEntry != NULL) {
        if ( (strcmp(currEntry->displayName, displayName) == 0) && (strcmp(currEntry->username, userName) == 0) )
            break;
        currEntry = currEntry->next;
    };
    if (currEntry != NULL)
        ESP_LOGI(TAG, "Successfully retrieved manifest entry");
    else
        ESP_LOGE(TAG, "Failed to retrieve manifest entry");
    
    return currEntry;
}

int removeManifestEntry(char* displayName) {
    if (content == NULL)
        NULL;

    ManifestEntry* currEntry = content->head;
    if (currEntry == NULL)
        return MANIFEST_FAILURE;

    if (strcmp(currEntry->displayName, displayName) == 0) {
        content->head = currEntry->next;
        free(currEntry);
        content->numEntry -= 1;
        return MANIFEST_SUCCESS;
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
            ESP_LOGI(TAG, "Successfully removed manifest entry");
            return MANIFEST_SUCCESS;
        }
    };

    ESP_LOGI(TAG, "Failed to remove manifest entry");
    return MANIFEST_FAILURE;
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