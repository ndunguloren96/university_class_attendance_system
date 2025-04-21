#ifndef COURSE_MANAGEMENT_H
#define COURSE_MANAGEMENT_H

#define MAX_LENGTH 100

void add_unit(int instructor_id);
void list_units();
void enroll_in_unit(int student_id);
void list_enrollments(int user_id, const char *role);
void add_session(int instructor_id);
void list_sessions(int unit_id);
void mark_attendance_session(int user_id, const char *role);

// Admin-only functions
void delete_unit();
void view_unit_details();

#endif // COURSE_MANAGEMENT_H
