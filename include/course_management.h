#ifndef COURSE_MANAGEMENT_H
#define COURSE_MANAGEMENT_H

#define MAX_LENGTH 100

void add_course();
void list_courses();
void enroll_in_course(int user_id);
void list_enrollments(int user_id);
void add_session();
void list_sessions(int course_id);

#endif // COURSE_MANAGEMENT_H
