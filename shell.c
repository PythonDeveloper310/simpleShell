#include "shell.h"
#define MAX_INPUT 256
#define MAX_ARGS 16
#define VERSION 2.3

int cmp(const char * _Str1, const char * _Str2) {
    return strcmp(_Str1, _Str2) == 0;
}

void shell_loop(char *username) {
    char input[MAX_INPUT];
    char *args[MAX_ARGS];
    char *cmd = NULL;
    int cmd_count = 0;
    time_t start = time(0);

    puts("\033[2J\033[1;1H");
    printf("simpleShell version %.1f\n", VERSION);
    puts("Type 'help' for list of available commands.\n");

    while (1) {
        printf("%s@simple-shell:~/ > ", username);
        double uptime = difftime(time(0), start);

        if (!fgets(input, sizeof(input), stdin)) break;
        input[strcspn(input, "\n")] = '\0';
        if (strlen(input) == 0) continue;

        int i = 0;
        char *token = strtok_s(input, " ", &cmd);

        while (token != NULL && i < MAX_ARGS - 1) {
            args[i++] = token;
            token = strtok_s(NULL, " ", &cmd);
        }
        args[i] = NULL;
        cmd = args[0];
        cmd_count++;

        if (cmp(cmd, "exit")) {
            puts("Exiting shell...");
            break;
        } else if (cmp(cmd, "help")) {
            puts("Available commands:");
            puts(
                "   help - Show this help message.\n"
                "   echo <msg> - Display the words in <msg> to the screen.\n"
                "   add <num1> <num2> - Adds num1 with num2 and display the result.\n"
                "   clear - Clears the screen.\n"
                "   version/ver - Display the version.\n"
                "   about - Show simpleShell information.\n"
                "   ezfetch - An easy and simple version of neofetch/fastfetch.\n"
                "   exit - Exits shell."
            );
        } else if (cmp(cmd, "echo")) {
            for (int j = 1; args[j] != NULL; j++)
                printf("%s ", args[j]);
            puts("");
        } else if (cmp(cmd, "clear")) puts("\033[2J\033[1;1H");
        else if (cmp(cmd, "version") || cmp(cmd, "ver")) printf("simpleShell version %.1f\n", VERSION);
        else if (cmp(cmd, "about")) puts("simpleShell - minimal C shell made for learning.");
        else if (cmp(cmd, "add")) {
            if (args[1] && args[2]) {
                int a = atoi(args[1]);
                int b = atoi(args[2]);
                printf("%d", a + b);
                puts("");
            } else
                puts("No arguments. Usage: add <num1> <num2>");
        } else if (cmp(cmd, "ezfetch")) {
            printf(
                "     _            _     ___ _        _ _ \n"
                "  __(_)_ __  _ __| |___/ __| |_  ___| | |\n"
                " (_-< | '  \\| '_ \\ / -_)__ \\ ' \\/ -_) | |\n"
                " /__/_|_|_|_| .__/_\\___|___/_||_\\___|_|_|\n"
                "            |_|                          \n"
                "\n"
                "ezFetch\n"
                "Version: %.1f\n"
                "Shell: simpleShell\n"
                "Commands executed: %d\n"
                "Uptime: %.0f\n",
                VERSION,
                cmd_count,
                uptime
            );
        }
        else {
            printf("Unknown command: %s\n", cmd);
        }
    }
}

