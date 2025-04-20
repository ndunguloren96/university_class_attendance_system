#include "auth_functions.h"
#include <stdio.h>
#include "user_management.h"

void handle_registration() {
    printf("Please fill in the following details to register.\n");
    register_user();
}

void handle_login() {
    char name[MAX_LENGTH];
    printf("Please enter your login credentials.\n");
    if (login_user(name)) {
        printf("Login successful!\n");
        printf("Welcome %s\n", name);
    } else {
        printf("Invalid credentials. Please try again.\n");
    }
}
