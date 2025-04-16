#include "../include/user_data.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Save user data to the file
void save_user(User user) {
    FILE *file = fopen("./bin/user_data.txt", "a");  // Adjust to your desired file path
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