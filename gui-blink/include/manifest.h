#ifndef __MANIFEST_H__
#define __MANIFEST_H__

#include <stdint.h>
#include "driver/sdspi_host.h"

typedef struct {
    uint8_t displayName[32];
    uint8_t username[32];
    uint8_t url[2048];
} ManifestEntry;

typedef struct {
    int numEntry;
    ManifestEntry* entryArray;
} ManifestContent;

int parseManifest(char* filepath);

#endif