#include "student_report.h"
#include <stdio.h>
#include <string.h>
#include "../include/database.h"
#include "../include/user_management.h"

void generate_student_report(const char* registration_number) {
    printf("Generating report for student: %s\n", registration_number);
    // Implement actual report logic here
}

void view_all_students() {
    const char *sql = "SELECT registration_number FROM users;";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Error preparing statement (view_all_students): %s\n", sqlite3_errmsg(db));
        return;
    }
    printf("All registered students:\n");
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const char *reg = (const char *)sqlite3_column_text(stmt, 0);
        printf("Registration Number: %s\n", reg);
    }
    sqlite3_finalize(stmt);
}

void view_all_attendance() {
    const char *sql = "SELECT registration_number, date, status FROM attendance;";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Error preparing statement (view_all_attendance): %s\n", sqlite3_errmsg(db));
        return;
    }
    printf("All attendance records:\n");
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const char *reg = (const char *)sqlite3_column_text(stmt, 0);
        const char *date = (const char *)sqlite3_column_text(stmt, 1);
        int status = sqlite3_column_int(stmt, 2);
        printf("Registration Number: %s, Date: %s, Status: %s\n", reg, date, (status == 1 ? "Present" : "Absent"));
    }
    sqlite3_finalize(stmt);
}
