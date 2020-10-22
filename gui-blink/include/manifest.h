#ifndef __MANIFEST_H__
#define __MANIFEST_H__

#include <stdio.h>
#include <stdint.h>

#define MAX_ENTRY 256

typedef struct {
    uint8_t displayName[32];
    uint8_t username[32];
    uint8_t url[2048];
} ManifestEntry;

typedef struct {
    int numEntry;
    ManifestEntry entryArray[MAX_ENTRY];
} ManifestContent;

ManifestContent* content = NULL;

void parseManifest();
int manifestEntryExist(uint8_t* displayname);
esp_err_t writeManifestToFile();
void addManifestEntry(uint8_t* displayName, uint8_t* login, uint8_t* url);
void getManifestEntry();
esp_err_t removeManifestEntry();

#endif