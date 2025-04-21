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
                             "role TEXT NOT NULL CHECK(role IN ('student','lecturer','admin')), "
                             "created_at DATETIME DEFAULT CURRENT_TIMESTAMP);";

    const char *unit_table = "CREATE TABLE IF NOT EXISTS units ("
                             "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                             "unit_code TEXT UNIQUE NOT NULL, "
                             "unit_name TEXT NOT NULL, "
                             "lecturer_id INTEGER, "
                             "FOREIGN KEY (lecturer_id) REFERENCES users(id));";

    const char *enrollment_table = "CREATE TABLE IF NOT EXISTS enrollments ("
                                   "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                                   "user_id INTEGER NOT NULL, "
                                   "unit_id INTEGER NOT NULL, "
                                   "enrolled_at DATETIME DEFAULT CURRENT_TIMESTAMP, "
                                   "FOREIGN KEY (user_id) REFERENCES users(id), "
                                   "FOREIGN KEY (unit_id) REFERENCES units(id), "
                                   "UNIQUE(user_id, unit_id));";

    const char *session_table = "CREATE TABLE IF NOT EXISTS sessions ("
                                "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                                "unit_id INTEGER NOT NULL, "
                                "session_code TEXT UNIQUE NOT NULL, "
                                "session_date TEXT NOT NULL, "
                                "topic TEXT, "
                                "created_at DATETIME DEFAULT CURRENT_TIMESTAMP, "
                                "FOREIGN KEY (unit_id) REFERENCES units(id));";

    const char *attendance_table = "CREATE TABLE IF NOT EXISTS attendance ("
                                   "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                                   "user_id INTEGER NOT NULL, "
                                   "session_id INTEGER NOT NULL, "
                                   "status INTEGER NOT NULL, "
                                   "marked_at DATETIME DEFAULT CURRENT_TIMESTAMP, "
                                   "FOREIGN KEY (user_id) REFERENCES users(id), "
                                   "FOREIGN KEY (session_id) REFERENCES sessions(id), "
                                   "UNIQUE(user_id, session_id));";

    char *err_msg = NULL;
    rc = sqlite3_exec(db, user_table, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error (users table): %s\n", err_msg);
        sqlite3_free(err_msg);
        return 0;
    }
    rc = sqlite3_exec(db, unit_table, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error (units table): %s\n", err_msg);
        sqlite3_free(err_msg);
        return 0;
    }
    rc = sqlite3_exec(db, enrollment_table, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error (enrollments table): %s\n", err_msg);
        sqlite3_free(err_msg);
        return 0;
    }
    rc = sqlite3_exec(db, session_table, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error (sessions table): %s\n", err_msg);
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
