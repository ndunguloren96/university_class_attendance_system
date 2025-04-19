#include "student_report.h"
#include <stdio.h>
#include <string.h>
#include "user_data.h"

// Simple student report (stub)
void generate_student_report(const char* registration_number) {
    printf("Generating report for student: %s\n", registration_number);
    // Implement actual report logic here
}

// Admin: view all students
void view_all_students() {
    FILE *file = fopen("./bin/user_data.txt", "r");
    if (!file) {
        printf("Could not open user data file.\n");
        return;
    }
    char reg[MAX_LENGTH], pass[MAX_LENGTH];
    printf("All registered students:\n");
    while (fscanf(file, "%s %s", reg, pass) == 2) {
        printf("Registration Number: %s\n", reg);
    }
    fclose(file);
}

// Admin: view all attendance
void view_all_attendance() {
    FILE *file = fopen("attendance_data.txt", "r");
    if (!file) {
        printf("Could not open attendance data file.\n");
        return;
    }
    char line[256];
    printf("All attendance records:\n");
    while (fgets(line, sizeof(line), file)) {
        printf("%s", line);
    }
    fclose(file);
}
