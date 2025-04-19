#include <stdio.h>
#include "auth_functions.h"
#include "attendance.h"
#include "student_report.h"
#include "utils.h"  // Include the utils header

int main() {
    int choice;

    while (1) {
        printf("\nWelcome to University Class Attendance System\n");
        printf("1. Register\n");
        printf("2. Login\n");
        printf("3. Mark Attendance\n");  // New option for marking attendance
        printf("4. View Attendance\n"); // New option for viewing attendance
        printf("5. Generate Student Report\n"); // New option for generating student report
        printf("6. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        clear_input_buffer();  // Clear input buffer after reading choice
        switch (choice) {
            case 1:
                handle_registration();  // Handle user registration
                break;
            case 2:
                handle_login();  // Handle user login
                break;
            case 3:
                {
                    char registration_number[100];
                    char date[20];
                    int status;
                    printf("Enter your registration number: ");
                    read_input(registration_number, sizeof(registration_number));
                    printf("Enter date (YYYY-MM-DD): ");
                    read_input(date, sizeof(date));
                    printf("Enter attendance status (1 for present, 0 for absent): ");
                    scanf("%d", &status);
                    clear_input_buffer();  // Clear input buffer after reading status
                    mark_attendance(registration_number, date, status); // Call to mark attendance
                }
                break;
            case 4:
                {
                    char registration_number[100];
                    printf("Enter your registration number: ");
                    read_input(registration_number, sizeof(registration_number));
                    view_attendance(registration_number);  // Call to view attendance
                }
                break;
            case 5:
                {
                    char registration_number[100];
                    printf("Enter your registration number: ");
                    read_input(registration_number, sizeof(registration_number));
                    generate_student_report(registration_number);  // Call to generate student report
                }
                break;
            case 6:
                printf("Exiting program.\n");
                return 0;  // Exit the program
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }

    return 0;
}
