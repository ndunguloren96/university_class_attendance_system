#include "utils.h"
#include <stdio.h>
#include <string.h>
#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void read_input(char *buffer, int length) {
    if (fgets(buffer, length, stdin)) {
        buffer[strcspn(buffer, "\n")] = '\0';
    }
}

// Cross-platform masked password input (shows '*' for each char)
void read_password(char *buffer, int length) {
    int i = 0;
    char ch;
    printf("Enter password: "); // Always print prompt here
    fflush(stdout);
#ifdef _WIN32
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
