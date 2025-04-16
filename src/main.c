#include <stdio.h>
#include "auth_functions.h"

int main() {
    int choice;

    while (1) {
        printf("\nTHE CO-OPERATIVE UNIVERSITY OF KENYA");
        printf("\nWelcome to University Class Attendance System\n");
        printf("1. Register\n");
        printf("2. Login\n");
        printf("3. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        clear_input_buffer();

        switch (choice) {
            case 1:
                handle_registration();  // Handle user registration
                break;
            case 2:
                handle_login();  // Handle user login
                break;
            case 3:
                printf("Exiting program.\n");
                return 0;  // Exit the program
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }

    return 0;
}
