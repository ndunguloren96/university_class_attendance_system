#ifndef ATTENDANCE_H
#define ATTENDANCE_H

// Define an Attendance structure
typedef struct {
    char registration_number[100];
    char date[20]; // Could be YYYY-MM-DD format
    int status; // 1 for present, 0 for absent
} Attendance;

// Function to mark attendance
void mark_attendance(const char* registration_number, const char* date, int status);

// Function to get attendance for a specific student
void view_attendance(const char* registration_number);

#endif

