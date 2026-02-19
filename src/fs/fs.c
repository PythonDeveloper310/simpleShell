#include "fs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>

#define MAX_ENTRIES     256
#define MAX_NAME        32

typedef struct {
    char name[MAX_NAME];
    int used;
    int type;
    int parent;
    int size;
    int firstBlock;
} ENTRY;

static ENTRY entries[MAX_ENTRIES];
static unsigned char bitmap[TOTAL_BLOCKS];
static const int DATA_START_BLOCK = 20;

// Metadata

void saveMetadata(void)
{
    FILE *f = fopen(DISK_FILE, "rb+");
    if (!f) return;

    fseek(f, 0, SEEK_SET);
    fwrite(entries, sizeof(entries), 1, f);
    fwrite(bitmap, sizeof(bitmap), 1, f);
    fflush(f);
    fclose(f);
}

void loadMetadata(void)
{
    FILE *f = fopen(DISK_FILE, "rb");
    if (!f) return;

    fread(entries, sizeof(entries), 1, f);
    fread(bitmap, sizeof(bitmap), 1, f);
    fclose(f);
}

// Disk

void formatDisk(void)
{
    FILE *f = fopen(DISK_FILE, "wb");
    if (!f) return;

    char zero[BLOCK_SIZE] = {0};

    for (int i = 0; i < TOTAL_BLOCKS; i++)
        fwrite(zero, 1, BLOCK_SIZE, f);

    fclose(f);

    memset(entries, 0, sizeof(entries));
    memset(bitmap, 0, sizeof(bitmap));

    for (int i = 0; i < DATA_START_BLOCK; i++)
        bitmap[i] = 1;

    entries[0].used = 1;
    entries[0].type = TYPE_DIRECTORY;
    entries[0].parent = -1;
    entries[0].firstBlock = -1;
    entries[0].size = 0;
    entries[0].name[0] = '\0';

    saveMetadata();
    printf("Disk formatted.\n");
}

// Block man.

int allocateBlock(void)
{
    for (int i = DATA_START_BLOCK; i < TOTAL_BLOCKS; i++) {
        if (!bitmap[i]) {
            bitmap[i] = 1;
            return i;
        }
    }
    return -1;
}

void freeBlock(int block)
{
    if (block >= DATA_START_BLOCK && block < TOTAL_BLOCKS)
        bitmap[block] = 0;
}

// Entries

int findEntry(const char *name, int parent)
{
    if (!name) return -1;

    for (int i = 0; i < MAX_ENTRIES; i++) {
        if (entries[i].used &&
            entries[i].parent == parent &&
            strcmp(entries[i].name, name) == 0)
            return i;
    }
    return -1;
}

int createEntry(const char *name, int parent, int type)
{
    if (!name || strlen(name) >= MAX_NAME)
        return -1;

    for (int i = 0; i < MAX_ENTRIES; i++) {
        if (!entries[i].used) {
            entries[i].used = 1;
            entries[i].type = type;
            entries[i].parent = parent;
            entries[i].size = 0;
            entries[i].firstBlock = -1;

            strncpy(entries[i].name, name, MAX_NAME - 1);
            entries[i].name[MAX_NAME - 1] = '\0';

            return i;
        }
    }
    return -1;
}

// File io

void microWrite(int index)
{
    if (index < 0 || !entries[index].used)
        return;

    FILE *f = fopen(DISK_FILE, "rb+");
    if (!f) return;

    char buffer[4096];
    int len = 0;
    int cursor = 0;

    system("cls");
    printf("Micro Editor\n");
    printf("ESC = Save & Exit\n");
    printf("----------------------------\n\n");

    while (1) {
        int ch = _getch();

        if (ch == 27)
            break;

        if (ch == 8) {
            if (cursor > 0) {
                cursor--;
                len--;
                printf("\b \b");
            }
        }
        else if (ch == 13) {
            if (len < (int)sizeof(buffer) - 1) {
                buffer[cursor++] = '\n';
                len++;
                printf("\n");
            }
        }
        else if (ch != 224 && len < (int)sizeof(buffer) - 1) {
            buffer[cursor++] = (char)ch;
            len++;
            printf("%c", ch);
        }
    }

    buffer[len] = '\0';

    int block = allocateBlock();
    if (block == -1) {
        printf("\nDisk full.\n");
        fclose(f);
        return;
    }

    fseek(f, block * BLOCK_SIZE, SEEK_SET);
    fwrite(buffer, 1, len, f);

    entries[index].firstBlock = block;
    entries[index].size = len;

    fclose(f);
    saveMetadata();

    printf("\n\nSaved.\n");
}

void readFile(int index)
{
    if (index < 0 || !entries[index].used)
        return;

    FILE *f = fopen(DISK_FILE, "rb");
    if (!f) return;

    int block = entries[index].firstBlock;
    int remaining = entries[index].size;
    char buffer[BLOCK_SIZE];

    while (block != -1 && remaining > 0) {

        int toRead = (remaining > BLOCK_SIZE) ? BLOCK_SIZE : remaining;

        fseek(f, block * BLOCK_SIZE, SEEK_SET);
        fread(buffer, 1, toRead, f);
        fwrite(buffer, 1, toRead, stdout);

        remaining -= toRead;

        fread(&block, sizeof(int), 1, f);
    }

    printf("\n");
    fclose(f);
}

void deleteEntry(int index)
{
    if (index < 0 || !entries[index].used)
        return;

    FILE *f = fopen(DISK_FILE, "rb");
    if (!f) return;

    int block = entries[index].firstBlock;
    int next;

    while (block != -1) {
        fseek(f, block * BLOCK_SIZE, SEEK_SET);
        fread(&next, sizeof(int), 1, f);
        freeBlock(block);
        block = next;
    }

    entries[index].used = 0;
    entries[index].firstBlock = -1;
    entries[index].size = 0;

    fclose(f);
    saveMetadata();

    printf("Deleted.\n");
}

// Directory

void listDirectory(int parent)
{
    for (int i = 0; i < MAX_ENTRIES; i++) {
        if (entries[i].used && entries[i].parent == parent) {
            printf("%s %s\n",
                entries[i].type == TYPE_DIRECTORY ? "[DIR]" : "[FILE]",
                entries[i].name);
        }
    }
}

// Path

void buildPathRecursively(int dir, char *buffer, unsigned long long bufsize)
{
    if (dir <= 0) return;

    int parent = entries[dir].parent;
    buildPathRecursively(parent, buffer, bufsize);

    if (strlen(buffer) + strlen(entries[dir].name) + 2 < bufsize) {
        strcat(buffer, "/");
        strcat(buffer, entries[dir].name);
    }
}

void getCurrentPath(int directory, char *out_buffer, unsigned long long bufsize)
{
    if (!out_buffer || bufsize == 0)
        return;

    out_buffer[0] = '\0';
    buildPathRecursively(directory, out_buffer, bufsize);

    if (strlen(out_buffer) == 0)
        strncpy(out_buffer, "/", bufsize - 1);
}

// Getters

int getType(int index)
{
    return (index >= 0 && entries[index].used) ? entries[index].type : -1;
}

int getSize(int index)
{
    return (index >= 0 && entries[index].used) ? entries[index].size : -1;
}

int isDirectory(int index)
{
    return getType(index) == TYPE_DIRECTORY;
}

int getParent(int index)
{
    return (index >= 0 && entries[index].used) ? entries[index].parent : -1;
}

const char* getName(int index)
{
    return (index >= 0 && entries[index].used) ? entries[index].name : NULL;
}
