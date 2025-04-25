/**
 * attendance.c
 * Functions for marking and viewing attendance in the Copa (CUK) system.
 * Used by students and lecs to record and check attendance per session.
 */

#include "attendance.h"
#include "../include/database.h"
#include <stdio.h>
#include <string.h>

/**
 * @brief Marks attendance for a student for a specific date.
 * @param registration_number The student's registration number.
 * @param date The date of attendance (YYYY-MM-DD).
 * @param status 1 for present, 0 for absent.
 */
void mark_attendance(const char *registration_number, const char *date, int status) {
    const char *sql = "INSERT INTO attendance (registration_number, date, status) VALUES (?, ?, ?);";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, registration_number, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, date, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, status);

    if (sqlite3_step(stmt) == SQLITE_DONE) {
        printf("Attendance marked successfully for %s on %s.\n", registration_number, date);
    } else {
        printf("Error marking attendance.\n");
    }

    sqlite3_finalize(stmt);
}

/**
 * @brief Views all attendance records for a specific student.
 * @param registration_number The student's registration number.
 */
void view_attendance(const char *registration_number) {
    const char *sql = "SELECT date, status FROM attendance WHERE registration_number = ?;";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, registration_number, -1, SQLITE_STATIC);

    printf("Attendance for %s:\n", registration_number);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const char *date = (const char *)sqlite3_column_text(stmt, 0);
        int status = sqlite3_column_int(stmt, 1);
        printf("Date: %s, Status: %s\n", date, (status == 1 ? "Present" : "Absent"));
    }

    sqlite3_finalize(stmt);
}
