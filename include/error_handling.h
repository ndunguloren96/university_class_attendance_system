#ifndef ERROR_HANDLING_H
#define ERROR_HANDLING_H

void log_error(const char *msg);
void print_error(const char *msg);
void handle_sql_error(const char *query, const char *error_msg);

#endif
