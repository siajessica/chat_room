#include <iostream>
#include <string>
#include <sqlite3.h>

using namespace std;

sqlite3* createDB(){
   sqlite3 *db;
   char *msgErr;
   int  rc = sqlite3_open("chatroom.db", &db);
   if(rc != SQLITE_OK) cerr << "cannot create database" << endl;
   else cout << "database created." << endl;

   string sql = "PRAGMA FOREIGN_KEYS=ON";
   rc = sqlite3_exec(db, sql.c_str(), NULL, NULL, &msgErr);
   if(rc != SQLITE_OK) cerr << "cannot enable foreign key" << endl;
   else cout << "foreign key enabled." << endl;

   sql = "CREATE TABLE IF NOT EXISTS USERS("
                    "USERNAME   TEXT    NOT NULL    PRIMARY KEY,"
                    "PASSWORD   TEXT    NOT NULL);";
    rc = sqlite3_exec(db, sql.c_str(), NULL, 0, &msgErr);
    if(rc != SQLITE_OK) cerr << "cannot create users table" << endl;
    else cout << "users table created." << endl;

    sql = "CREATE TABLE IF NOT EXISTS FRIENDS("
            "USER1      TEXT        	NOT NULL,"
            "USER2      TEXT        	NOT NULL,"
            "CHAT_ID    INTEGER		PRIMARY KEY	AUTOINCREMENT);";
    rc = sqlite3_exec(db, sql.c_str(), NULL, 0, NULL);
    if(rc != SQLITE_OK) cerr << "cannot create friend table" << endl;
    else cout << "friends table created." << endl;

    //user1 is the person who is blocking, user2 is the person who is blocked
    sql = "CREATE TABLE IF NOT EXISTS BLOCKED("
            "USER1      TEXT        NOT NULL,"
            "USER2      TEXT        NOT NULL);";
    rc = sqlite3_exec(db, sql.c_str(), NULL, 0, NULL);
    if(rc != SQLITE_OK) cerr << "cannot create block table" << endl;
    else cout << "blocked table created." << endl;

    /*sql = "CREATE TABLE IF NOT EXISTS CHATS("
            "CHAT_ID        INTEGER     PRIMARY KEY	AUTOINCREMENT,"
            "NAME  	    TEXT     	NOT NULL,"
            "MEMBERS        TEXT        NOT NULL,"
            "NUM_MEMBER     INTEGER     NOT NULL);";
    rc = sqlite3_exec(db, sql.c_str(), NULL, 0, NULL);
    if(rc != SQLITE_OK) cerr << "cannot create chats table" << endl;
    else cout << "chats table created." << endl;*/

    sql = "CREATE TABLE IF NOT EXISTS MESSAGE("
            "SENDER     TEXT            NOT NULL,"
            "CHAT_ID    INTEGER         NOT NULL,"
            "BODY       TEXT            NOT NULL,"
            "TIME       TIMESTAMP       DEFAULT         CURRENT_TIMESTAMP       NOT NULL);";
    rc = sqlite3_exec(db, sql.c_str(), NULL, 0, NULL);
    if(rc != SQLITE_OK) cerr << "cannot create message table" << endl;
    else cout << "message table created." << endl;

   return db;
}

int in_database(void *numUser, int arg, char **colData, char **colName){
    int *flag = (int*)numUser;
    *flag = 1;
    return 1;
}

int print_select(void* NotUsed, int argc, char** argv, char** azColName){
	for (int i = 0; i < argc; i++) {
		cout << azColName[i] << ": " << argv[i] << endl;
	}

	return 0;
}
