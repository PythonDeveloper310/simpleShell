#pragma once

#define DISK_FILE       "vdisk.bin"
#define DISK_SIZE       (1024 * 1024)
#define BLOCK_SIZE      1024
#define TOTAL_BLOCKS    (DISK_SIZE / BLOCK_SIZE)
#define PATH_BUFFER     512

#define TYPE_FILE       1
#define TYPE_DIRECTORY  2

void saveMetadata(void);
void loadMetadata(void);
void formatDisk(void);

int allocateBlock(void);
void freeBlock(int block);

int findEntry(const char *name, int parent);
int createEntry(const char *name, int parent, int type);

void microWrite(int index);
void readFile(int index);
void deleteEntry(int index);

int getType(int index);
int getSize(int index);
void listDirectory(int parent);

void buildPathRecursively(int directory, char *buffer, unsigned long long bufsize);
void getCurrentPath(int directory, char *out_buffer, unsigned long long bufsize);

int isDirectory(int index);
int getParent(int index);
const char* getName(int index);
