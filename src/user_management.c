#include "../include/user_management.h"
#include "../include/database.h"
#include "../include/security_utils.h"
#include "../include/utils.h"
#include "../include/error_handling.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

    printf("Enter your password: ");
    fflush(stdout); // Ensure prompt is shown before input
    read_password(password, MAX_LENGTH);

    printf("Confirm your password: ");
    fflush(stdout);
    read_password(confirm_password, MAX_LENGTH);

    if (strcmp(password, confirm_password) != 0) {
        print_error("Passwords do not match. Registration cancelled.");
        return;
    }

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
        printf("Registration successful! You can now log in.\n");
    } else {
        print_error("Could not register user. Please try again.");
    }

    sqlite3_finalize(insert_stmt);
}

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

    printf("Password: ");
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

void reset_password() {
    char reg_number[MAX_LENGTH], new_password[MAX_LENGTH], confirm_password[MAX_LENGTH];
    char salt[SALT_SIZE], hash[HASH_SIZE];
    printf("Enter your registration number: ");
    fgets(reg_number, MAX_LENGTH, stdin);
    reg_number[strcspn(reg_number, "\n")] = '\0';

    printf("Enter your new password: ");
    fflush(stdout);
    read_password(new_password, MAX_LENGTH);

    printf("Confirm your new password: ");
    fflush(stdout);
    read_password(confirm_password, MAX_LENGTH);

    if (strcmp(new_password, confirm_password) != 0) {
        print_error("Passwords do not match. Password reset cancelled.");
        return;
    }

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
        printf("Password reset successful! You can now log in with your new password.\n");
    } else {
        print_error("Registration number not found. Password not changed.");
    }

    sqlite3_finalize(stmt);
}

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

void remove_student() {
    char reg_number[MAX_LENGTH], admin_password[MAX_LENGTH];
    printf("Enter the registration number of the student to remove: ");
    fgets(reg_number, MAX_LENGTH, stdin);
    reg_number[strcspn(reg_number, "\n")] = '\0';

    printf("Enter admin password to confirm: ");
    fgets(admin_password, MAX_LENGTH, stdin);
    admin_password[strcspn(admin_password, "\n")] = '\0';

    // Check admin password
    const char *admin_sql = "SELECT password FROM users WHERE role = 'admin' LIMIT 1;";
    sqlite3_stmt *admin_stmt;
    if (sqlite3_prepare_v2(db, admin_sql, -1, &admin_stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Error preparing statement (admin_sql): %s\n", sqlite3_errmsg(db));
        return;
    }
    int confirmed = 0;
    if (sqlite3_step(admin_stmt) == SQLITE_ROW) {
        const char *real_pass = (const char *)sqlite3_column_text(admin_stmt, 0);
        if (strcmp(real_pass, admin_password) == 0) confirmed = 1;
    }
    sqlite3_finalize(admin_stmt);

    if (!confirmed) {
        printf("Admin password incorrect. Student not removed.\n");
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
        printf("Student removed successfully.\n");
    } else {
        printf("Error: Could not remove student. Check registration number.\n");
    }
    sqlite3_finalize(del_stmt);
}
