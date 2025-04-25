/**
 * main.c
 * Copa (CUK) Class Attendance System - Main Engine
 * Handles all the main menu logic, login, registration, and role-based navigation.
 * Proudly crafted for The Co-operative University of Kenya (CUK) by a fellow coder.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>      // For lockout countdowns
#include <unistd.h>    // For sleep() on Unix/Linux
#include "auth_functions.h"
#include "attendance.h"
#include "student_report.h"
#include "utils.h"
#include "database.h"
#include "course_management.h"
#include "security_utils.h"
#include "error_handling.h"

// Copa-wide constants for a bit of school pride
#define COPA_SCHOOL "The Co-operative University of Kenya (CUK)"
#define COPA_SHORT "Copa"

// External helpers (from other .c files)
extern int is_strong_password(const char *password);
extern void print_success(const char *msg);

// Pause and clear the screen for a fresh start (Copa style!)
void pause_and_clear() {
    // Give the user a breather before the next screen
    printf("\nPress Enter to continue...");
    getchar();
    system("clear"); // On Windows, swap for "cls" if you want
}

// Welcome menu for everyone (students, lecs, admins, even guests!)
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

// Menu after login, with options based on your Copa role
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

// Admin login with lockout and password check (no shortcuts for Copa admins!)
int admin_login() {
    // This function is for the Copa admins only. If you mess up your password 3 times, you get a timeout.
    char reg_number[MAX_LENGTH];
    char email[MAX_LENGTH];
    char password[MAX_LENGTH];
    char db_email[MAX_LENGTH], db_hash[HASH_SIZE], db_salt[SALT_SIZE];
    int found = 0;
    int attempts = 0;
    printf("\n--- Admin Login ---\n");
    printf("Enter admin registration number: ");
    read_input(reg_number, sizeof(reg_number));
    printf("Enter admin email: ");
    read_input(email, sizeof(email));

    // Too many wrong tries? Chill for a bit!
    if (is_account_locked(reg_number)) {
        print_error("Account is locked due to too many failed login attempts. Please try again later.");
        return 0;
    }

    // 3 strikes and you're out (for a while)
    while (attempts < 3) {
        read_password(password, sizeof(password));

        // Check admin details in DB
        const char *sql = "SELECT email, password, salt FROM users WHERE registration_number = ? AND role = 'admin';";
        sqlite3_stmt *stmt;
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
            print_error("Database error.");
            return 0;
        }
        sqlite3_bind_text(stmt, 1, reg_number, -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) == SQLITE_ROW) {
            strncpy(db_email, (const char *)sqlite3_column_text(stmt, 0), MAX_LENGTH-1);
            strncpy(db_hash, (const char *)sqlite3_column_text(stmt, 1), HASH_SIZE-1);
            strncpy(db_salt, (const char *)sqlite3_column_text(stmt, 2), SALT_SIZE-1);
            db_email[MAX_LENGTH-1] = db_hash[HASH_SIZE-1] = db_salt[SALT_SIZE-1] = '\0';
            if (strcmp(email, db_email) == 0 && verify_password(password, db_salt, db_hash)) {
                found = 1;
                reset_login_attempts(reg_number);
            } else {
                record_failed_login(reg_number);
            }
        } else {
            record_failed_login(reg_number);
        }
        sqlite3_finalize(stmt);

        if (found) {
            print_success("Admin login successful!");
            return 1;
        } else {
            attempts++;
            if (attempts < 3)
                print_error("Invalid admin credentials.");
        }
    }

    // 3 fails: lockout and dramatic countdown
    print_error("Too many failed attempts. Please wait 30 seconds before trying again.");
    // Lockout for 30 seconds in DB
    const char *lock_sql = "UPDATE login_attempts SET locked_until = strftime('%%s','now') + 30 WHERE reg_number = ?;";
    sqlite3_stmt *lock_stmt;
    if (sqlite3_prepare_v2(db, lock_sql, -1, &lock_stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_text(lock_stmt, 1, reg_number, -1, SQLITE_STATIC);
        sqlite3_step(lock_stmt);
        sqlite3_finalize(lock_stmt);
    }

    // Live countdown (red, for drama)
    for (int i = 30; i > 0; --i) {
        printf("\r\033[1;31mLocked out. Please wait %2d seconds... \033[0m", i);
        fflush(stdout);
#ifdef _WIN32
        Sleep(1000);
#else
        sleep(1);
#endif
    }
    printf("\r\033[1;32mYou may now try again. Press Enter to continue...\033[0m      \n");
    fflush(stdout);
    getchar(); // Wait for Enter
    return 0;
}

// Main Copa loop: registration, login, and all the fun stuff
int main() {
    // Welcome to the Copa backend! Let's get this show on the road.
    if (!initialize_database()) {
        fprintf(stderr, "Failed to initialize database. Exiting.\n");
        return 1;
    }

    int choice;
    char first_name[MAX_LENGTH], last_name[MAX_LENGTH], role[MAX_LENGTH];
    int user_id = 0;
    int logged_in = 0;
    int is_admin = 0;

    // Copa's main event loop
    while (1) {
        if (!logged_in) {
            // Not logged in? Here's your Copa menu
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
                            // Fetch user_id from DB
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

                            print_success("Login successful!");
                            printf("Welcome, %s %s!\n", first_name, last_name);
                            logged_in = 1;
                            is_admin = (strcmp(role, "admin") == 0);
                        } else {
                            print_error("Invalid credentials. Please try again.");
                        }
                    } else if (login_type == 2) {
                        if (admin_login()) {
                            logged_in = 1;
                            is_admin = 1;
                        }
                    } else {
                        print_error("Invalid login type. Please enter 1 or 2.");
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
            // You're logged in! Let's see what you can do...
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
                // Lec menu - for our beloved Copa lecs
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
                // Student menu - Copa students, this is your playground
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
