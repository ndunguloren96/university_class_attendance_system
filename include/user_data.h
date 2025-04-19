#ifndef USER_DATA_H
#define USER_DATA_H

#define MAX_LENGTH 100

// Structure to store user data
typedef struct {
    char registration_number[MAX_LENGTH];
    char password[MAX_LENGTH];
} User;

// Function declarations related to user data operations
void save_user(User user);
int login_user(char *name); // Updated to accept name buffer
void register_user();

#endif // USER_DATA_H
