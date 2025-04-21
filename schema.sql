DROP TABLE IF EXISTS attendance;
DROP TABLE IF EXISTS sessions;
DROP TABLE IF EXISTS enrollments;
DROP TABLE IF EXISTS units;
DROP TABLE IF EXISTS users;
DROP TABLE IF EXISTS login_attempts;

-- USERS: All users (students, instructors, admins)
CREATE TABLE users (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    registration_number TEXT UNIQUE NOT NULL,
    password TEXT NOT NULL,
    salt TEXT NOT NULL,
    first_name TEXT NOT NULL,
    last_name TEXT NOT NULL,
    email TEXT UNIQUE NOT NULL,
    role TEXT NOT NULL CHECK(role IN ('student', 'instructor', 'admin')),
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP
);

-- UNITS: Each unit offered
CREATE TABLE units (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    unit_code TEXT UNIQUE NOT NULL,
    unit_name TEXT NOT NULL,
    instructor_id INTEGER,
    FOREIGN KEY (instructor_id) REFERENCES users(id)
);

-- ENROLLMENTS: Students enrolled in units
CREATE TABLE enrollments (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    user_id INTEGER NOT NULL,
    unit_id INTEGER NOT NULL,
    enrolled_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (user_id) REFERENCES users(id),
    FOREIGN KEY (unit_id) REFERENCES units(id),
    UNIQUE(user_id, unit_id)
);

-- SESSIONS: Each class session for a unit
CREATE TABLE sessions (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    unit_id INTEGER NOT NULL,
    session_code TEXT UNIQUE NOT NULL,
    session_date TEXT NOT NULL,
    topic TEXT,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (unit_id) REFERENCES units(id)
);

-- ATTENDANCE: Attendance per student per session
CREATE TABLE attendance (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    user_id INTEGER NOT NULL,
    session_id INTEGER NOT NULL,
    status INTEGER NOT NULL, -- 1=present, 0=absent
    marked_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (user_id) REFERENCES users(id),
    FOREIGN KEY (session_id) REFERENCES sessions(id),
    UNIQUE(user_id, session_id)
);

-- LOGIN ATTEMPTS: For account lockout
CREATE TABLE IF NOT EXISTS login_attempts (
    reg_number TEXT PRIMARY KEY,
    attempts INTEGER DEFAULT 0,
    last_attempt INTEGER,
    locked_until INTEGER DEFAULT 0
);

