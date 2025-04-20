#include "auth_functions.h"
#include <stdio.h>
#include "user_management.h" // Use database-based user management

void handle_registration() {
    register_user();  // Call the register_user function
}

void handle_login() {
    char name[MAX_LENGTH];
    if (login_user(name)) {
        printf("Login successful!\n");
        printf("Welcome %s\n", name);
    } else {
        printf("Invalid credentials.\n");
    }
}
