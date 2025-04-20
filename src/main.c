#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "auth_functions.h"
#include "attendance.h"
#include "student_report.h"
#include "utils.h"
#include "database.h"

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
    char name[MAX_LENGTH];
    char reg_number[MAX_LENGTH];
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
                        if (login_user(name)) {
                            printf("Welcome, %s!\n", name);
                            logged_in = 1;
                            is_admin = 0;
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
            display_user_menu(is_admin);
            if (scanf("%d", &choice) != 1) {
                printf("Invalid input. Please enter a number.\n");
                clear_input_buffer();
                pause_and_clear();
                continue;
            }
            clear_input_buffer();
            if (is_admin) {
                switch (choice) {
                    case 1: {
                        printf("\n--- Mark Attendance ---\n");
                        char date[20];
                        int status;
                        printf("Enter registration number: ");
                        read_input(reg_number, sizeof(reg_number));
                        printf("Enter date (YYYY-MM-DD): ");
                        read_input(date, sizeof(date));
                        printf("Enter attendance status (1 for present, 0 for absent): ");
                        if (scanf("%d", &status) != 1 || (status != 0 && status != 1)) {
                            printf("Invalid status. Please enter 1 or 0.\n");
                            clear_input_buffer();
                            pause_and_clear();
                            break;
                        }
                        clear_input_buffer();
                        mark_attendance(reg_number, date, status);
                        pause_and_clear();
                        break;
                    }
                    case 2: {
                        printf("\n--- View Attendance ---\n");
                        printf("Enter registration number: ");
                        read_input(reg_number, sizeof(reg_number));
                        view_attendance(reg_number);
                        pause_and_clear();
                        break;
                    }
                    case 3: {
                        printf("\n--- Generate Student Report ---\n");
                        printf("Enter registration number: ");
                        read_input(reg_number, sizeof(reg_number));
                        generate_student_report(reg_number);
                        pause_and_clear();
                        break;
                    }
                    case 4:
                        printf("\n--- All Registered Students ---\n");
                        view_all_students();
                        pause_and_clear();
                        break;
                    case 5:
                        printf("\n--- All Attendance Records ---\n");
                        view_all_attendance();
                        pause_and_clear();
                        break;
                    case 6:
                        printf("Logged out successfully.\n");
                        logged_in = 0;
                        is_admin = 0;
                        pause_and_clear();
                        break;
                    case 7:
                        printf("Thank you for using the University Class Attendance System. Goodbye!\n");
                        close_database();
                        return 0;
                    default:
                        printf("Invalid choice. Please enter a number between 1 and 7.\n");
                        pause_and_clear();
                }
            } else {
                switch (choice) {
                    case 1: {
                        printf("\n--- Mark Attendance ---\n");
                        char date[20];
                        int status;
                        printf("Enter your registration number: ");
                        read_input(reg_number, sizeof(reg_number));
                        printf("Enter date (YYYY-MM-DD): ");
                        read_input(date, sizeof(date));
                        printf("Enter attendance status (1 for present, 0 for absent): ");
                        if (scanf("%d", &status) != 1 || (status != 0 && status != 1)) {
                            printf("Invalid status. Please enter 1 or 0.\n");
                            clear_input_buffer();
                            pause_and_clear();
                            break;
                        }
                        clear_input_buffer();
                        mark_attendance(reg_number, date, status);
                        pause_and_clear();
                        break;
                    }
                    case 2: {
                        printf("\n--- View Attendance ---\n");
                        printf("Enter your registration number: ");
                        read_input(reg_number, sizeof(reg_number));
                        view_attendance(reg_number);
                        pause_and_clear();
                        break;
                    }
                    case 3: {
                        printf("\n--- Generate Student Report ---\n");
                        printf("Enter your registration number: ");
                        read_input(reg_number, sizeof(reg_number));
                        generate_student_report(reg_number);
                        pause_and_clear();
                        break;
                    }
                    case 4:
                        printf("Logged out successfully.\n");
                        logged_in = 0;
                        pause_and_clear();
                        break;
                    case 5:
                        printf("Thank you for using the University Class Attendance System. Goodbye!\n");
                        close_database();
                        return 0;
                    default:
                        printf("Invalid choice. Please enter a number between 1 and 5.\n");
                        pause_and_clear();
                }
            }
        }
    }

    close_database();
    return 0;
}
