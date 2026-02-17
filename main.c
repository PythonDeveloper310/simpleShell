#include "shell.h"
// #include <corecrt.h>
// #include <stdio.h>
// #include <string.h>

// #define MAX_SIZE 64
// #define USERS_FILE "shusers.txt"

int main(void)
{
    // char username[MAX_SIZE], password[MAX_SIZE];
    // FILE *usersFile = fopen(USERS_FILE, "a");

    // puts("simpleShell Login System.");
    // puts("Username: ");
    // scanf_s("%63s", username);

    // puts("Password: ");
    // scanf_s("%63s", password);
    
    shell_loop("user");
}