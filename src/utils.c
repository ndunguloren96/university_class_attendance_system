/**
 * utils.c
 * Utility functions for input handling in the Copa (CUK) Attendance System.
 * Includes cross-platform input buffer clearing, safe string input, and password masking.
 * These are the little helpers that make the user experience smooth for students, lecs, and admins.
 */

#include "utils.h"
#include <stdio.h>
#include <string.h>
#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

/**
 * @brief Clears the standard input buffer.
 * 
 * Useful for removing leftover characters (like stray newlines) after scanf/fgets.
 * Ensures the next input is clean, especially after numeric input.
 */
void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/**
 * @brief Reads a line of input from stdin and removes the trailing newline.
 * 
 * @param buffer Pointer to the buffer where input will be stored.
 * @param length Maximum number of characters to read (including null terminator).
 */
void read_input(char *buffer, int length) {
    if (fgets(buffer, length, stdin)) {
        buffer[strcspn(buffer, "\n")] = '\0';
    }
}

/**
 * @brief Reads a password from the user, masking input with '*'.
 * 
 * Cross-platform: works on both Windows and Unix-like systems.
 * Ensures password is not shown on screen for privacy.
 * 
 * @param buffer Pointer to the buffer where the password will be stored.
 * @param length Maximum number of characters to read (including null terminator).
 */
void read_password(char *buffer, int length) {
    int i = 0;
    char ch;
    printf("Enter password: "); // Always print prompt here
    fflush(stdout);
#ifdef _WIN32
    // Windows: use _getch() for character-by-character input
    while ((ch = _getch()) != '\r' && ch != '\n' && i < length - 1) {
        if (ch == '\b' || ch == 127) {
            if (i > 0) {
                i--;
                printf("\b \b");
            }
        } else if (ch >= 32 && ch <= 126) {
            buffer[i++] = ch;
            printf("*");
        }
    }
#else
    // Unix/Linux: use termios to disable echo
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    while (read(STDIN_FILENO, &ch, 1) == 1 && ch != '\n' && ch != '\r' && i < length - 1) {
        if (ch == 127 || ch == '\b') {
            if (i > 0) {
                i--;
                printf("\b \b");
            }
        } else if (ch >= 32 && ch <= 126) {
            buffer[i++] = ch;
            printf("*");
        }
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
#endif
    buffer[i] = '\0';
    printf("\n");
}
