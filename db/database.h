#include <iostream>
#include <sqlite3.h>

#ifndef _DATABASE_H
#define _DATABASE_H

sqlite3* createDB();
int in_database(void *numUser, int arg, char **colData, char **colName);
int print_select(void* NotUsed, int argc, char** argv, char** azColName);

#endif
