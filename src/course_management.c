#include "course_management.h"
#include "database.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void add_unit(int instructor_id) {
    char unit_code[MAX_LENGTH], unit_name[MAX_LENGTH];
    printf("\nEnter unit code (e.g., CSC101, MAT202): "); // Example for instructors
    fgets(unit_code, MAX_LENGTH, stdin);
    unit_code[strcspn(unit_code, "\n")] = '\0';
    printf("Enter unit name: ");
    fgets(unit_name, MAX_LENGTH, stdin);
    unit_name[strcspn(unit_name, "\n")] = '\0';

    const char *sql = "INSERT INTO units (unit_code, unit_name, instructor_id) VALUES (?, ?, ?);";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Error preparing statement (add_unit): %s\n", sqlite3_errmsg(db));
        return;
    }
    sqlite3_bind_text(stmt, 1, unit_code, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, unit_name, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, instructor_id);

    if (sqlite3_step(stmt) == SQLITE_DONE) {
        printf("Unit added successfully!\n\n");
    } else {
        printf("Error: Could not add unit. It may already exist.\n\n");
    }
    sqlite3_finalize(stmt);
}

void list_units() {
    const char *sql = "SELECT u.id, u.unit_code, u.unit_name, usr.first_name, usr.last_name FROM units u LEFT JOIN users usr ON u.instructor_id = usr.id;";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Error preparing statement (list_units): %s\n", sqlite3_errmsg(db));
        return;
    }
    printf("\n%-5s | %-10s | %-30s | %-20s\n", "ID", "Code", "Name", "Instructor");
    printf("---------------------------------------------------------------\n");
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const char *code = (const char *)sqlite3_column_text(stmt, 1);
        const char *name = (const char *)sqlite3_column_text(stmt, 2);
        const char *fname = (const char *)sqlite3_column_text(stmt, 3);
        const char *lname = (const char *)sqlite3_column_text(stmt, 4);
        char instructor[2 * MAX_LENGTH] = "";
        if (fname && lname) snprintf(instructor, sizeof(instructor), "%s %s", fname, lname);
        printf("%-5d | %-10s | %-30s | %-20s\n", id, code, name, instructor);
    }
    printf("\n");
    sqlite3_finalize(stmt);
}

void delete_unit() {
    list_units();
    printf("Enter the ID of the unit to delete: ");
    int unit_id;
    if (scanf("%d", &unit_id) != 1) {
        printf("Invalid input. Please enter a valid unit ID.\n");
        clear_input_buffer();
        return;
    }
    clear_input_buffer();

    // Delete enrollments, sessions, and attendance related to this unit
    const char *del_attendance = "DELETE FROM attendance WHERE session_id IN (SELECT id FROM sessions WHERE unit_id = ?);";
    const char *del_sessions = "DELETE FROM sessions WHERE unit_id = ?;";
    const char *del_enrollments = "DELETE FROM enrollments WHERE unit_id = ?;";
    const char *del_unit = "DELETE FROM units WHERE id = ?;";

    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, del_attendance, -1, &stmt, NULL);
    sqlite3_bind_int(stmt, 1, unit_id);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    sqlite3_prepare_v2(db, del_sessions, -1, &stmt, NULL);
    sqlite3_bind_int(stmt, 1, unit_id);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    sqlite3_prepare_v2(db, del_enrollments, -1, &stmt, NULL);
    sqlite3_bind_int(stmt, 1, unit_id);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    sqlite3_prepare_v2(db, del_unit, -1, &stmt, NULL);
    sqlite3_bind_int(stmt, 1, unit_id);
    if (sqlite3_step(stmt) == SQLITE_DONE && sqlite3_changes(db) > 0) {
        printf("Unit deleted successfully.\n");
    } else {
        printf("Error: Could not delete unit. Check the ID.\n");
    }
    sqlite3_finalize(stmt);
}

void view_unit_details() {
    list_units();
    printf("Enter the ID of the unit to view details: ");
    int unit_id;
    if (scanf("%d", &unit_id) != 1) {
        printf("Invalid input. Please enter a valid unit ID.\n");
        clear_input_buffer();
        return;
    }
    clear_input_buffer();

    // Get unit info and instructor
    const char *unit_sql = "SELECT unit_code, unit_name, usr.first_name, usr.last_name FROM units u LEFT JOIN users usr ON u.instructor_id = usr.id WHERE u.id = ?;";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, unit_sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Error preparing statement (unit_sql): %s\n", sqlite3_errmsg(db));
        return;
    }
    sqlite3_bind_int(stmt, 1, unit_id);
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const char *code = (const char *)sqlite3_column_text(stmt, 0);
        const char *name = (const char *)sqlite3_column_text(stmt, 1);
        const char *fname = (const char *)sqlite3_column_text(stmt, 2);
        const char *lname = (const char *)sqlite3_column_text(stmt, 3);
        printf("\nUnit Code: %s\nUnit Name: %s\nInstructor: %s %s\n", code, name, fname ? fname : "-", lname ? lname : "-");
    } else {
        printf("Unit not found.\n");
        sqlite3_finalize(stmt);
        return;
    }
    sqlite3_finalize(stmt);

    // Count enrolled students
    const char *count_sql = "SELECT COUNT(*) FROM enrollments WHERE unit_id = ?;";
    if (sqlite3_prepare_v2(db, count_sql, -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, unit_id);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            int count = sqlite3_column_int(stmt, 0);
            printf("Number of enrolled students: %d\n", count);
        }
        sqlite3_finalize(stmt);
    }

    // List taught sessions
    const char *session_sql = "SELECT session_code, session_date, topic FROM sessions WHERE unit_id = ? ORDER BY session_date;";
    if (sqlite3_prepare_v2(db, session_sql, -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, unit_id);
        printf("Taught Sessions:\n%-12s | %-12s | %-30s\n", "Session Code", "Date", "Topic");
        printf("---------------------------------------------------------------\n");
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            const char *scode = (const char *)sqlite3_column_text(stmt, 0);
            const char *sdate = (const char *)sqlite3_column_text(stmt, 1);
            const char *topic = (const char *)sqlite3_column_text(stmt, 2);
            printf("%-12s | %-12s | %-30s\n", scode, sdate, topic);
        }
        sqlite3_finalize(stmt);
    }
}

void enroll_in_unit(int student_id) {
    list_units();
    printf("Enter unit ID to enroll: ");
    int unit_id;
    scanf("%d", &unit_id);
    clear_input_buffer();

    const char *sql = "INSERT INTO enrollments (user_id, unit_id) VALUES (?, ?);";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Error preparing statement (enroll_in_unit): %s\n", sqlite3_errmsg(db));
        return;
    }
    sqlite3_bind_int(stmt, 1, student_id);
    sqlite3_bind_int(stmt, 2, unit_id);

    if (sqlite3_step(stmt) == SQLITE_DONE) {
        printf("Enrolled in unit successfully!\n\n");
    } else {
        printf("Error: Could not enroll. You may already be enrolled.\n\n");
    }
    sqlite3_finalize(stmt);
}

void list_enrollments(int user_id, const char *role) {
    const char *sql_student = "SELECT u.id, u.unit_code, u.unit_name FROM enrollments e JOIN units u ON e.unit_id = u.id WHERE e.user_id = ?;";
    const char *sql_instructor = "SELECT id, unit_code, unit_name FROM units WHERE instructor_id = ?;";
    sqlite3_stmt *stmt;
    const char *sql = (strcmp(role, "student") == 0) ? sql_student : sql_instructor;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Error preparing statement (list_enrollments): %s\n", sqlite3_errmsg(db));
        return;
    }
    sqlite3_bind_int(stmt, 1, user_id);
    printf("\n%-5s | %-10s | %-30s\n", "ID", "Code", "Name");
    printf("-----------------------------------------------\n");
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const char *code = (const char *)sqlite3_column_text(stmt, 1);
        const char *name = (const char *)sqlite3_column_text(stmt, 2);
        printf("%-5d | %-10s | %-30s\n", id, code, name);
    }
    printf("\n");
    sqlite3_finalize(stmt);
}

void add_session(int instructor_id) {
    list_units();
    printf("Enter unit ID to add session (see 'ID' column above, not the code): ");
    int unit_id;
    if (scanf("%d", &unit_id) != 1) {
        printf("Invalid input. Please enter the numeric ID from the 'ID' column.\n\n");
        clear_input_buffer();
        return;
    }
    clear_input_buffer();

    // Check instructor is assigned to this unit
    const char *check_sql = "SELECT COUNT(*) FROM units WHERE id = ? AND instructor_id = ?;";
    sqlite3_stmt *check_stmt;
    sqlite3_prepare_v2(db, check_sql, -1, &check_stmt, NULL);
    sqlite3_bind_int(check_stmt, 1, unit_id);
    sqlite3_bind_int(check_stmt, 2, instructor_id);
    int allowed = 0;
    if (sqlite3_step(check_stmt) == SQLITE_ROW) {
        allowed = sqlite3_column_int(check_stmt, 0);
    }
    sqlite3_finalize(check_stmt);
    if (!allowed) {
        printf("You are not the instructor for this unit.\n\n");
        return;
    }

    printf("Enter session code (unique, e.g. LESSON1): ");
    char session_code[MAX_LENGTH];
    fgets(session_code, MAX_LENGTH, stdin);
    session_code[strcspn(session_code, "\n")] = '\0';

    printf("Enter session date (YYYY-MM-DD): ");
    char session_date[MAX_LENGTH];
    fgets(session_date, MAX_LENGTH, stdin);
    session_date[strcspn(session_date, "\n")] = '\0';

    printf("Enter session topic: ");
    char topic[MAX_LENGTH];
    fgets(topic, MAX_LENGTH, stdin);
    topic[strcspn(topic, "\n")] = '\0';

    const char *sql = "INSERT INTO sessions (unit_id, session_code, session_date, topic) VALUES (?, ?, ?, ?);";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Error preparing statement (add_session): %s\n", sqlite3_errmsg(db));
        return;
    }
    sqlite3_bind_int(stmt, 1, unit_id);
    sqlite3_bind_text(stmt, 2, session_code, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, session_date, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, topic, -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_DONE) {
        printf("Session added successfully!\n\n");
    } else {
        printf("Error: Could not add session.\n\n");
    }
    sqlite3_finalize(stmt);
}

void list_sessions(int unit_id) {
    const char *sql = "SELECT id, session_code, session_date, topic FROM sessions WHERE unit_id = ? ORDER BY session_date;";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Error preparing statement (list_sessions): %s\n", sqlite3_errmsg(db));
        return;
    }
    sqlite3_bind_int(stmt, 1, unit_id);
    printf("\n%-5s | %-10s | %-12s | %-30s\n", "ID", "Code", "Date", "Topic");
    printf("---------------------------------------------------------------\n");
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const char *code = (const char *)sqlite3_column_text(stmt, 1);
        const char *date = (const char *)sqlite3_column_text(stmt, 2);
        const char *topic = (const char *)sqlite3_column_text(stmt, 3);
        printf("%-5d | %-10s | %-12s | %-30s\n", id, code, date, topic);
    }
    printf("\n");
    sqlite3_finalize(stmt);
}

void mark_attendance_session(int user_id, const char *role) {
    if (strcmp(role, "student") == 0) {
        printf("Your enrolled units:\n");
        list_enrollments(user_id, "student");
    } else if (strcmp(role, "instructor") == 0) {
        printf("Your units:\n");
        list_enrollments(user_id, "instructor");
    }
    printf("Enter unit ID: ");
    int unit_id;
    scanf("%d", &unit_id);
    clear_input_buffer();

    printf("Available sessions for this unit:\n");
    list_sessions(unit_id);
    printf("Enter session code: ");
    char session_code[MAX_LENGTH];
    fgets(session_code, MAX_LENGTH, stdin);
    session_code[strcspn(session_code, "\n")] = '\0';

    // Get session_id from session_code and unit_id
    const char *find_session_sql = "SELECT id FROM sessions WHERE session_code = ? AND unit_id = ?;";
    sqlite3_stmt *find_stmt;
    if (sqlite3_prepare_v2(db, find_session_sql, -1, &find_stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Error preparing statement (find_session): %s\n", sqlite3_errmsg(db));
        return;
    }
    sqlite3_bind_text(find_stmt, 1, session_code, -1, SQLITE_STATIC);
    sqlite3_bind_int(find_stmt, 2, unit_id);
    int session_id = 0;
    if (sqlite3_step(find_stmt) == SQLITE_ROW) {
        session_id = sqlite3_column_int(find_stmt, 0);
    }
    sqlite3_finalize(find_stmt);

    if (session_id == 0) {
        printf("Invalid session code for this unit.\n\n");
        return;
    }

    int status = 1; // Present
    const char *sql = "INSERT OR REPLACE INTO attendance (user_id, session_id, status) VALUES (?, ?, ?);";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Error preparing statement (mark_attendance_session): %s\n", sqlite3_errmsg(db));
        return;
    }
    sqlite3_bind_int(stmt, 1, user_id);
    sqlite3_bind_int(stmt, 2, session_id);
    sqlite3_bind_int(stmt, 3, status);
    if (sqlite3_step(stmt) == SQLITE_DONE) {
        if (strcmp(role, "student") == 0)
            printf("Attendance marked for session.\n\n");
        else
            printf("Instructor marked as taught for session.\n\n");
    } else {
        printf("Error: Could not mark attendance.\n\n");
    }
    sqlite3_finalize(stmt);
}
