#include "utils.h"
#include <stdio.h>
#include <string.h>

void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void read_input(char *buffer, int length) {
    if (fgets(buffer, length, stdin)) {
        buffer[strcspn(buffer, "\n")] = '\0';
    }
}
