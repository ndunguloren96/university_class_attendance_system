#include "auth_functions.h"
#include <stdio.h>

void clear_input_buffer() {
    while (getchar() != '\n');  // Flush input buffer
}

void handle_registration() {
    register_user();  // Call the register_user function
}

void handle_login() {
    if (login_user()) {
        printf("Login successful!\n");
    } else {
        printf("Invalid credentials.\n");
    }
}
