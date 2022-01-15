#include <iostream>
#include <string>
#include <sqlite3.h>
#include "database.h"

using namespace std;

int list_friends(string username){
    sqlite3 *db;
    int rc = sqlite3_open("chatroom.db", &db);
    if(rc != SQLITE_OK) cerr << "error opening database." << endl;

    string sql = "SELECT USER2 FROM FRIENDS WHERE USER1 = '" + username + "';";
    rc = sqlite3_exec(db, sql.c_str(), print_select, 0, NULL);
    sql = "SELECT USER1 FROM FRIENDS WHERE USER2 = '" + username + "';";
    rc = sqlite3_exec(db, sql.c_str(), print_select, 0, NULL);

    return 1;
}

//user1 is the person who is blocking, user2 is the person who is blocked
int list_block(string username){
    sqlite3 *db;
    int rc = sqlite3_open("chatroom.db", &db);
    if(rc != SQLITE_OK) cerr << "error opening database." << endl;

    string sql = "SELECT USER2 FROM BLOCKED WHERE USER1 = '" + username + "';";
    rc = sqlite3_exec(db, sql.c_str(), print_select, 0, NULL);

    return 1;
}

int remove_friend(string user1, string user2){
    sqlite3 *db;
    int rc = sqlite3_open("chatroom.db", &db);
    if(rc != SQLITE_OK) cerr << "error opening database." << endl;

    //check if both users exists
    int exist = 0;
    string sql = "SELECT * FROM USERS WHERE USERNAME = '" + user2 + "';";
    rc = sqlite3_exec(db, sql.c_str(), in_database, &exist, NULL);
    if(!exist){
        printf("User not found.\n");
        return 0;
    }

    //check if there exist any relation
    exist = 0;
    sql = "SELECT * FROM FRIENDS WHERE USER1 = '" + user1 + "' AND USER2 = '" + user2 + "';";
    rc = sqlite3_exec(db, sql.c_str(), in_database, &exist, NULL);
    if(!exist){
        sql = "SELECT * FROM FRIENDS WHERE USER1 = '" + user2 + "' AND USER2 = '" + user1 + "';";
        rc = sqlite3_exec(db, sql.c_str(), in_database, &exist, NULL);
    }
    else{
        sql = "DELETE FROM FRIENDS WHERE USER1 = '" + user1 + "' AND USER2 = '" + user2 + "';";
        rc = sqlite3_exec(db, sql.c_str(), NULL, 0, NULL);
        if(rc != SQLITE_OK) cerr << "error remove friend." << endl;
        else cout << "Friend deleted." << endl;
        return 1;
    }
    if(!exist){
        printf("No such friend found.\n");
        return 0;
    }
    else{
        sql = "DELETE FROM FRIENDS WHERE USER1 = '" + user2 + "' AND USER2 = '" + user1 + "';";
        rc = sqlite3_exec(db, sql.c_str(), NULL, 0, NULL);
        if(rc != SQLITE_OK) cerr << "error remove friend." << endl;
        else cout << "Friend deleted." << endl;
        return 1;
    }
    return 0;
}

//add friend
int add_friend(string user1, string user2){
    sqlite3 *db;
    int rc = sqlite3_open("chatroom.db", &db);
    if(rc != SQLITE_OK) cerr << "error opening database." << endl;

    //check if the person being added exists
    int exist = 0;
    string sql = "SELECT * FROM USERS WHERE USERNAME = '" + user2 + "';";
    rc = sqlite3_exec(db, sql.c_str(), in_database, &exist, NULL);
    if(!exist){
        printf("User not found.\n");
        return 0;
    }

    //check if the user is blocked
    exist = 0;
    sql = "SELECT * FROM BLOCKED WHERE USER1 = '" + user2 + "' AND USER2 = '" + user1 + "';";
    rc = sqlite3_exec(db, sql.c_str(), in_database, &exist, NULL);
    if(exist){
        printf("ERROR CANNOT ADD FRIEND: You have been blocked by the user.\n");
        return 0;
    }

    //check if the user is blocked
    exist = 0;
    sql = "SELECT * FROM BLOCKED WHERE USER1 = '" + user1 + "' AND USER2 = '" + user2 + "';";
    rc = sqlite3_exec(db, sql.c_str(), in_database, &exist, NULL);
    if(exist){
        printf("ERROR BLOCKED: Please unblock the user before adding as friend.\n");
        return 0;
    }

    //check if they are already friends
    exist = 0;
    sql = "SELECT * FROM FRIENDS WHERE USER1 = '" + user2 + "' AND USER2 ='" + user1 + "';";
    rc = sqlite3_exec(db, sql.c_str(), in_database, &exist, NULL);
    if(!exist){
	    sql = "SELECT * FROM FRIENDS WHERE USER1 = '" + user1 + "' AND USER2 = '" + user2 + "';";
	    rc = sqlite3_exec(db, sql.c_str(), in_database, &exist, NULL);
    }
    if(exist){
	    cout << "You are already friends." << endl;
	    return 1;
    }

    sql = "INSERT INTO FRIENDS (USER1, USER2) VALUES('"
                        + user1 + "', '" + user2 + "');";
    rc = sqlite3_exec(db, sql.c_str(), NULL, 0, NULL);
    if(rc != SQLITE_OK){
        cerr << "insert friend error." << endl;
        return 0;
    } else cout << "Friend Added." << endl;
    
    return 1;
}

//block people
int add_block(string user1, string user2){
    sqlite3 *db;
    int rc = sqlite3_open("chatroom.db", &db);
    if(rc != SQLITE_OK) cerr << "error opening database." << endl;

    //check if both users exists
    int exist = 0;
    string sql = "SELECT * FROM USERS WHERE USERNAME = '" + user2 + "';";
    rc = sqlite3_exec(db, sql.c_str(), in_database, &exist, NULL);
    if(!exist){
        printf("User not found.\n");
        return 0;
    }

    //check if already been blocked before
    exist = 0;
    sql = "SELECT * FROM BLOCKED WHERE USER1 = '" + user1 + "' AND USER2 = '" + user2 + "';";
    sqlite3_exec(db, sql.c_str(), in_database, &exist, NULL);
    if(exist){
	    cout << "User is already blocked." << endl;
	    return 0;
    }

    //remove friend relation
    remove_friend(user1, user2);
    sql = "INSERT INTO BLOCKED (USER1, USER2) VALUES('"
                        + user1 + "', '" + user2 + "');";
    rc = sqlite3_exec(db, sql.c_str(), NULL, 0, NULL);
    if(rc != SQLITE_OK){
        cerr << "insert block error." << endl;
        return 0;
    } else cout << "User blocked successfully." << endl;
    
    return 1;
}

int unblock(string user1, string user2){
    sqlite3 *db;
    int rc = sqlite3_open("chatroom.db", &db);
    if(rc != SQLITE_OK) cerr << "error opening database." << endl;

    //check if both users exists
    int exist = 0;
    string sql = "SELECT * FROM USERS WHERE USERNAME = '" + user2 + "';";
    rc = sqlite3_exec(db, sql.c_str(), in_database, &exist, NULL);
    if(!exist){
	    cout << "User not found." << endl;
	    return 0;
    }

    sql = "DELETE FROM BLOCKED WHERE USER1 = '" + user1 + "' AND USER2 = '" + user2 + "';";
    rc = sqlite3_exec(db, sql.c_str(), NULL, 0, NULL);
    if(rc != SQLITE_OK) cerr << "error remove friend." << endl;
    else cout << "User unblocked." << endl;
}
