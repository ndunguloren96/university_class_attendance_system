# University Class Attendance System

This project is a simple C-based system for managing user registration, login, attendance, and password management for a university class attendance system.

## Features

- **User Registration**: Allows users to register with a unique registration number and password.
- **User Login**: Validates user credentials against stored data.
- **Password Reset**: Allows users to reset their password if they forget it.
- **Attendance Management**: Users can mark and view attendance.
- **Admin Features**: Admins can view all students and attendance records.
- **File-Based Storage**: User data and attendance records are stored in text files for simplicity.

## File Structure

- **`src/`**: Contains the source code files.
  - `main.c`: Entry point of the program.
  - `user_data.c`: Handles user data operations like registration, login, and password reset.
  - `auth_functions.c`: Provides helper functions for authentication.
  - `auth_system.c`: Implements the core authentication logic.
  - `attendance.c`: Handles attendance marking and viewing.
  - `student_report.c`: Handles student report generation and admin features.
  - `utils.c`: Provides utility functions like input handling.
- **`include/`**: Contains header files for the project.
- **`bin/`**: Stores the compiled executable and user data file.

## How to Build

1. Ensure you have GCC installed.
2. Run the following command to compile the project:
   ```bash
   make
   ```
3. The executable will be generated in the `bin/` directory.

## How to Run

1. Execute the program:
   ```bash
   ./bin/university_system
   ```
2. Follow the on-screen instructions to register, log in, reset your password, or manage attendance.

## License

This project is licensed under the MIT License. See the `LICENSE` file for details.
