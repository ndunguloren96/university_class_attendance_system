#include "../include/user_management.h"
#include "../include/database.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void register_user() {
    char reg_number[MAX_LENGTH], password[MAX_LENGTH];
    printf("Enter your registration number: ");
    fgets(reg_number, MAX_LENGTH, stdin);
    reg_number[strcspn(reg_number, "\n")] = '\0';

    printf("Enter your password: ");
    fgets(password, MAX_LENGTH, stdin);
    password[strcspn(password, "\n")] = '\0';

    const char *sql = "INSERT INTO users (registration_number, password) VALUES (?, ?);";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, reg_number, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, password, -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_DONE) {
        printf("Registration successful!\n");
    } else {
        printf("Error: Registration number already exists.\n");
    }

    sqlite3_finalize(stmt);
}

int login_user(char *name) {
    char reg_number[MAX_LENGTH], password[MAX_LENGTH];
    printf("Enter your registration number: ");
    fgets(reg_number, MAX_LENGTH, stdin);
    reg_number[strcspn(reg_number, "\n")] = '\0';

    printf("Enter your password: ");
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
    }

    sqlite3_finalize(stmt);
    return found;
}

void reset_password() {
    char reg_number[MAX_LENGTH], new_password[MAX_LENGTH];
    printf("Enter your registration number: ");
    fgets(reg_number, MAX_LENGTH, stdin);
    reg_number[strcspn(reg_number, "\n")] = '\0';

    printf("Enter your new password: ");
    fgets(new_password, MAX_LENGTH, stdin);
    new_password[strcspn(new_password, "\n")] = '\0';

    const char *sql = "UPDATE users SET password = ? WHERE registration_number = ?;";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, new_password, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, reg_number, -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_DONE) {
        printf("Password reset successful!\n");
    } else {
        printf("Error: Registration number not found.\n");
    }

    sqlite3_finalize(stmt);
}
