#include "../include/user_data.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Save user data to the file
void save_user(User user) {
    FILE *file = fopen("./bin/user_data.txt", "a");  // Open file in append mode
    if (file == NULL) {
        perror("Error opening file");
        exit(1);
    }
    fprintf(file, "%s %s\n", user.registration_number, user.password);
    fclose(file);
}

// Function to register a user
void register_user() {
    User new_user;
    printf("Enter your registration number: ");
    fgets(new_user.registration_number, MAX_LENGTH, stdin);
    new_user.registration_number[strcspn(new_user.registration_number, "\n")] = '\0'; // Remove newline character

    printf("Enter your password: ");
    fgets(new_user.password, MAX_LENGTH, stdin);
    new_user.password[strcspn(new_user.password, "\n")] = '\0'; // Remove newline character

    save_user(new_user);
    printf("Registration successful!\n");
}

// Function to check user credentials during login
// Now accepts a name buffer to set on successful login
int login_user(char *name) {
    char reg_number[MAX_LENGTH], password[MAX_LENGTH];
    User user;
    int found = 0;

    printf("Enter your registration number: ");
    fgets(reg_number, MAX_LENGTH, stdin);
    reg_number[strcspn(reg_number, "\n")] = '\0';

    printf("Enter your password: ");
    fgets(password, MAX_LENGTH, stdin);
    password[strcspn(password, "\n")] = '\0';

    FILE *file = fopen("./bin/user_data.txt", "r");
    if (file == NULL) {
        perror("Error opening file");
        return 0;
    }

    while (fscanf(file, "%s %s", user.registration_number, user.password) != EOF) {
        if (strcmp(reg_number, user.registration_number) == 0 && strcmp(password, user.password) == 0) {
            found = 1;
            if (name) {
                strncpy(name, user.registration_number, MAX_LENGTH);
                name[MAX_LENGTH - 1] = '\0';
            }
            break;
        }
    }

    fclose(file);
    return found;
}