#ifndef ATTENDANCE_H
#define ATTENDANCE_H

// Define an Attendance structure
typedef struct {
    char registration_number[100];
    char date[20]; // Could be YYYY-MM-DD format
    int status; // 1 for present, 0 for absent
} Attendance;


