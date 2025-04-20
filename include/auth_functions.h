#ifndef AUTH_FUNCTIONS_H
#define AUTH_FUNCTIONS_H

#include "auth_system.h"
#include "user_management.h" // Use database-based user management

// Function declarations related to function operations for authentication
void clear_input_buffer();
void handle_registration();
void handle_login();

#endif // AUTH_FUNCTIONS_H
