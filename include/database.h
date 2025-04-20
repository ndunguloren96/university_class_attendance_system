#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>

extern sqlite3 *db;

int initialize_database();
void close_database();

#endif // DATABASE_H
