#include <stdio.h>
#include <string.h>
#include "auth_system.h"
#include "database.h"

// Function to authenticate user
int authenticate_user(const char *reg_number, const char *password) {
    const char *sql = "SELECT registration_number FROM users WHERE registration_number = ? AND password = ?;";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, reg_number, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, password, -1, SQLITE_STATIC);

    int found = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        found = 1;
    }
    sqlite3_finalize(stmt);
    return found;
}