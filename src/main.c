#include "shell.h"
#include "utils.h"
#include <stdbool.h>
#include <string.h>

#define MAX_SIZE 64
#define USERS_FILE "shusers.txt"

int main(void)
{
    char username[MAX_SIZE], password[MAX_SIZE];
    char file_user[MAX_SIZE], file_pass[MAX_SIZE + 1];
    char hashed_password[MAX_SIZE + 1];
    char line[128];
    bool new_user = false;

    puts("simpleShell Login System.");
    printf("[simpleShell] Username: ");
    fgets(username, MAX_SIZE, stdin);
    username[strcspn(username, "\n")] = 0;

    while (true) {
        bool user_found = false;
        bool login_success = false;

        printf("[simpleShell] Password: ");
        fgets(password, MAX_SIZE, stdin);
        password[strcspn(password, "\n")] = 0;

        if (stremp(username)) {
            strncpy(username, "root", MAX_SIZE);
            username[MAX_SIZE-1] = '\0';
        }

        if (stremp(password)) {
            strncpy(password, "simple-shell", MAX_SIZE);
            password[MAX_SIZE-1] = '\0';
        }

        sha256_string(password, hashed_password);
        printf("DEBUG: hashed input: %s\n", hashed_password);
        FILE *usersFile = fopen(USERS_FILE, "r");
        
        if (usersFile != NULL) {
            while (fgets(line, sizeof(line), usersFile)) {
                sscanf(line, "%63s %64s", file_user, file_pass);
                file_pass[strcspn(file_pass, "\r\n")] = 0;
                file_user[strcspn(file_user, "\r\n")] = 0;
                printf("DEBUG: file hash: %s\n", file_pass);

                if (cmp(username, file_user)) {
                    user_found = true;
                    if (cmp(hashed_password, file_pass))
                        login_success = true;
                    break;
                }
            }
            fclose(usersFile);
        }

        if (!user_found) {
            puts("User not found.");
            new_user = true;
            usersFile = fopen(USERS_FILE, "a");
            if (usersFile == NULL) {
                puts("Error when opening users data file.");
                return 1;
            }

            fprintf(usersFile, "%s %s\n", username, hashed_password);
            fclose(usersFile);
            break;
        } else if (login_success) {
            new_user = false;
            break;
        } else
            puts("Invalid password.");
    } 
    shell_loop(username, new_user);
}