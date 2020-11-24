#ifndef __MANIFEST_H__
#define __MANIFEST_H__

#include <stdio.h>
#include <stdint.h>

#define MAX_ENTRY 256
#define MANIFEST_SUCCESS 1
#define MANIFEST_FAILURE 0

#define MANIFEST_FILENAME "/sdcard/MANIFEST"

typedef struct _ManifestEntry {
    char displayName[32];
    char username[32];
    char url[2048];
    struct _ManifestEntry* next;
} ManifestEntry;

typedef struct {
    int numEntry;
    ManifestEntry* head;
    ManifestEntry* tail;
} ManifestContent;

int readManifestToMemory();
int writeManifestToFile();
int addManifestEntry(char* displayName, char* login, char* url);
ManifestEntry* getManifestEntry(char* displayName, char* userName);
int removeManifestEntry(char* displayName, char* userName);
int deallocateManifest();
int wipeStorageData();

#endif