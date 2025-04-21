#include "../include/error_handling.h"
#include <stdio.h>
#include <time.h>

void log_error(const char *msg) {
    FILE *f = fopen("logs/error.log", "a");
    if (!f) return;
    time_t now = time(NULL);
    fprintf(f, "[%s] %s\n", ctime(&now), msg);
    fclose(f);
}

void print_error(const char *msg) {
    printf("\033[1;31mError:\033[0m %s\n", msg); // Red color for errors
    log_error(msg);
}
