#include <iostream>
#include <string>

using namespace std;

#ifndef _LOGININTERFACE_H
#define _LOGININTERFACE_H

int userid_callback(void* NotUsed, int argc, char** argv, char** azColName);
int login(string username, string password);
int sign_up(string username, string password);
int change_password(string username, string new_password);

#endif