#include "../include/user_management.h"
#include "../include/database.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void register_user() {
    char reg_number[MAX_LENGTH], password[MAX_LENGTH], confirm_password[MAX_LENGTH];
    printf("Enter your registration number: ");
    fgets(reg_number, MAX_LENGTH, stdin);
    reg_number[strcspn(reg_number, "\n")] = '\0';

    // Check if the registration number already exists
    const char *check_sql = "SELECT COUNT(*) FROM users WHERE registration_number = ?;";
    sqlite3_stmt *check_stmt;
    if (sqlite3_prepare_v2(db, check_sql, -1, &check_stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Error preparing statement (check_sql): %s\n", sqlite3_errmsg(db));
        return;
    }
    sqlite3_bind_text(check_stmt, 1, reg_number, -1, SQLITE_STATIC);

    int exists = 0;
    if (sqlite3_step(check_stmt) == SQLITE_ROW) {
        exists = sqlite3_column_int(check_stmt, 0);
    }
    sqlite3_finalize(check_stmt);

    if (exists) {
        printf("Error: Registration number already exists. Please try a different one.\n");
        return;
    }

    printf("Enter your password: ");
    fgets(password, MAX_LENGTH, stdin);
    password[strcspn(password, "\n")] = '\0';

    printf("Confirm your password: ");
    fgets(confirm_password, MAX_LENGTH, stdin);
    confirm_password[strcspn(confirm_password, "\n")] = '\0';

    if (strcmp(password, confirm_password) != 0) {
        printf("Error: Passwords do not match. Registration cancelled.\n");
        return;
    }

    const char *insert_sql = "INSERT INTO users (registration_number, password) VALUES (?, ?);";
    sqlite3_stmt *insert_stmt;
    if (sqlite3_prepare_v2(db, insert_sql, -1, &insert_stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Error preparing statement (insert_sql): %s\n", sqlite3_errmsg(db));
        return;
    }
    sqlite3_bind_text(insert_stmt, 1, reg_number, -1, SQLITE_STATIC);
    sqlite3_bind_text(insert_stmt, 2, password, -1, SQLITE_STATIC);

    if (sqlite3_step(insert_stmt) == SQLITE_DONE) {
        printf("Registration successful! You can now log in.\n");
    } else {
        fprintf(stderr, "Error executing statement (insert_sql): %s\n", sqlite3_errmsg(db));
        printf("Error: Could not register user. Please try again.\n");
    }

    sqlite3_finalize(insert_stmt);
}

int login_user(char *name) {
    char reg_number[MAX_LENGTH], password[MAX_LENGTH];
    printf("Registration number: ");
    fgets(reg_number, MAX_LENGTH, stdin);
    reg_number[strcspn(reg_number, "\n")] = '\0';

    printf("Password: ");
    fgets(password, MAX_LENGTH, stdin);
    password[strcspn(password, "\n")] = '\0';

    const char *sql = "SELECT registration_number FROM users WHERE registration_number = ? AND password = ?;";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, reg_number, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, password, -1, SQLITE_STATIC);

    int found = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        found = 1;
        strncpy(name, (const char *)sqlite3_column_text(stmt, 0), MAX_LENGTH);
        name[MAX_LENGTH - 1] = '\0'; // Ensure null-termination
    }

    sqlite3_finalize(stmt);

    return found;
}

void reset_password() {
    char reg_number[MAX_LENGTH], new_password[MAX_LENGTH], confirm_password[MAX_LENGTH];
    printf("Enter your registration number: ");
    fgets(reg_number, MAX_LENGTH, stdin);
    reg_number[strcspn(reg_number, "\n")] = '\0';

    printf("Enter your new password: ");
    fgets(new_password, MAX_LENGTH, stdin);
    new_password[strcspn(new_password, "\n")] = '\0';

    printf("Confirm your new password: ");
    fgets(confirm_password, MAX_LENGTH, stdin);
    confirm_password[strcspn(confirm_password, "\n")] = '\0';

    if (strcmp(new_password, confirm_password) != 0) {
        printf("Error: Passwords do not match. Password reset cancelled.\n");
        return;
    }

    const char *sql = "UPDATE users SET password = ? WHERE registration_number = ?;";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, new_password, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, reg_number, -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_DONE) {
        printf("Password reset successful! You can now log in with your new password.\n");
    } else {
        printf("Error: Registration number not found. Password not changed.\n");
    }

    sqlite3_finalize(stmt);
}
