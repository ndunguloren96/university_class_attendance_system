#ifndef COURSE_MANAGEMENT_H
#define COURSE_MANAGEMENT_H

#define MAX_LENGTH 100

void add_course(int instructor_id);
void list_courses();
void enroll_in_course(int student_id);
void list_enrollments(int user_id, const char *role);
void add_session(int instructor_id);
void list_sessions(int course_id);
void mark_attendance_session(int user_id, const char *role);

#endif // COURSE_MANAGEMENT_H
