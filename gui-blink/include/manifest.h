#ifndef __MANIFEST_H__
#define __MANIFEST_H__

#include <stdio.h>
#include <stdint.h>

#define MAX_ENTRY 256

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

void parseManifest();
int manifestEntryExist(char* displayname);
int writeManifestToFile();
void addManifestEntry(char* displayName, char* login, char* url);
ManifestEntry* getManifestEntry(char* displayName);
int removeManifestEntry();

#endif