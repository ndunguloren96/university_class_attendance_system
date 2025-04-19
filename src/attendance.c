#include "attendance.h"
#include <stdio.h>
#include <string.h>

void mark_attendance(const char* registration_number, const char* date, int status) {
    FILE* file = fopen("attendance_data.txt", "a");
    if (file == NULL) {
        printf("Error opening attendance file.\n");
        return;
    }
    fprintf(file, "%s,%s,%d\n", registration_number, date, status);
    fclose(file);
    printf("Attendance marked successfully for %s on %s.\n", registration_number, date);
}

void view_attendance(const char* registration_number) {
    FILE* file = fopen("attendance_data.txt", "r");
    if (file == NULL) {
        printf("Error opening attendance file.\n");
        return;
    }

    char line[256];
    char reg_number[100], date[20];
    int status;
    printf("Attendance for %s:\n", registration_number);
    
    while (fgets(line, sizeof(line), file)) {
        sscanf(line, "%99[^,],%19[^,],%d", reg_number, date, &status);
        if (strcmp(reg_number, registration_number) == 0) {
            printf("Date: %s, Status: %s\n", date, (status == 1 ? "Present" : "Absent"));
        }
    }

    fclose(file);
}
