#include <iostream>
#include <string>
#include <sqlite3.h>
#include "database.h"

using namespace std;

int login(string username, string password){
    sqlite3 *db;
    int rc = sqlite3_open("chatroom.db", &db);
    if(rc != SQLITE_OK) cerr << "error opening database." << endl;

    int exist = 0;
    string sql = "SELECT * FROM USERS WHERE USERNAME = '" + username + "' AND PASSWORD = '" + password + "';";
    rc = sqlite3_exec(db, sql.c_str(), in_database, &exist, NULL);
    if(rc != SQLITE_OK) cerr << "select error." << endl;

    if(exist){
        sql = "UPDATE USERS SET STATUS = 1 WHERE USERNAME = '" + username + "';";
        rc = sqlite3_exec(db, sql.c_str(), NULL, 0, NULL);
        return 1;
    }
    else return 0;
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
    if(rc != SQLITE_OK) return 0;
    else cout << "Account created." << endl;

    return 1;
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
