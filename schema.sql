CREATE TABLE users (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    registration_number TEXT UNIQUE NOT NULL,
    password TEXT NOT NULL,
    first_name TEXT NOT NULL,
    last_name TEXT NOT NULL
);

CREATE TABLE attendance (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    registration_number TEXT NOT NULL,
    date TEXT NOT NULL,
    status INTEGER NOT NULL,
    FOREIGN KEY (registration_number) REFERENCES users (registration_number)
);

