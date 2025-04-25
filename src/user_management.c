/**
 * user_management.c
 * Handles user registration, login, password reset, and admin-only student removal for the Copa (CUK) system.
 * All user data is securely stored and checked in the SQLite database.
 * Passwords are hashed and salted for security.
 */

#include "../include/user_management.h"
#include "../include/database.h"
#include "../include/security_utils.h"
#include "../include/utils.h"
#include "../include/error_handling.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/**
 * @brief Checks if a password is strong (at least 6 chars, 1 uppercase, 1 lowercase, 1 digit).
 * @param password The password string to check.
 * @return 1 if strong, 0 otherwise.
 */
int is_strong_password(const char *password) {
    int has_upper = 0, has_lower = 0, has_digit = 0;
    if (strlen(password) < 6) return 0;
    for (const char *p = password; *p; ++p) {
        if (*p >= 'A' && *p <= 'Z') has_upper = 1;
        else if (*p >= 'a' && *p <= 'z') has_lower = 1;
        else if (*p >= '0' && *p <= '9') has_digit = 1;
    }
    return has_upper && has_lower && has_digit;
}

/**
 * @brief Prints a success message in green.
 * @param msg The message to print.
 */
void print_success(const char *msg) {
    printf("\033[1;32m%s\033[0m\n", msg);
}

/**
 * @brief Registers a new user (student, instructor, or admin).
 * Prompts for all required fields, checks for uniqueness, and hashes the password.
 * Handles password strength and confirmation.
 */
void register_user() {
    char reg_number[MAX_LENGTH], password[MAX_LENGTH], confirm_password[MAX_LENGTH];
    char first_name[MAX_LENGTH], last_name[MAX_LENGTH], email[MAX_LENGTH], role[MAX_LENGTH] = "student";
    char salt[SALT_SIZE], hash[HASH_SIZE];
    printf("Enter your first name: ");
    fgets(first_name, MAX_LENGTH, stdin);
    first_name[strcspn(first_name, "\n")] = '\0';

    printf("Enter your last name: ");
    fgets(last_name, MAX_LENGTH, stdin);
    last_name[strcspn(last_name, "\n")] = '\0';

    printf("Enter your registration number: ");
    fgets(reg_number, MAX_LENGTH, stdin);
    reg_number[strcspn(reg_number, "\n")] = '\0';

    printf("Enter your email: ");
    fgets(email, MAX_LENGTH, stdin);
    email[strcspn(email, "\n")] = '\0';

    printf("Are you registering as a student, instructor, or admin? [student]: ");
    fgets(role, MAX_LENGTH, stdin);
    role[strcspn(role, "\n")] = '\0';
    if (strlen(role) == 0) strcpy(role, "student");

    // Check for unique registration number and email
    const char *check_sql = "SELECT COUNT(*) FROM users WHERE registration_number = ? OR email = ?;";
    sqlite3_stmt *check_stmt;
    if (sqlite3_prepare_v2(db, check_sql, -1, &check_stmt, NULL) != SQLITE_OK) {
        print_error(sqlite3_errmsg(db));
        return;
    }
    sqlite3_bind_text(check_stmt, 1, reg_number, -1, SQLITE_STATIC);
    sqlite3_bind_text(check_stmt, 2, email, -1, SQLITE_STATIC);

    int exists = 0;
    if (sqlite3_step(check_stmt) == SQLITE_ROW) {
        exists = sqlite3_column_int(check_stmt, 0);
    }
    sqlite3_finalize(check_stmt);

    if (exists) {
        print_error("Registration number or email already exists. Please try a different one.");
        return;
    }

    int strong = 0;
    do {
        fflush(stdout);
        read_password(password, MAX_LENGTH);

        printf("Confirm password: ");
        fflush(stdout);
        read_password(confirm_password, MAX_LENGTH);

        if (strcmp(password, confirm_password) != 0) {
            print_error("Passwords do not match. Registration cancelled.");
            return;
        }

        strong = is_strong_password(password);
        if (!strong && strcmp(role, "admin") == 0) {
            printf("Warning: Admin password is not strong (must be at least 6 chars, 1 uppercase, 1 lowercase, 1 digit).\n");
            printf("Continue with weak password? (y/n): ");
            char yn[8];
            fgets(yn, sizeof(yn), stdin);
            if (yn[0] != 'y' && yn[0] != 'Y') continue;
        } else if (!strong) {
            printf("Warning: Password is not strong (must be at least 6 chars, 1 uppercase, 1 lowercase, 1 digit).\n");
            printf("Continue with weak password? (y/n): ");
            char yn[8];
            fgets(yn, sizeof(yn), stdin);
            if (yn[0] != 'y' && yn[0] != 'Y') continue;
        }
        break;
    } while (1);

    generate_salt(salt, SALT_SIZE);
    hash_password(password, salt, hash);

    const char *insert_sql = "INSERT INTO users (registration_number, password, salt, first_name, last_name, email, role) VALUES (?, ?, ?, ?, ?, ?, ?);";
    sqlite3_stmt *insert_stmt;
    if (sqlite3_prepare_v2(db, insert_sql, -1, &insert_stmt, NULL) != SQLITE_OK) {
        print_error(sqlite3_errmsg(db));
        return;
    }
    sqlite3_bind_text(insert_stmt, 1, reg_number, -1, SQLITE_STATIC);
    sqlite3_bind_text(insert_stmt, 2, hash, -1, SQLITE_STATIC);
    sqlite3_bind_text(insert_stmt, 3, salt, -1, SQLITE_STATIC);
    sqlite3_bind_text(insert_stmt, 4, first_name, -1, SQLITE_STATIC);
    sqlite3_bind_text(insert_stmt, 5, last_name, -1, SQLITE_STATIC);
    sqlite3_bind_text(insert_stmt, 6, email, -1, SQLITE_STATIC);
    sqlite3_bind_text(insert_stmt, 7, role, -1, SQLITE_STATIC);

    if (sqlite3_step(insert_stmt) == SQLITE_DONE) {
        print_success("Registration successful! You can now log in.");
    } else {
        print_error("Could not register user. Please try again.");
    }

    sqlite3_finalize(insert_stmt);
}

/**
 * @brief Handles user login.
 * Checks registration number and password, verifies password hash, and returns user info.
 * Locks account after too many failed attempts.
 * @param first_name Output buffer for user's first name.
 * @param last_name Output buffer for user's last name.
 * @param role Output buffer for user's role.
 * @return 1 if login successful, 0 otherwise.
 */
int login_user(char *first_name, char *last_name, char *role) {
    char reg_number[MAX_LENGTH], password[MAX_LENGTH];
    char hash[HASH_SIZE], salt[SALT_SIZE];
    printf("Registration number: ");
    fflush(stdout);
    fgets(reg_number, MAX_LENGTH, stdin);
    reg_number[strcspn(reg_number, "\n")] = '\0';

    if (is_account_locked(reg_number)) {
        print_error("Account is locked due to too many failed login attempts. Please try again later.");
        return 0;
    }

    fflush(stdout);
    read_password(password, MAX_LENGTH);

    const char *sql = "SELECT first_name, last_name, role, password, salt FROM users WHERE registration_number = ?;";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        print_error(sqlite3_errmsg(db));
        return 0;
    }
    sqlite3_bind_text(stmt, 1, reg_number, -1, SQLITE_STATIC);

    int found = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        strncpy(first_name, (const char *)sqlite3_column_text(stmt, 0), MAX_LENGTH);
        strncpy(last_name, (const char *)sqlite3_column_text(stmt, 1), MAX_LENGTH);
        strncpy(role, (const char *)sqlite3_column_text(stmt, 2), MAX_LENGTH);
        strncpy(hash, (const char *)sqlite3_column_text(stmt, 3), HASH_SIZE);
        strncpy(salt, (const char *)sqlite3_column_text(stmt, 4), SALT_SIZE);
        first_name[MAX_LENGTH-1] = last_name[MAX_LENGTH-1] = role[MAX_LENGTH-1] = '\0';
        if (verify_password(password, salt, hash)) {
            found = 1;
            reset_login_attempts(reg_number);
        } else {
            record_failed_login(reg_number);
            print_error("Invalid credentials.");
        }
    } else {
        record_failed_login(reg_number);
        print_error("Invalid credentials.");
    }

    sqlite3_finalize(stmt);

    return found;
}

/**
 * @brief Allows a user to reset their password after verifying registration number and email.
 * Handles password strength and confirmation.
 */
void reset_password() {
    char reg_number[MAX_LENGTH], email[MAX_LENGTH], new_password[MAX_LENGTH], confirm_password[MAX_LENGTH];
    char salt[SALT_SIZE], hash[HASH_SIZE];
    printf("Enter your registration number: ");
    fgets(reg_number, MAX_LENGTH, stdin);
    reg_number[strcspn(reg_number, "\n")] = '\0';

    printf("Enter your email: ");
    fgets(email, MAX_LENGTH, stdin);
    email[strcspn(email, "\n")] = '\0';

    // Check user exists and email matches
    const char *check_sql = "SELECT email FROM users WHERE registration_number = ?;";
    sqlite3_stmt *check_stmt;
    if (sqlite3_prepare_v2(db, check_sql, -1, &check_stmt, NULL) != SQLITE_OK) {
        print_error(sqlite3_errmsg(db));
        return;
    }
    sqlite3_bind_text(check_stmt, 1, reg_number, -1, SQLITE_STATIC);
    int found = 0;
    char db_email[MAX_LENGTH];
    if (sqlite3_step(check_stmt) == SQLITE_ROW) {
        strncpy(db_email, (const char *)sqlite3_column_text(check_stmt, 0), MAX_LENGTH-1);
        db_email[MAX_LENGTH-1] = '\0';
        if (strcmp(email, db_email) == 0) found = 1;
    }
    sqlite3_finalize(check_stmt);

    if (!found) {
        print_error("Registration number and email do not match any user.");
        return;
    }

    int strong = 0;
    do {
        fflush(stdout);
        read_password(new_password, MAX_LENGTH);

        printf("Confirm new password: ");
        fflush(stdout);
        read_password(confirm_password, MAX_LENGTH);

        if (strcmp(new_password, confirm_password) != 0) {
            print_error("Passwords do not match. Password reset cancelled.");
            return;
        }

        strong = is_strong_password(new_password);
        if (!strong) {
            printf("Warning: Password is not strong (must be at least 6 chars, 1 uppercase, 1 lowercase, 1 digit).\n");
            printf("Continue with weak password? (y/n): ");
            char yn[8];
            fgets(yn, sizeof(yn), stdin);
            if (yn[0] != 'y' && yn[0] != 'Y') continue;
        }
        break;
    } while (1);

    generate_salt(salt, SALT_SIZE);
    hash_password(new_password, salt, hash);

    const char *sql = "UPDATE users SET password = ?, salt = ? WHERE registration_number = ?;";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        print_error(sqlite3_errmsg(db));
        return;
    }
    sqlite3_bind_text(stmt, 1, hash, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, salt, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, reg_number, -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_DONE) {
        print_success("Password reset successful! You can now log in with your new password.");
    } else {
        print_error("Registration number not found. Password not changed.");
    }

    sqlite3_finalize(stmt);
}

/**
 * @brief Lists all users of a given role (e.g., students, instructors, admins).
 * @param role_filter The role to filter by.
 */
void list_users(const char *role_filter) {
    const char *sql = "SELECT registration_number, first_name, last_name, email, role FROM users WHERE role = ?;";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Error preparing statement (list_users): %s\n", sqlite3_errmsg(db));
        return;
    }
    sqlite3_bind_text(stmt, 1, role_filter, -1, SQLITE_STATIC);
    printf("%-15s | %-20s | %-30s | %-10s\n", "Reg Number", "Name", "Email", "Role");
    printf("-------------------------------------------------------------------------------\n");
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const char *reg = (const char *)sqlite3_column_text(stmt, 0);
        const char *fname = (const char *)sqlite3_column_text(stmt, 1);
        const char *lname = (const char *)sqlite3_column_text(stmt, 2);
        const char *email = (const char *)sqlite3_column_text(stmt, 3);
        const char *role = (const char *)sqlite3_column_text(stmt, 4);
        char fullname[2 * MAX_LENGTH];
        snprintf(fullname, sizeof(fullname), "%s %s", fname, lname);
        printf("%-15s | %-20s | %-30s | %-10s\n", reg, fullname, email, role);
    }
    sqlite3_finalize(stmt);
}

/**
 * @brief Removes a student from the system (admin only).
 * Prompts for admin password and verifies before deletion.
 */
void remove_student() {
    char reg_number[MAX_LENGTH], admin_password[MAX_LENGTH];
    printf("Enter the registration number of the student to remove: ");
    fgets(reg_number, MAX_LENGTH, stdin);
    reg_number[strcspn(reg_number, "\n")] = '\0';

    fflush(stdout);
    read_password(admin_password, MAX_LENGTH);

    // Fetch admin's hashed password and salt
    const char *admin_sql = "SELECT password, salt FROM users WHERE role = 'admin' LIMIT 1;";
    sqlite3_stmt *admin_stmt;
    if (sqlite3_prepare_v2(db, admin_sql, -1, &admin_stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Error preparing statement (admin_sql): %s\n", sqlite3_errmsg(db));
        return;
    }
    int confirmed = 0;
    char stored_hash[HASH_SIZE] = {0};
    char stored_salt[SALT_SIZE] = {0};
    if (sqlite3_step(admin_stmt) == SQLITE_ROW) {
        strncpy(stored_hash, (const char *)sqlite3_column_text(admin_stmt, 0), HASH_SIZE - 1);
        strncpy(stored_salt, (const char *)sqlite3_column_text(admin_stmt, 1), SALT_SIZE - 1);
        if (verify_password(admin_password, stored_salt, stored_hash)) {
            confirmed = 1;
        }
    }
    sqlite3_finalize(admin_stmt);

    if (!confirmed) {
        print_error("Admin password incorrect. Student not removed.");
        return;
    }

    // Remove student
    const char *del_sql = "DELETE FROM users WHERE registration_number = ? AND role = 'student';";
    sqlite3_stmt *del_stmt;
    if (sqlite3_prepare_v2(db, del_sql, -1, &del_stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Error preparing statement (del_sql): %s\n", sqlite3_errmsg(db));
        return;
    }
    sqlite3_bind_text(del_stmt, 1, reg_number, -1, SQLITE_STATIC);

    if (sqlite3_step(del_stmt) == SQLITE_DONE && sqlite3_changes(db) > 0) {
        print_success("Student removed successfully.");
    } else {
        print_error("Error: Could not remove student. Check registration number.");
    }
    sqlite3_finalize(del_stmt);
}
