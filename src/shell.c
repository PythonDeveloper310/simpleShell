#include "shell.h"
#include "utils.h"
#include "fs/fs.h"
#include <stdio.h>
#include <time.h>

#define MAX_INPUT 256
#define MAX_ARGS 16
#define VERSION 3.2

void shell_loop(char *username, bool new_user)
{
    char input[MAX_INPUT];
    char *args[MAX_ARGS];
    char path[PATH_BUFFER];
    int current_dir = 0;
    int cmd_count = 0;
    time_t start = time(NULL);

    getCurrentPath(current_dir, path, PATH_BUFFER);

    clear_screen();
    printf("simpleShell version %.1f\n", VERSION);
    puts("Type 'help' for list of available commands.");
    if (!new_user)
        printf("Welcome back, %s.\n", username);
    
    puts("");

    while (1) {
        loadMetadata();

        printf("%s@simple-shell:%s > ", username, path);

        if (!fgets(input, sizeof(input), stdin)) break;
        input[strcspn(input, "\n")] = '\0';
        if (strlen(input) == 0) continue;

        int i = 0;
        char *token = strtok(input, " ");
        while (token && i < MAX_ARGS - 1) {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL;

        char *cmd = args[0];
        cmd_count++;
        if (!cmd) continue;

        if (cmp(cmd, "exit")) {
            puts("Exiting shell...");
            break;
        } else if (cmp(cmd, "help")) {
            printf(
                "Available commands:\n"
                "   Core:\n"
                "       help       - Show this message.\n"
                "       echo <msg> - Print message.\n"
                "       clear      - Clear screen.\n"
                "       ver        - Show version.\n"
                "       about      - Info about shell.\n"
                "       ezfetch    - Show shell info.\n"
                "       exit       - Exit shell.\n"
                "\n"
                "   MicroVFS:\n"
                "       format      - Format virtual disk (%s).\n"
                "       mkdir <dir> - Create directory.\n"
                "       mkfile <f>  - Create file.\n"
                "       micro <f>   - Edit file.\n"
                "       cat <f>     - Read file.\n"
                "       rm <f>      - Delete file.\n"
                "       cd <dir>    - Change directory.\n"
                "       ls          - List files.\n",
                DISK_FILE
            );
        } else if (cmp(cmd, "echo")) {
            for (int j = 1; args[j]; j++) printf("%s ", args[j]);
            puts("");
        } else if (cmp(cmd, "clear")) {
            clear_screen();
        } else if (cmp(cmd, "ver")) {
            printf("simpleShell version %.1f\n", VERSION);
        } else if (cmp(cmd, "about")) {
            puts("simpleShell - minimal C shell made for learning.");
        } else if (cmp(cmd, "ezfetch")) {
            double uptime = difftime(time(NULL), start);
            printf(
                "ezFetch\n"
                "Version: %.1f\n"
                "Commands executed: %d\n"
                "Uptime: %.0f seconds\n",
                VERSION, cmd_count, uptime
            );
        } else if (cmp(cmd, "format")) {
            formatDisk();
        } else if (cmp(cmd, "mkdir") || cmp(cmd, "mkfile")) {
            if (!args[1]) {
                printf("Usage: %s <name>\n", cmd);
                continue;
            }
            int type = cmp(cmd, "mkdir") ? TYPE_DIRECTORY : TYPE_FILE;
            if (createEntry(args[1], current_dir, type) != -1)
                saveMetadata();
            else printf("Failed to create %s.\n", args[1]);
        } else if (cmp(cmd, "micro")) {
            if (!args[1]) { puts("Usage: micro <fileName>"); continue; }
            int idx = findEntry(args[1], current_dir);
            if (idx != -1 && getType(idx) == TYPE_FILE) microWrite(idx);
            else puts("File not found.");
        } else if (cmp(cmd, "cat")) {
            if (!args[1]) { puts("Usage: cat <fileName>"); continue; }
            int idx = findEntry(args[1], current_dir);
            if (idx != -1 && getType(idx) == TYPE_FILE) readFile(idx);
            else puts("File not found.");
        } else if (cmp(cmd, "rm")) {
            if (!args[1]) { puts("Usage: rm <fileName>"); continue; }
            int idx = findEntry(args[1], current_dir);
            if (idx != -1) {
                deleteEntry(idx);
                saveMetadata();
            } else puts("File/Directory not found.");
        } else if (cmp(cmd, "cd")) {
            if (!args[1]) { puts("Usage: cd <dirName>"); continue; }
            if (strcmp(args[1], "..") == 0) {
                int parent = getParent(current_dir);
                if (parent != -1) current_dir = parent;
            } else {
                int idx = findEntry(args[1], current_dir);
                if (idx != -1 && isDirectory(idx)) current_dir = idx;
                else puts("Directory not found.");
            }
            getCurrentPath(current_dir, path, PATH_BUFFER);
        } else if (cmp(cmd, "ls")) {
            listDirectory(current_dir);
        }
        else {
            printf("Unknown command: %s\n", cmd);
        }
    }
}
