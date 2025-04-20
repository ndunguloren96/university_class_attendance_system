#include "database.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

sqlite3 *db;

int initialize_database() {
    int rc = sqlite3_open("university_attendance.db", &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        return 0;
    }

    const char *user_table = "CREATE TABLE IF NOT EXISTS users ("
                             "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                             "registration_number TEXT UNIQUE NOT NULL, "
                             "password TEXT NOT NULL, "
                             "first_name TEXT NOT NULL, "
                             "last_name TEXT NOT NULL, "
                             "email TEXT UNIQUE NOT NULL, "
                             "role TEXT NOT NULL CHECK(role IN ('student','instructor','admin')), "
                             "created_at DATETIME DEFAULT CURRENT_TIMESTAMP);";

    const char *attendance_table = "CREATE TABLE IF NOT EXISTS attendance ("
                                   "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                                   "registration_number TEXT NOT NULL, "
                                   "date TEXT NOT NULL, "
                                   "status INTEGER NOT NULL, "
                                   "FOREIGN KEY (registration_number) REFERENCES users (registration_number));";

    char *err_msg = NULL;
    rc = sqlite3_exec(db, user_table, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error (users table): %s\n", err_msg);
        sqlite3_free(err_msg);
        return 0;
    }

    rc = sqlite3_exec(db, attendance_table, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error (attendance table): %s\n", err_msg);
        sqlite3_free(err_msg);
        return 0;
    }

    return 1;
}

void close_database() {
    sqlite3_close(db);
}
