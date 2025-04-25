/**
 * student_report.c
 * Reporting functions for the Copa (CUK) Attendance System.
 * Includes per-student attendance reports, listing all students, and admin attendance overviews.
 */

#include "student_report.h"
#include <stdio.h>
#include <string.h>
#include "../include/database.h"
#include "../include/user_management.h"

/**
 * @brief Generates a detailed attendance report for a specific student.
 * Looks up the student's name and prints all attendance records, totals, and percentage.
 * @param registration_number The student's registration number.
 */
void generate_student_report(const char* registration_number) {
    const char *user_sql = "SELECT first_name, last_name FROM users WHERE registration_number = ?;";
    sqlite3_stmt *user_stmt;
    char full_name[2 * MAX_LENGTH] = "";

    if (sqlite3_prepare_v2(db, user_sql, -1, &user_stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_text(user_stmt, 1, registration_number, -1, SQLITE_STATIC);
        if (sqlite3_step(user_stmt) == SQLITE_ROW) {
            snprintf(full_name, sizeof(full_name), "%s %s",
                (const char *)sqlite3_column_text(user_stmt, 0),
                (const char *)sqlite3_column_text(user_stmt, 1));
        }
    }
    sqlite3_finalize(user_stmt);

    const char *sql = "SELECT date, status FROM attendance WHERE registration_number = ? ORDER BY date;";
    sqlite3_stmt *stmt;
    int total = 0, present = 0, absent = 0;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Error preparing statement (generate_student_report): %s\n", sqlite3_errmsg(db));
        return;
    }
    sqlite3_bind_text(stmt, 1, registration_number, -1, SQLITE_STATIC);

    printf("Attendance Report for: %s (%s)\n", full_name[0] ? full_name : "Unknown", registration_number);
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

/**
 * @brief Lists all registered students in the system.
 * Prints registration number and full name.
 */
void view_all_students() {
    const char *sql = "SELECT registration_number, first_name, last_name FROM users;";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Error preparing statement (view_all_students): %s\n", sqlite3_errmsg(db));
        return;
    }
    printf("All registered students:\n");
    printf("%-15s | %-20s\n", "Reg Number", "Name");
    printf("--------------------------------------------\n");
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const char *reg = (const char *)sqlite3_column_text(stmt, 0);
        const char *fname = (const char *)sqlite3_column_text(stmt, 1);
        const char *lname = (const char *)sqlite3_column_text(stmt, 2);
        char fullname[2 * MAX_LENGTH];
        snprintf(fullname, sizeof(fullname), "%s %s", fname, lname);
        printf("%-15s | %-20s\n", reg, fullname);
    }
    sqlite3_finalize(stmt);
}

/**
 * @brief Admin function: Lists all attendance records for all students.
 * Shows registration number, name, date, and status for each record.
 */
void view_all_attendance() {
    const char *sql = "SELECT u.registration_number, u.first_name, u.last_name, a.marked_at, a.status "
                      "FROM attendance a "
                      "JOIN users u ON a.user_id = u.id "
                      "ORDER BY a.marked_at;";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Error preparing statement (view_all_attendance): %s\n", sqlite3_errmsg(db));
        return;
    }
    printf("All attendance records:\n");
    printf("%-15s | %-20s | %-20s | %-10s\n", "Reg Number", "Name", "Date", "Status");
    printf("--------------------------------------------------------------------------\n");
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const char *reg = (const char *)sqlite3_column_text(stmt, 0);
        const char *fname = (const char *)sqlite3_column_text(stmt, 1);
        const char *lname = (const char *)sqlite3_column_text(stmt, 2);
        const char *date = (const char *)sqlite3_column_text(stmt, 3);
        int status = sqlite3_column_int(stmt, 4);
        char fullname[2 * MAX_LENGTH];
        snprintf(fullname, sizeof(fullname), "%s %s", fname, lname);
        printf("%-15s | %-20s | %-20s | %-10s\n", reg, fullname, date, (status == 1 ? "Present" : "Absent"));
    }
    sqlite3_finalize(stmt);
}
