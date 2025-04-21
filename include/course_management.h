#ifndef COURSE_MANAGEMENT_H
#define COURSE_MANAGEMENT_H

#define MAX_LENGTH 100

void add_unit(int lecturer_id);
void list_units();
void enroll_in_unit(int student_id);
void list_enrollments(int user_id, const char *role);
void add_session(int lecturer_id);
void list_sessions(int unit_id);
void mark_attendance_session(int user_id, const char *role);

// Admin-only functions
void delete_unit();
void view_unit_details();

// Lecturer-only functions
void lecturer_view_enrolled_students(int lecturer_id);
void lecturer_view_attendance(int lecturer_id);

#endif // COURSE_MANAGEMENT_H
