#include <cstring>
#include "error_handling.h"
#include "../include/security_utils.h"
#include "../include/database.h"

void remove_student() {
    char reg_number[50], entered_password[50];
    printf("Enter the registration number of the student to remove: ");
    fgets(reg_number, sizeof(reg_number), stdin);
    reg_number[strcspn(reg_number, "\n")] = '\0';

    printf("Enter admin password to confirm: ");
    fgets(entered_password, sizeof(entered_password), stdin);
    entered_password[strcspn(entered_password, "\n")] = '\0';

    // Fetch the admin's hashed password and salt from the database
    const char *admin_sql = "SELECT password, salt FROM users WHERE role = 'admin' LIMIT 1;";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, admin_sql, -1, &stmt, NULL) != SQLITE_OK) {
        print_error("Failed to prepare SQL statement.");
        return;
    }

    char stored_hash[HASH_SIZE] = {0};
    char stored_salt[SALT_SIZE] = {0};
    int verified = 0;

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        strncpy(stored_hash, (const char *)sqlite3_column_text(stmt, 0), HASH_SIZE);
        strncpy(stored_salt, (const char *)sqlite3_column_text(stmt, 1), SALT_SIZE);
        verified = verify_password(entered_password, stored_salt, stored_hash);
    }
    sqlite3_finalize(stmt);

    if (!verified) {
        print_error("Admin password incorrect. Student not removed.");
        return;
    }

    // Proceed to remove the student
    const char *del_sql = "DELETE FROM users WHERE registration_number = ? AND role = 'student';";
    if (sqlite3_prepare_v2(db, del_sql, -1, &stmt, NULL) != SQLITE_OK) {
        print_error("Failed to prepare SQL statement for deleting student.");
        return;
    }
    sqlite3_bind_text(stmt, 1, reg_number, -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_DONE && sqlite3_changes(db) > 0) {
        printf("Student removed successfully.\n");
    } else {
        print_error("Error: Could not remove student. Check registration number.");
    }
    sqlite3_finalize(stmt);
}