#ifndef SECURITY_UTILS_H
#define SECURITY_UTILS_H

#include <stdbool.h>

#define HASH_SIZE 65
#define SALT_SIZE 33
#define MAX_LOGIN_ATTEMPTS 5
#define LOCKOUT_DURATION 300 // seconds

void generate_salt(char *salt, int length);
void hash_password(const char *password, const char *salt, char *hash);
bool verify_password(const char *password, const char *salt, const char *hash);
int check_login_attempts(const char *reg_number);
void record_failed_login(const char *reg_number);
void reset_login_attempts(const char *reg_number);
bool is_account_locked(const char *reg_number);

#endif
