#include <stdio.h>
#include <string.h>
#include "auth_functions.h"
#include "attendance.h"
#include "student_report.h"
#include "utils.h"  // Include the utils header
#include "user_data.h"

#define ADMIN_USERNAME "admin"
#define ADMIN_PASSWORD "admin123"

void display_main_menu() {
    printf("\nWelcome to University Class Attendance System\n");
    printf("1. Register\n");
    printf("2. Login\n");
    printf("3. Exit\n");
    printf("Enter your choice: ");
}

void display_user_menu(int is_admin) {
    printf("\nUser Menu\n");
    printf("1. Mark Attendance\n");
    printf("2. View Attendance\n");
    printf("3. Generate Student Report\n");
    if (is_admin) {
        printf("4. View All Students (Admin)\n");
        printf("5. View All Attendance (Admin)\n");
        printf("6. Back\n");
        printf("7. Exit\n");
    } else {
        printf("4. Back\n");
        printf("5. Exit\n");
    }
    printf("Enter your choice: ");
}

int admin_login() {
    char username[MAX_LENGTH];
    char password[MAX_LENGTH];
    printf("Enter admin username: ");
    read_input(username, sizeof(username));
    printf("Enter admin password: ");
    read_input(password, sizeof(password));
    if (strcmp(username, ADMIN_USERNAME) == 0 && strcmp(password, ADMIN_PASSWORD) == 0) {
        return 1;
    }
    printf("Invalid admin credentials.\n");
    return 0;
}

int main() {
    int choice;
    char name[MAX_LENGTH];
    char reg_number[MAX_LENGTH];
    int logged_in = 0;
    int is_admin = 0;

    while (1) {
        if (!logged_in) {
            display_main_menu();
            scanf("%d", &choice);
            clear_input_buffer();  // Clear input buffer after reading choice
            switch (choice) {
                case 1:
                    handle_registration();  // Handle user registration
                    break;
                case 2: {
                    printf("Login as:\n1. User\n2. Admin\nEnter choice: ");
                    int login_type = 0;
                    scanf("%d", &login_type);
                    clear_input_buffer();  // Clear input buffer after reading login type
                    if (login_type == 1) {
                        if (login_user(name)) {
                            printf("Login successful!\n");
                            printf("Welcome %s\n", name);
                            logged_in = 1;
                            is_admin = 0;
                        } else {
                            printf("Invalid credentials.\n");
                        }
                    } else if (login_type == 2) {
                        if (admin_login()) {
                            printf("Admin login successful!\n");
                            logged_in = 1;
                            is_admin = 1;
                        }
                    } else {
                        printf("Invalid login type.\n");
                    }
                    break;
                }
                case 3:
                    printf("Exiting program.\n");
                    return 0;  // Exit the program
                default:
                    printf("Invalid choice. Please try again.\n");
            }
        } else {
            display_user_menu(is_admin);
            scanf("%d", &choice);
            clear_input_buffer();  // Clear input buffer after reading choice
            if (is_admin) {
                switch (choice) {
                    case 1: {
                        char date[20];
                        int status;
                        printf("Enter registration number: ");
                        read_input(reg_number, sizeof(reg_number));
                        printf("Enter date (YYYY-MM-DD): ");
                        read_input(date, sizeof(date));
                        printf("Enter attendance status (1 for present, 0 for absent): ");
                        scanf("%d", &status);
                        clear_input_buffer();  // Clear input buffer after reading status
                        mark_attendance(reg_number, date, status); // Call to mark attendance
                        break;
                    }
                    case 2: {
                        printf("Enter registration number: ");
                        read_input(reg_number, sizeof(reg_number));
                        view_attendance(reg_number);  // Call to view attendance
                        break;
                    }
                    case 3: {
                        printf("Enter registration number: ");
                        read_input(reg_number, sizeof(reg_number));
                        generate_student_report(reg_number);  // Call to generate student report
                        break;
                    }
                    case 4:
                        view_all_students();  // Call to view all students (Admin)
                        break;
                    case 5:
                        view_all_attendance();  // Call to view all attendance (Admin)
                        break;
                    case 6:
                        logged_in = 0;  // Go back to the main menu
                        is_admin = 0;
                        break;
                    case 7:
                        printf("Exiting program.\n");
                        return 0;  // Exit the program
                    default:
                        printf("Invalid choice. Please try again.\n");
                }
            } else {
                switch (choice) {
                    case 1: {
                        char date[20];
                        int status;
                        printf("Enter your registration number: ");
                        read_input(reg_number, sizeof(reg_number));
                        printf("Enter date (YYYY-MM-DD): ");
                        read_input(date, sizeof(date));
                        printf("Enter attendance status (1 for present, 0 for absent): ");
                        scanf("%d", &status);
                        clear_input_buffer();  // Clear input buffer after reading status
                        mark_attendance(reg_number, date, status); // Call to mark attendance
                        break;
                    }
                    case 2: {
                        printf("Enter your registration number: ");
                        read_input(reg_number, sizeof(reg_number));
                        view_attendance(reg_number);  // Call to view attendance
                        break;
                    }
                    case 3: {
                        printf("Enter your registration number: ");
                        read_input(reg_number, sizeof(reg_number));
                        generate_student_report(reg_number);  // Call to generate student report
                        break;
                    }
                    case 4:
                        logged_in = 0;  // Go back to the main menu
                        break;
                    case 5:
                        printf("Exiting program.\n");
                        return 0;  // Exit the program
                    default:
                        printf("Invalid choice. Please try again.\n");
                }
            }
        }
    }

    return 0;
}
