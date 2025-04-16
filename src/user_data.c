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