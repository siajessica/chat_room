#include <iostream>
#include <string>
#include <sqlite3.h>
#include "database.h"

using namespace std;

int userID;

int userid_callback(void* NotUsed, int argc, char** argv, char** azColName){
	userID = atoi(argv[0]);
	return 0;
}

int login(string username, string password){
    sqlite3 *db;
    int rc = sqlite3_open("chatroom.db", &db);
    if(rc != SQLITE_OK) cerr << "error opening database." << endl;

    userID = -1;;
    string sql = "SELECT ID FROM USERS WHERE USERNAME = '" + username + "' AND PASSWORD = '" + password + "';";
    rc = sqlite3_exec(db, sql.c_str(), userid_callback, 0, NULL);
    if(rc != SQLITE_OK) cout << "select error." << endl;

    if(userID != -1){
        return userID;
    }
    else return -1;
}

int logout(string username){
    sqlite3 *db;
    int rc = sqlite3_open("chatroom.db", &db);
    if(rc != SQLITE_OK) cerr << "error opening database." << endl;
    string sql = "UPDATE USERS SET STATUS = 0 WHERE USERNAME = '" + username + "';";
    rc = sqlite3_exec(db, sql.c_str(), NULL, 0, NULL);
    return 0;
}

int sign_up(string username, string password){
    sqlite3 *db;
    int rc = sqlite3_open("chatroom.db", &db);
    if(rc != SQLITE_OK) cerr << "error opening database." << endl;

    string sql = "INSERT INTO USERS (USERNAME, PASSWORD) VALUES('"
                    + username + "', '" + password + "');";
    rc = sqlite3_exec(db, sql.c_str(), NULL, 0, NULL);
    if(rc != SQLITE_OK) return -1;
    else cout << "Account created." << endl;

    sql = "SELECT ID FROM USERS WHERE USERNAME = '" + username + "' AND PASSWORD = '" + password + "';";
    rc = sqlite3_exec(db, sql.c_str(), NULL, 0, NULL);
    return userID;
}

int change_password(string username, string new_password){
    sqlite3 *db;
    int rc = sqlite3_open("chatroom.db", &db);
    if(rc != SQLITE_OK) cerr << "error opening database." << endl;

    string sql = "UPDATE USERS SET PASSWORD = '" + new_password + "' WHERE USERNAME = '" + username + "';";
    rc = sqlite3_exec(db, sql.c_str(), NULL, 0, NULL);
    if(rc != SQLITE_OK) cerr << "change password error." << endl;
    else cout << "Password changed." << endl;

    sqlite3_close(db);
    return 0;
}
