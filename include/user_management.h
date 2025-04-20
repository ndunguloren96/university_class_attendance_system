#ifndef USER_MANAGEMENT_H
#define USER_MANAGEMENT_H

#define MAX_LENGTH 100

typedef enum { ROLE_STUDENT, ROLE_INSTRUCTOR, ROLE_ADMIN } UserRole;

typedef struct {
    int id;
    char registration_number[MAX_LENGTH];
    char first_name[MAX_LENGTH];
    char last_name[MAX_LENGTH];
    char email[MAX_LENGTH];
    char role[MAX_LENGTH];
} User;

void register_user();
int login_user(char *first_name, char *last_name, char *role);
void reset_password();
void list_users(const char *role_filter); // List users by role

#endif // USER_MANAGEMENT_H
