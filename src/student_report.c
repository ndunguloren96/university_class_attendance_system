#include "student_report.h"
#include <stdio.h>
#include <string.h>
#include "../include/database.h"
#include "../include/user_management.h"

void generate_student_report(const char* registration_number) {
    const char *sql = "SELECT date, status FROM attendance WHERE registration_number = ? ORDER BY date;";
    sqlite3_stmt *stmt;
    int total = 0, present = 0, absent = 0;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Error preparing statement (generate_student_report): %s\n", sqlite3_errmsg(db));
        return;
    }
    sqlite3_bind_text(stmt, 1, registration_number, -1, SQLITE_STATIC);

    printf("Attendance Report for Student: %s\n", registration_number);
    printf("--------------------------------------------\n");
    printf("%-15s | %-10s\n", "Date", "Status");
    printf("--------------------------------------------\n");

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const char *date = (const char *)sqlite3_column_text(stmt, 0);
        int status = sqlite3_column_int(stmt, 1);
        printf("%-15s | %-10s\n", date, (status == 1 ? "Present" : "Absent"));
        total++;
        if (status == 1) present++;
        else absent++;
    }
    sqlite3_finalize(stmt);

    if (total == 0) {
        printf("No attendance records found for this student.\n");
    } else {
        printf("--------------------------------------------\n");
        printf("Total Sessions: %d\n", total);
        printf("Present: %d\n", present);
        printf("Absent: %d\n", absent);
        printf("Attendance Percentage: %.2f%%\n", total > 0 ? (present * 100.0 / total) : 0.0);
    }
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
