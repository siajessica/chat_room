#include <iostream>
#include <sqlite3.h>
#include <string>
#include "database.h"
#include "login_interface.h"
#include "relationship.h"
#include "message.h"

using namespace std;

int main(){
    sqlite3 *db = createDB();
    int rc = sqlite3_open("chatroom.db", &db);

    sign_up("Antonio", "123456789");
    sign_up("Jensen", "qwertyuiop");
    sign_up("Lala", "!@#$%^&*()");
    cout << "SIGN UP 3X" <<endl;
    string sql = "SELECT * FROM USERS;";
    rc = sqlite3_exec(db, sql.c_str(), print_select, 0, NULL);
    cout << endl;

    change_password("Lala", "a1b2c3");
    cout << "CHANGE PASSWORD" << endl;
    sql = "SELECT * FROM USERS;";
    rc = sqlite3_exec(db, sql.c_str(), print_select, 0, NULL);
    cout << endl;

    add_friend("Antonio", "Jensen");
    cout << "ADD ONE FRIEND" << endl;
    list_friends("Jensen");


    add_block("Antonio", "Jensen");
    add_block("Lala", "Jensen");
    add_block("Antonio", "Jensen");
    add_block("Antonio", "Lulu");
    cout << "ADD BLOCK" << endl;
    sql = "SELECT * FROM BLOCKED;";
    rc = sqlite3_exec(db, sql.c_str(), print_select, 0, NULL);
    cout << endl;

    add_friend("Jensen", "Antonio");
    add_friend("Antonio", "Lala");
    add_friend("Lala", "Antonio");
    add_friend("Jensen", "Lulu");
    sql = "SELECT * FROM FRIENDS;";
    rc = sqlite3_exec(db, sql.c_str(), print_select, 0, NULL);
    cout << endl;

    cout << "UNBLOCK" << endl;
    unblock("Lala", "Jensen");
    remove_friend("Lala", "Antonio");
    add_friend("Lala", "Jensen");
    sql = "SELECT * FROM FRIENDS;";
    rc = sqlite3_exec(db, sql.c_str(), print_select, 0, NULL);
    cout << endl;
    cout << "BLOCK TABLE" << endl;
    sql = "SELECT * FROM BLOCKED;";
    rc = sqlite3_exec(db, sql.c_str(), print_select, 0, NULL);
    cout << endl;

    record_message("Jensen", 3, "Hello");
    record_message("Anton", 1, "HAHAHA");
    record_message("Lala", 3, "How r u?");
    record_message("Anton", 3, "Finee");
    record_message("Kim", 1, "LOL");
    message_history(3);
    message_history(1);
}
