#include <iostream>
#include <string>

using namespace std;

#ifndef _MESSAGE_H
#define _MESSAGE_H

int print_chat(void* NotUsed, int argc, char** argv, char** azColName);
void message_history(int chat_id);
int record_message(string sender, int chat_id, string body);

#endif