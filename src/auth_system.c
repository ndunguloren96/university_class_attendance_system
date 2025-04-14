#include <stdio.h>
#include <string.h> // Include string.h for strcmp function
#include "auth_system.h"

// Function to authenticate user
int authenticate_user(const char *reg_number, const char *password) {
    User user;
    FILE *file = fopen("./bin/user_data.txt", "r");
    if (file == NULL) {
        perror("Error opening file");
        return 0;
    }

    while (fscanf(file, "%s %s", user.registration_number, user.password) != EOF) {
        if (strcmp(reg_number, user.registration_number) == 0 && strcmp(password, user.password) == 0) {
            fclose(file);
            return 1;
        }
    }

    fclose(file);
    return 0;
}