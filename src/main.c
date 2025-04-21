#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "auth_functions.h"
#include "attendance.h"
#include "student_report.h"
#include "utils.h"
#include "database.h"
#include "course_management.h"

#define ADMIN_USERNAME "admin"
#define ADMIN_PASSWORD "admin123"

void pause_and_clear() {
    printf("\nPress Enter to continue...");
    getchar();
    system("clear"); // On Windows use "cls"
}

void display_main_menu() {
    printf("\n============================================\n");
    printf("  THE CO-OPERATIVE UNIVERSITY OF KENYA\n");
    printf("============================================\n");
    printf("Welcome to University Class Attendance System\n");
    printf("--------------------------------------------\n");
    printf("1. Register\n");
    printf("2. Login\n");
    printf("3. Reset Password\n");
    printf("4. Exit\n");
    printf("--------------------------------------------\n");
    printf("Enter your choice: ");
}

void display_user_menu(int is_admin) {
    printf("\n============================================\n");
    printf("User Menu\n");
    printf("--------------------------------------------\n");
    printf("1. Mark Attendance\n");
    printf("2. View Attendance\n");
    printf("3. Generate Student Report\n");
    if (is_admin) {
        printf("4. View All Students (Admin)\n");
        printf("5. View All Attendance (Admin)\n");
        printf("6. Logout\n");
        printf("7. Exit\n");
    } else {
        printf("4. Logout\n");
        printf("5. Exit\n");
    }
    printf("--------------------------------------------\n");
    printf("Enter your choice: ");
}

int admin_login() {
    char username[MAX_LENGTH];
    char password[MAX_LENGTH];
    printf("\n--- Admin Login ---\n");
    printf("Enter admin username: ");
    read_input(username, sizeof(username));
    printf("Enter admin password: ");
    read_input(password, sizeof(password));
    if (strcmp(username, ADMIN_USERNAME) == 0 && strcmp(password, ADMIN_PASSWORD) == 0) {
        printf("Admin login successful!\n");
        return 1;
    }
    printf("Invalid admin credentials.\n");
    return 0;
}

int main() {
    if (!initialize_database()) {
        fprintf(stderr, "Failed to initialize database. Exiting.\n");
        return 1;
    }

    int choice;
    char first_name[MAX_LENGTH], last_name[MAX_LENGTH], role[MAX_LENGTH];
    char reg_number[MAX_LENGTH];
    int user_id = 0;
    int logged_in = 0;
    int is_admin = 0;

    while (1) {
        if (!logged_in) {
            display_main_menu();
            if (scanf("%d", &choice) != 1) {
                printf("Invalid input. Please enter a number.\n");
                clear_input_buffer();
                pause_and_clear();
                continue;
            }
            clear_input_buffer();
            switch (choice) {
                case 1:
                    printf("\n--- User Registration ---\n");
                    handle_registration();
                    pause_and_clear();
                    break;
                case 2: {
                    printf("\n--- Login ---\n");
                    printf("Login as:\n1. User\n2. Admin\nEnter choice: ");
                    int login_type = 0;
                    if (scanf("%d", &login_type) != 1) {
                        printf("Invalid input. Please enter a number.\n");
                        clear_input_buffer();
                        pause_and_clear();
                        continue;
                    }
                    clear_input_buffer();
                    if (login_type == 1) {
                        if (login_user(first_name, last_name, role)) {
                            // Fetch user_id
                            const char *sql = "SELECT id FROM users WHERE first_name = ? AND last_name = ? AND role = ?;";
                            sqlite3_stmt *stmt;
                            sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
                            sqlite3_bind_text(stmt, 1, first_name, -1, SQLITE_STATIC);
                            sqlite3_bind_text(stmt, 2, last_name, -1, SQLITE_STATIC);
                            sqlite3_bind_text(stmt, 3, role, -1, SQLITE_STATIC);
                            if (sqlite3_step(stmt) == SQLITE_ROW) {
                                user_id = sqlite3_column_int(stmt, 0);
                            }
                            sqlite3_finalize(stmt);

                            printf("Welcome, %s %s!\n", first_name, last_name);
                            logged_in = 1;
                            is_admin = (strcmp(role, "admin") == 0);
                        } else {
                            printf("Invalid credentials. Please try again.\n");
                        }
                    } else if (login_type == 2) {
                        if (admin_login()) {
                            logged_in = 1;
                            is_admin = 1;
                        }
                    } else {
                        printf("Invalid login type. Please enter 1 or 2.\n");
                    }
                    pause_and_clear();
                    break;
                }
                case 3:
                    printf("\n--- Reset Password ---\n");
                    reset_password();
                    pause_and_clear();
                    break;
                case 4:
                    printf("Thank you for using the University Class Attendance System. Goodbye!\n");
                    close_database();
                    return 0;
                default:
                    printf("Invalid choice. Please enter a number between 1 and 4.\n");
                    pause_and_clear();
            }
        } else {
            if (is_admin) {
                printf("\n--- Admin Menu ---\n");
                printf("1. Remove Student\n");
                printf("2. Delete Unit\n");
                printf("3. View All Students\n");
                printf("4. View All Units\n");
                printf("5. View Unit Details\n");
                printf("6. View All Attendance Records\n");
                printf("7. Logout\n");
                printf("8. Exit\n");
                printf("Enter your choice: ");
                if (scanf("%d", &choice) != 1) {
                    printf("Invalid input. Please enter a number.\n");
                    clear_input_buffer();
                    pause_and_clear();
                    continue;
                }
                clear_input_buffer();
                switch (choice) {
                    case 1:
                        printf("\n--- Remove Student ---\n");
                        remove_student();
                        pause_and_clear();
                        break;
                    case 2:
                        printf("\n--- Delete Unit ---\n");
                        delete_unit();
                        pause_and_clear();
                        break;
                    case 3:
                        printf("\n--- All Registered Students ---\n");
                        list_users("student");
                        pause_and_clear();
                        break;
                    case 4:
                        printf("\n--- All Units ---\n");
                        list_units();
                        pause_and_clear();
                        break;
                    case 5:
                        printf("\n--- Unit Details ---\n");
                        view_unit_details();
                        pause_and_clear();
                        break;
                    case 6:
                        printf("\n--- All Attendance Records ---\n");
                        view_all_attendance();
                        pause_and_clear();
                        break;
                    case 7:
                        printf("Logged out successfully.\n\n");
                        logged_in = 0;
                        is_admin = 0;
                        pause_and_clear();
                        break;
                    case 8:
                        printf("Thank you for using the University Class Attendance System. Goodbye!\n\n");
                        close_database();
                        return 0;
                    default:
                        printf("Invalid choice. Please enter a number between 1 and 8.\n\n");
                        pause_and_clear();
                }
            } else if (strcmp(role, "instructor") == 0) {
                printf("\n--- Instructor Menu ---\n");
                printf("1. Add Unit (e.g., unit code: CSC101)\n");
                printf("2. Add Session\n");
                printf("3. Mark Taught Session\n");
                printf("4. View My Units\n");
                printf("5. Logout\n");
                printf("6. Exit\n");
                printf("Enter your choice: ");
                if (scanf("%d", &choice) != 1) {
                    printf("Invalid input. Please enter a number.\n");
                    clear_input_buffer();
                    pause_and_clear();
                    continue;
                }
                clear_input_buffer();
                switch (choice) {
                    case 1:
                        printf("\n--- Add Unit ---\n");
                        add_unit(user_id);
                        pause_and_clear();
                        break;
                    case 2:
                        add_session(user_id);
                        pause_and_clear();
                        break;
                    case 3:
                        mark_attendance_session(user_id, role);
                        pause_and_clear();
                        break;
                    case 4:
                        list_enrollments(user_id, "instructor");
                        pause_and_clear();
                        break;
                    case 5:
                        printf("Logged out successfully.\n\n");
                        logged_in = 0;
                        pause_and_clear();
                        break;
                    case 6:
                        printf("Thank you for using the University Class Attendance System. Goodbye!\n\n");
                        close_database();
                        return 0;
                    default:
                        printf("Invalid choice.\n\n");
                        pause_and_clear();
                }
            } else if (strcmp(role, "student") == 0) {
                printf("\n--- Student Menu ---\n");
                printf("1. Enroll in Unit\n");
                printf("2. Mark Attendance\n");
                printf("3. View My Units\n");
                printf("4. Logout\n");
                printf("5. Exit\n");
                printf("Enter your choice: ");
                if (scanf("%d", &choice) != 1) {
                    printf("Invalid input. Please enter a number.\n");
                    clear_input_buffer();
                    pause_and_clear();
                    continue;
                }
                clear_input_buffer();
                switch (choice) {
                    case 1:
                        enroll_in_unit(user_id);
                        pause_and_clear();
                        break;
                    case 2:
                        mark_attendance_session(user_id, role);
                        pause_and_clear();
                        break;
                    case 3:
                        list_enrollments(user_id, "student");
                        pause_and_clear();
                        break;
                    case 4:
                        printf("Logged out successfully.\n\n");
                        logged_in = 0;
                        pause_and_clear();
                        break;
                    case 5:
                        printf("Thank you for using the University Class Attendance System. Goodbye!\n\n");
                        close_database();
                        return 0;
                    default:
                        printf("Invalid choice.\n\n");
                        pause_and_clear();
                }
            }
        }
    }

    close_database();
    return 0;
}
