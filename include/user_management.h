#ifndef USER_MANAGEMENT_H
#define USER_MANAGEMENT_H

#include "user_data.h"

// Function declarations for user management
void register_user();
int login_user(char *name);
void reset_password();

#endif // USER_MANAGEMENT_H
