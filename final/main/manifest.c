#include "../include/manifest.h"
#include <stdlib.h>
#include <string.h>
#include "esp_log.h"

#define TAG "MANIFEST"

static ManifestContent* content = NULL;

/**************************************************
 * _clearBuffer
 * Set all values in data buffer to null char
 * 
 * input:
 * uint8_t* data - buffer to be cleared
 * int len - length of buffer
 * 
 * output:
 * void
**************************************************/
void _clearBuffer(char* buff, int size) {
    for (int i = 0; i < size; i++)
        buff[i] = '\0';
}

/**************************************************
 * readManifestToMemory
 * Parse existing manifest file and store it as a
 * data structure for easy access. Create a new
 * manifest file if no such file existed. The
 * manifest is stored as 'content' in manifest.c
 * 
 * input:
 * void
 * 
 * output:
 * int - manifest return status
**************************************************/
int readManifestToMemory() {
    if (content != NULL) {
        ESP_LOGE(TAG, "Manifest already read to memory. Deallocate before reading again");
        return MANIFEST_FAILURE;
    }
    
    // allocate data structure to store info
    content = malloc(sizeof(ManifestContent));
    content->numEntry = 0;
    content->head = NULL;
    content->tail = NULL;

    FILE* fp = fopen(MANIFEST_FILENAME, "r");
    if (fp == NULL) {
        ESP_LOGI(TAG, "Manifest file does not exist yet");
        return MANIFEST_SUCCESS;
    }

    // store manifest info to structure
    char lineBuffer[64];
    char* displayName = NULL;
    char* username = NULL;
    char* url = NULL;

    do {
        int i = 0;
        memset(lineBuffer, '\0', sizeof(lineBuffer) / sizeof(lineBuffer[0]));
        do {
            lineBuffer[i] = fgetc(fp);
            if (feof(fp))
                return MANIFEST_SUCCESS;
        } while(lineBuffer[i++] != '\n');
        lineBuffer[i-1] = '\0';

        displayName = strtok(lineBuffer, ",");
        username = strtok(NULL, ",");
        url = strtok(NULL, ",");

        addManifestEntry(displayName, username, url);
        // attempt to read past EOF to set EOF flag
        if (fgetc(fp) != EOF)
            fseek(fp, -1, SEEK_CUR);
    } while (!feof(fp));

    fclose(fp);

    ESP_LOGI(TAG, "Successfully read manifest to memory");
    return MANIFEST_SUCCESS;
}

/**************************************************
 * writeManifestToFile
 * Print contents of manifest stored as data
 * structure 'content' in manifest.c into text
 * file named '/sdcard/manifest'. Manifest stored
 * on memory is not deallocated!
 * 
 * input:
 * void
 * 
 * output:
 * int - manifest return status
**************************************************/
int writeManifestToFile() {
    FILE* fp = fopen(MANIFEST_FILENAME, "w");
    if (fp == NULL) {
        ESP_LOGE(TAG, "Unable to open manifest file for writing");
        return MANIFEST_FAILURE;
    }

    ManifestEntry* currEntry = content->head;
    while (currEntry != NULL) {
        fprintf(fp, currEntry->displayName);
        fputc(',', fp);
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

/**************************************************
 * deallocateManifest
 * Free up all memory allocated to linkedlist
 * structure used to store manifest file content.
 * 'content' in manifest.c is set to NULL.
 * 
 * input:
 * void
 * 
 * output:
 * int - manifest return status
**************************************************/
int deallocateManifest() {
    if (content == NULL) {
        ESP_LOGE(TAG, "Attempting to deallocate manifest that was unallocated");
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

    // ESP_LOGI(TAG, "Successfully deallocated manifest on memory");
    return MANIFEST_SUCCESS;
}

/**************************************************
 * addManifestEntry
 * Add a new entry into the manifest on memory if
 * entry does not exist. Otherwise, modifies the
 * existing manifest entry. Prior to calling this
 * function, manifest file must be parsed and stored
 * via readManifestToMemory(). Content should later
 * be saved to text file via writeManifestToFile().
 * 
 * input:
 * char* displayName - website name such as 'Facebook'
 * char* username - login id which is unprotected
 * char* url - website login url such as 'facebook.com'
 * 
 * output:
 * int - manifest return status
**************************************************/
int addManifestEntry(char* displayName, char* username, char* url) {
    if (content == NULL)
        return MANIFEST_FAILURE;

    ManifestEntry* newEntry = NULL;
    newEntry = getManifestEntry(displayName, username);
    if (newEntry == NULL) {
        newEntry = calloc(1, sizeof(ManifestEntry));
        if (content->head == NULL)
            content->head = newEntry;
        else
            content->tail->next = newEntry;
        content->tail = newEntry;
        newEntry->next = NULL;
        content->numEntry += 1;
    }

    strcpy(newEntry->displayName, displayName);
    strcpy(newEntry->username, username);
    strcpy(newEntry->url, url);

    // ESP_LOGI(TAG, "Successfully added manifest entry");
    return MANIFEST_SUCCESS;
}

/**************************************************
 * getManifestEntry
 * Retrieve existing entry from manifest by comparing
 * displayName and username. Return NULL if entry does
 * not exist. Prior to calling this function, manifest
 * file must be parsed and stored via readManifestToMemory().
 * 
 * input:
 * char* displayName - website name such as 'Facebook'
 * char* userName - login id which is unprotected
 * 
 * output:
 * ManifestEntry* - entry that matched displayName and userName
**************************************************/
ManifestEntry* getManifestEntry(char* displayName, char* userName) {
    if (content == NULL)
        return NULL;
    ManifestEntry* currEntry = content->head;
    while(currEntry != NULL) {
        if ( (strcmp(currEntry->displayName, displayName) == 0) && (strcmp(currEntry->username, userName) == 0) )
            break;
        currEntry = currEntry->next;
    };
    /*
    if (currEntry != NULL)
        ESP_LOGI(TAG, "Successfully retrieved manifest entry");
    else
        ESP_LOGE(TAG, "Failed to retrieve manifest entry. Not found");
    */
    
    return currEntry;
}

/**************************************************
 * removeManifestEntry
 * Remove an entry from the manifest on memory if
 * entry existed. If entry does not exist, return fail.
 * Prior to calling this function, manifest file must
 * be parsed and stored via readManifestToMemory().
 * Content should later be saved to text file via
 * writeManifestToFile().
 * 
 * input:
 * char* displayName - website name such as 'Facebook'
 * 
 * output:
 * int - manifest return status
**************************************************/
int removeManifestEntry(char* displayName, char* userName) {
    if (content == NULL)
        NULL;

    ManifestEntry* currEntry = content->head;
    if (currEntry == NULL)
        return MANIFEST_FAILURE;

    if ((strcmp(currEntry->displayName, displayName) == 0)
        && (strcmp(currEntry->username, userName) == 0)) {
        content->head = currEntry->next;
        free(currEntry);
        content->numEntry -= 1;
        return MANIFEST_SUCCESS;
    }
    
    while (currEntry->next != NULL) {
        ManifestEntry* prevEntry = currEntry;
        currEntry = currEntry->next;
        if ((strcmp(currEntry->displayName, displayName) == 0)
            && (strcmp(currEntry->username, userName) == 0)) {
            prevEntry->next = currEntry->next;
            if (currEntry->next == NULL)
                content->tail = prevEntry;
            free(currEntry);
            content->numEntry -= 1;
            ESP_LOGI(TAG, "Successfully removed manifest entry");
            return MANIFEST_SUCCESS;
        }
    };

    ESP_LOGE(TAG, "Failed to remove manifest entry");
    return MANIFEST_FAILURE;
}

/**************************************************
 * wipeStorageData
 * Scan through the manifest entries and delete the
 * corresponding password files before clearing the
 * manifest file.
 * 
 * input:
 * void
 * 
 * output:
 * void
**************************************************/
int wipeStorageData() {
    while(content->head != NULL) {
        char path[256] = {'\0'};
        strcat(path, "/sdcard/");
        strcat(path, content->head->displayName);
        remove(path);
        ManifestEntry* tmp = content->head;
        content->head = content->head->next;
        free(tmp);
    }
    writeManifestToFile();
    ESP_LOGI(TAG, "Successfully wiped SD storage data");
    return MANIFEST_SUCCESS;
}