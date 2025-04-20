#include "auth_functions.h"
#include <stdio.h>
#include "user_management.h"

void handle_registration() {
    printf("Please fill in the following details to register.\n");
    register_user();
}

void handle_login() {
    char first_name[MAX_LENGTH], last_name[MAX_LENGTH], role[MAX_LENGTH];
    printf("Please enter your login credentials.\n");
    if (login_user(first_name, last_name, role)) {
        printf("Login successful!\n");
        printf("Welcome %s %s\n", first_name, last_name);
    } else {
        printf("Invalid credentials. Please try again.\n");
    }
}
