#include "error_handling.h"
#include <cstdio>

void handle_sql_error(const char *query, const char *error_msg) {
    log_error("SQL Error:");
    log_error(query);
    log_error(error_msg);
    print_error("An error occurred while processing the database query. Please try again.");
}
