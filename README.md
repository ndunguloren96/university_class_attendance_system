# University Class Attendance System

This project is a scalable, user-friendly C-based backend for managing university class attendance, supporting multiple units, lecturers, and roles.
This is our school project.

## Features

- **User Registration**: Register with first/last name, registration number, email, password, and role (student/lecturer/admin).
- **User Login**: Personalized greetings using full name.
- **Password Reset**: Secure password reset with confirmation.
- **Unit Management**: Admins/lecturers can add and manage units.
- **Enrollment**: Students enroll in units.
- **Session Management**: Lecturers create sessions for units.
- **Attendance Management**: Mark and view attendance per session.
- **Reports**: Generate detailed attendance reports per student, unit, or session.
- **Admin Features**: View all users, units, sessions, and attendance.
- **Database Storage**: All data is stored in an SQLite database (`university_attendance.db`).

## Database Schema

- **users**: id, registration_number, password, first_name, last_name, email, role, created_at
- **units**: id, unit_code, unit_name, lecturer_id
- **enrollments**: id, user_id, unit_id, enrolled_at
- **sessions**: id, unit_id, session_date, topic, created_at
- **attendance**: id, user_id, session_id, status, marked_at

## How to Build and Run

1. Ensure you have GCC and SQLite development libraries installed.
2. Clone this repo:
   ```bash
   git clone https://github.com/ndunguloren96/university_class_attendance_system
   ```
   or you can fork it and clone it from your GitHub Account.
4. Create the database:
   ```bash
   sqlite3 university_attendance.db < schema.sql
   ```
5. Build and run:
   ```bash
   make clean
   make
   ./bin/university_system
   ```
   Checkout screenshots for what to expect when the codebase is running correctly.
   
## Other tools you might need:
1. WSL for Windows

## Collaboration
If interested, you can continue with the development of this project. 

## License

MIT License
