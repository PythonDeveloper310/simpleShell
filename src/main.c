#include "shell.h"
#include "utils.h"
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#define MAX_SIZE 64
#define USERS_FILE "shusers.txt"

static void strip_newline(char *str)
{
    if (str)
        str[strcspn(str, "\r\n")] = '\0';
}

int main(void)
{
    char username[MAX_SIZE] = {0};
    char password[MAX_SIZE] = {0};
    char file_user[MAX_SIZE] = {0};
    char file_pass[MAX_SIZE + 1] = {0};
    char hashed_password[MAX_SIZE + 1] = {0};
    char line[128] = {0};

    bool new_user = false;

    clear_screen();
    puts("simpleShell Login System.");

    printf("[simpleShell] Username: ");
    if (!fgets(username, sizeof(username), stdin))
        return 1;
    strip_newline(username);

    if (stremp(username)) {
        strncpy(username, "root", MAX_SIZE - 1);
        username[MAX_SIZE - 1] = '\0';
    }

    while (true) {
        bool user_found = false;
        bool login_success = false;

        printf("[simpleShell] Password: ");
        if (!fgets(password, sizeof(password), stdin))
            return 1;
        strip_newline(password);

        if (stremp(password)) {
            strncpy(password, "simple-shell", MAX_SIZE - 1);
            password[MAX_SIZE - 1] = '\0';
        }

        sha256_string(password, hashed_password);
        FILE *usersFile = fopen(USERS_FILE, "r");

        if (usersFile) {
            while (fgets(line, sizeof(line), usersFile)) {
                if (sscanf(line, "%63s %64s", file_user, file_pass) == 2) {
                    strip_newline(file_user);
                    strip_newline(file_pass);

                    if (cmp(username, file_user)) {
                        user_found = true;
                        if (cmp(hashed_password, file_pass))
                            login_success = true;
                        break;
                    }
                }
            }
            fclose(usersFile);
        }

        if (!user_found) {
            puts("User not found. Creating new user...");
            FILE *appendFile = fopen(USERS_FILE, "a");
            if (!appendFile) {
                puts("Error opening users data file.");
                return 1;
            }

            fprintf(appendFile, "%s %s\n", username, hashed_password);
            fclose(appendFile);

            new_user = true;
            break;
        }
        if (login_success) {
            new_user = false;
            break;
        }
        puts("Invalid password.");
    }

    shell_loop(username, new_user);
    return 0;
}
