#include "attendance.h"
#include "error_handling.h"
#include <sqlite3.h>
#include <stdio.h>

extern sqlite3 *db;

void view_all_attendance() {
    const char *query = "SELECT s.student_id, s.name, u.unit_code, u.unit_name, a.attendance_date "
                        "FROM attendance a "
                        "JOIN students s ON a.student_id = s.student_id "
                        "JOIN units u ON a.unit_id = u.unit_id";

    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        log_error(sqlite3_errmsg(db));
        return;
    }

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        int student_id = sqlite3_column_int(stmt, 0);
        const unsigned char *name = sqlite3_column_text(stmt, 1);
        const unsigned char *unit_code = sqlite3_column_text(stmt, 2);
        const unsigned char *unit_name = sqlite3_column_text(stmt, 3);
        const unsigned char *attendance_date = sqlite3_column_text(stmt, 4);

        printf("Student ID: %d, Name: %s, Unit Code: %s, Unit Name: %s, Attendance Date: %s\n",
               student_id, name, unit_code, unit_name, attendance_date);
    }

    if (rc != SQLITE_DONE) {
        log_error(sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);
}