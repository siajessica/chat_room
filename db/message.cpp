#include <iostream>
#include <string>
#include <sqlite3.h>
#include "database.h"

using namespace std;

int print_chat(void* NotUsed, int argc, char** argv, char** azColName){
	for (int i = 0; i < argc; i++) {
		cout << azColName[i] << ": " << argv[i] << endl;
	}
    cout << endl;
	return 0;
}

void message_history(int chat_id){
    sqlite3 *db;
    int rc = sqlite3_open("chatroom.db", &db);
    if(rc != SQLITE_OK) cerr << "error opening database." << endl;

    string sql = "SELECT SENDER, BODY, TIME FROM MESSAGE WHERE CHAT_ID = " + to_string(chat_id) + " ORDER BY DATE(TIME);";
    rc = sqlite3_exec(db, sql.c_str(), print_chat, 0, NULL);
    if(rc != SQLITE_OK) cout << "error loading chat history." << endl;
    return;
}

int record_message(string sender, int chat_id, string body){
    sqlite3 *db;
    int rc = sqlite3_open("chatroom.db", &db);
    if(rc != SQLITE_OK) cerr << "error opening database." << endl;

    string sql = "INSERT INTO MESSAGE (SENDER, CHAT_ID, BODY) VALUES('" + sender + "', " + to_string(chat_id) + " ,'" + body + "');";
    rc = sqlite3_exec(db, sql.c_str(), NULL, 0, NULL);
    if(rc != SQLITE_OK) return 0;
    return 1;
}