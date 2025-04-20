#include "course_management.h"
#include "database.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void add_course(int instructor_id) {
    char course_code[MAX_LENGTH], course_name[MAX_LENGTH];
    printf("Enter course code: ");
    fgets(course_code, MAX_LENGTH, stdin);
    course_code[strcspn(course_code, "\n")] = '\0';
    printf("Enter course name: ");
    fgets(course_name, MAX_LENGTH, stdin);
    course_name[strcspn(course_name, "\n")] = '\0';

    const char *sql = "INSERT INTO courses (course_code, course_name, instructor_id) VALUES (?, ?, ?);";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Error preparing statement (add_course): %s\n", sqlite3_errmsg(db));
        return;
    }
    sqlite3_bind_text(stmt, 1, course_code, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, course_name, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, instructor_id);

    if (sqlite3_step(stmt) == SQLITE_DONE) {
        printf("Course added successfully!\n");
    } else {
        printf("Error: Could not add course. It may already exist.\n");
    }
    sqlite3_finalize(stmt);
}

void list_courses() {
    const char *sql = "SELECT c.id, c.course_code, c.course_name, u.first_name, u.last_name FROM courses c LEFT JOIN users u ON c.instructor_id = u.id;";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Error preparing statement (list_courses): %s\n", sqlite3_errmsg(db));
        return;
    }
    printf("%-5s | %-10s | %-30s | %-20s\n", "ID", "Code", "Name", "Instructor");
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
    sqlite3_finalize(stmt);
}

void enroll_in_course(int student_id) {
    list_courses();
    printf("Enter course ID to enroll: ");
    int course_id;
    scanf("%d", &course_id);
    clear_input_buffer();

    const char *sql = "INSERT INTO enrollments (user_id, course_id) VALUES (?, ?);";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Error preparing statement (enroll_in_course): %s\n", sqlite3_errmsg(db));
        return;
    }
    sqlite3_bind_int(stmt, 1, student_id);
    sqlite3_bind_int(stmt, 2, course_id);

    if (sqlite3_step(stmt) == SQLITE_DONE) {
        printf("Enrolled in course successfully!\n");
    } else {
        printf("Error: Could not enroll. You may already be enrolled.\n");
    }
    sqlite3_finalize(stmt);
}

void list_enrollments(int user_id, const char *role) {
    const char *sql_student = "SELECT c.id, c.course_code, c.course_name FROM enrollments e JOIN courses c ON e.course_id = c.id WHERE e.user_id = ?;";
    const char *sql_instructor = "SELECT id, course_code, course_name FROM courses WHERE instructor_id = ?;";
    sqlite3_stmt *stmt;
    const char *sql = (strcmp(role, "student") == 0) ? sql_student : sql_instructor;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Error preparing statement (list_enrollments): %s\n", sqlite3_errmsg(db));
        return;
    }
    sqlite3_bind_int(stmt, 1, user_id);
    printf("%-5s | %-10s | %-30s\n", "ID", "Code", "Name");
    printf("-----------------------------------------------\n");
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const char *code = (const char *)sqlite3_column_text(stmt, 1);
        const char *name = (const char *)sqlite3_column_text(stmt, 2);
        printf("%-5d | %-10s | %-30s\n", id, code, name);
    }
    sqlite3_finalize(stmt);
}

void add_session(int instructor_id) {
    list_courses();
    printf("Enter course ID to add session: ");
    int course_id;
    scanf("%d", &course_id);
    clear_input_buffer();

    printf("Enter session date (YYYY-MM-DD): ");
    char session_date[MAX_LENGTH];
    fgets(session_date, MAX_LENGTH, stdin);
    session_date[strcspn(session_date, "\n")] = '\0';

    printf("Enter session topic: ");
    char topic[MAX_LENGTH];
    fgets(topic, MAX_LENGTH, stdin);
    topic[strcspn(topic, "\n")] = '\0';

    // Check instructor is assigned to this course
    const char *check_sql = "SELECT COUNT(*) FROM courses WHERE id = ? AND instructor_id = ?;";
    sqlite3_stmt *check_stmt;
    sqlite3_prepare_v2(db, check_sql, -1, &check_stmt, NULL);
    sqlite3_bind_int(check_stmt, 1, course_id);
    sqlite3_bind_int(check_stmt, 2, instructor_id);
    int allowed = 0;
    if (sqlite3_step(check_stmt) == SQLITE_ROW) {
        allowed = sqlite3_column_int(check_stmt, 0);
    }
    sqlite3_finalize(check_stmt);
    if (!allowed) {
        printf("You are not the instructor for this course.\n");
        return;
    }

    const char *sql = "INSERT INTO sessions (course_id, session_date, topic) VALUES (?, ?, ?);";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Error preparing statement (add_session): %s\n", sqlite3_errmsg(db));
        return;
    }
    sqlite3_bind_int(stmt, 1, course_id);
    sqlite3_bind_text(stmt, 2, session_date, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, topic, -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_DONE) {
        printf("Session added successfully!\n");
    } else {
        printf("Error: Could not add session.\n");
    }
    sqlite3_finalize(stmt);
}

void list_sessions(int course_id) {
    const char *sql = "SELECT id, session_date, topic FROM sessions WHERE course_id = ? ORDER BY session_date;";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Error preparing statement (list_sessions): %s\n", sqlite3_errmsg(db));
        return;
    }
    sqlite3_bind_int(stmt, 1, course_id);
    printf("%-5s | %-12s | %-30s\n", "ID", "Date", "Topic");
    printf("-----------------------------------------------\n");
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const char *date = (const char *)sqlite3_column_text(stmt, 1);
        const char *topic = (const char *)sqlite3_column_text(stmt, 2);
        printf("%-5d | %-12s | %-30s\n", id, date, topic);
    }
    sqlite3_finalize(stmt);
}

void mark_attendance_session(int user_id, const char *role) {
    // List courses and sessions for the user
    if (strcmp(role, "student") == 0) {
        printf("Your enrolled courses:\n");
        list_enrollments(user_id, "student");
    } else if (strcmp(role, "instructor") == 0) {
        printf("Your courses:\n");
        list_enrollments(user_id, "instructor");
    }
    printf("Enter course ID: ");
    int course_id;
    scanf("%d", &course_id);
    clear_input_buffer();

    printf("Available sessions for this course:\n");
    list_sessions(course_id);
    printf("Enter session ID: ");
    int session_id;
    scanf("%d", &session_id);
    clear_input_buffer();

    int status = 1; // Present
    if (strcmp(role, "student") == 0) {
        // Student marks attendance
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
            printf("Attendance marked for session.\n");
        } else {
            printf("Error: Could not mark attendance.\n");
        }
        sqlite3_finalize(stmt);
    } else if (strcmp(role, "instructor") == 0) {
        // Instructor marks that they taught
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
            printf("Instructor marked as taught for session.\n");
        } else {
            printf("Error: Could not mark teaching session.\n");
        }
        sqlite3_finalize(stmt);
    }
}
